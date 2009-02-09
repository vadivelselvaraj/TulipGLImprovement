#include "QuantitativeParallelAxis.h"
#include "AxisConfigDialogs.h"

#include <tulip/IntegerProperty.h>
#include <tulip/DoubleProperty.h>

#include <cmath>

using namespace std;

namespace tlp {

QuantitativeParallelAxis::QuantitativeParallelAxis(const Coord &baseCoord, const float height, const float axisAreaWidth, ParallelCoordinatesGraphProxy *graphProxy,
													const string &graphPropertyName, const bool ascendingOrder, const Color &axisColor) :
  ParallelAxis(new GlQuantitativeAxis(graphPropertyName, baseCoord, height, VERTICAL_AXIS, axisColor, true, ascendingOrder), axisAreaWidth),
  nbAxisGrad(DEFAULT_NB_AXIS_GRAD), graphProxy(graphProxy), log10Scale(false) {
  glQuantitativeAxis = dynamic_cast<GlQuantitativeAxis *>(glAxis);
  boxPlotValuesCoord.resize(5);
  boxPlotStringValues.resize(5);
  redraw();
}

void QuantitativeParallelAxis::setAxisLabels() {
	double min = getAxisMinValue();
	double max = getAxisMaxValue();
	glQuantitativeAxis->setAxisParameters(min, max, nbAxisGrad, RIGHT_OR_ABOVE, true);
	glQuantitativeAxis->setLogScale(log10Scale);
}

void QuantitativeParallelAxis::computeBoxPlotCoords() {

	set<double> propertyValuesSet;
	Iterator<unsigned int> *dataIt = graphProxy->getDataIterator();
	while (dataIt->hasNext()) {
		unsigned int dataId = dataIt->next();
		double value;
		if (getAxisDataTypeName() == "double") {
			value = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType> (getAxisName(), dataId);
		} else {
			value = (double) graphProxy->getPropertyValueForData<IntegerProperty, IntegerType> (getAxisName(), dataId);
		}
		propertyValuesSet.insert(value);
	}
	delete dataIt;

	vector<double> propertyValuesVector(propertyValuesSet.begin(), propertyValuesSet.end());
	unsigned int vectorSize = propertyValuesVector.size();

	if (vectorSize < 4) {

		boxPlotValuesCoord[BOTTOM_OUTLIER] = Coord(-1,-1,-1);
		boxPlotValuesCoord[FIRST_QUARTILE] = Coord(-1,-1,-1);
		boxPlotValuesCoord[MEDIAN] = Coord(-1,-1,-1);
		boxPlotValuesCoord[THIRD_QUARTILE] = Coord(-1,-1,-1);
		boxPlotValuesCoord[TOP_OUTLIER] = Coord(-1,-1,-1);

		boxPlotStringValues[BOTTOM_OUTLIER] = "KO";
		boxPlotStringValues[FIRST_QUARTILE] = "KO";
		boxPlotStringValues[MEDIAN] = "KO";
		boxPlotStringValues[THIRD_QUARTILE] = "KO";
		boxPlotStringValues[TOP_OUTLIER] = "KO";

	} else {


		double median;
		if (vectorSize % 2 == 1) {
			median = propertyValuesVector[vectorSize / 2.];
		} else {
			median = (propertyValuesVector[(vectorSize / 2.) - 1] + propertyValuesVector[vectorSize / 2.]) / 2.;
		}

		double firstQuartile;
		if (vectorSize % 2 == 1) {
			firstQuartile = propertyValuesVector[vectorSize / 4.];
		} else {
			firstQuartile = (propertyValuesVector[(vectorSize / 4.) - 1] + propertyValuesVector[vectorSize / 4.]) / 2;
		}

		double thirdQuartile;
		if (vectorSize % 2 == 1) {
			thirdQuartile = propertyValuesVector[3 *(vectorSize / 4.)];
		} else {
			thirdQuartile = (propertyValuesVector[3 *(vectorSize / 4.) - 1] + propertyValuesVector[3 *(vectorSize / 4.)]) / 2.;
		}

		double lowBorder = (firstQuartile - (1.5 * (thirdQuartile - firstQuartile)));
		double bottomOutlier;
		vector<double>::iterator it;
		for (it = propertyValuesVector.begin() ; it != propertyValuesVector.end() ; ++it) {
			if (*it > lowBorder) {
				bottomOutlier = *it;
				break;
			}
		}

		double highBorder =  (thirdQuartile + (1.5 * (thirdQuartile - firstQuartile)));
		double topOutlier;
		vector<double>::reverse_iterator itr;
		for (itr = propertyValuesVector.rbegin() ; itr != propertyValuesVector.rend() ; ++itr) {
			if (*itr < highBorder) {
				topOutlier = *itr;
				break;
			}
		}



		boxPlotValuesCoord[BOTTOM_OUTLIER] = getAxisCoordForValue(bottomOutlier);
		boxPlotValuesCoord[FIRST_QUARTILE] = getAxisCoordForValue(firstQuartile);
		boxPlotValuesCoord[MEDIAN] = getAxisCoordForValue(median);
		boxPlotValuesCoord[THIRD_QUARTILE] = getAxisCoordForValue(thirdQuartile);
		boxPlotValuesCoord[TOP_OUTLIER] = getAxisCoordForValue(topOutlier);

		boxPlotStringValues[BOTTOM_OUTLIER] = getStringFromNumber(bottomOutlier);
		boxPlotStringValues[FIRST_QUARTILE] = getStringFromNumber(firstQuartile);
		boxPlotStringValues[MEDIAN] = getStringFromNumber(median);
		boxPlotStringValues[THIRD_QUARTILE] = getStringFromNumber(thirdQuartile);
		boxPlotStringValues[TOP_OUTLIER] = getStringFromNumber(topOutlier);
	}
}



double QuantitativeParallelAxis::getAxisMinValue() {
	if (graphProxy->getGraph()->getRoot() == graphProxy->getGraph()) {
		if (getAxisDataTypeName() == "double") {
			return graphProxy->getPropertyMinValue<DoubleProperty, DoubleType>(getAxisName());
		} else {
			return (double) graphProxy->getPropertyMinValue<IntegerProperty, IntegerType>(getAxisName());
		}
	} else {
		Iterator<unsigned int> *dataIt = graphProxy->getDataIterator();
		unsigned int dataId = dataIt->next();
		double min;
		if (getAxisDataTypeName() == "double") {
			min = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(),dataId);
		} else {
			min = graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(),dataId);
		}
		while (dataIt->hasNext()) {
			dataId = dataIt->next();
			double propertyValue;
			if (getAxisDataTypeName() == "double") {
				propertyValue = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(),dataId);
			} else {
				propertyValue = graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(),dataId);
			}

			if (propertyValue < min) {
				min = propertyValue;
			}
		}
		delete dataIt;
		return min;
	}
}

double QuantitativeParallelAxis::getAxisMaxValue() {
	if (graphProxy->getGraph()->getRoot() == graphProxy->getGraph()) {
		if (getAxisDataTypeName() == "double") {
			return graphProxy->getPropertyMaxValue<DoubleProperty, DoubleType>(getAxisName());
		} else {
			return (double) graphProxy->getPropertyMaxValue<IntegerProperty, IntegerType>(getAxisName());
		}
	} else {
		Iterator<unsigned int> *dataIt = graphProxy->getDataIterator();
		unsigned int dataId = dataIt->next();
		double max;
		if (getAxisDataTypeName() == "double") {
			max = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(),dataId);
		} else {
			max = graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(),dataId);
		}
		while (dataIt->hasNext()) {
			dataId = dataIt->next();
			double propertyValue;
			if (getAxisDataTypeName() == "double") {
				propertyValue = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(),dataId);
			} else {
				propertyValue = graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(),dataId);
			}

			if (propertyValue > max) {
				max = propertyValue;
			}
		}
		delete dataIt;
		return max;
	}
}

void QuantitativeParallelAxis::translate(const Coord &c) {
	ParallelAxis::translate(c);
	boxPlotValuesCoord[BOTTOM_OUTLIER] += c;
	boxPlotValuesCoord[FIRST_QUARTILE] += c;
	boxPlotValuesCoord[MEDIAN] += c;
	boxPlotValuesCoord[THIRD_QUARTILE] += c;
	boxPlotValuesCoord[TOP_OUTLIER] += c;

}

Coord QuantitativeParallelAxis::getPointCoordOnAxisForData(const unsigned int dataIdx) {
	double value = 0;
	if (getAxisDataTypeName() == "double") {
		value = graphProxy->getPropertyValueForData<DoubleProperty, DoubleType>(getAxisName(), dataIdx);
	} else if (getAxisDataTypeName() == "int") {
		value = (double) graphProxy->getPropertyValueForData<IntegerProperty, IntegerType>(getAxisName(), dataIdx);
	}
	return glQuantitativeAxis->getAxisPointCoordForValue(value);
}

Coord QuantitativeParallelAxis::getAxisCoordForValue(double value) {
	return glQuantitativeAxis->getAxisPointCoordForValue(value);
}

void QuantitativeParallelAxis::redraw() {
	setAxisLabels();
	ParallelAxis::redraw();
	computeBoxPlotCoords();
}

void QuantitativeParallelAxis::showConfigDialog() {
	QuantitativeAxisConfigDialog dialog(this);
	dialog.exec();
	updateSlidersWithDataSubset(graphProxy->getHighlightedElts());
}

string QuantitativeParallelAxis::getAxisDataTypeName() const {
	return graphProxy->getProperty(getAxisName())->getTypename();
}


double QuantitativeParallelAxis::getValueForAxisCoord(const Coord &axisCoord) {
	return glQuantitativeAxis->getValueForAxisPoint(axisCoord);
}

std::string QuantitativeParallelAxis::getTopSliderTextValue() {
	if (getAxisDataTypeName() == "int") {
		return getStringFromNumber(getValueForAxisCoord(topSliderCoord));
	} else if (getAxisDataTypeName() == "double") {
		return getStringFromNumber(getValueForAxisCoord(topSliderCoord));
	} else {
		return "";
	}
}

std::string QuantitativeParallelAxis::getBottomSliderTextValue() {
	if (getAxisDataTypeName() == "int") {
		return getStringFromNumber(getValueForAxisCoord(bottomSliderCoord));
	} else if (getAxisDataTypeName() == "double") {
		return getStringFromNumber(getValueForAxisCoord(bottomSliderCoord));
	} else {
		return "";
	}
}

set<unsigned int> QuantitativeParallelAxis::getDataInRange(float yLowBound, float yHighBound) {
	set<unsigned int> dataSubset;
	Iterator<unsigned int> *dataIt = graphProxy->getDataIterator();
	while (dataIt->hasNext()) {
		unsigned int dataId = dataIt->next();
		Coord dataCoord = getPointCoordOnAxisForData(dataId);
		if (dataCoord.getY() <= yHighBound && dataCoord.getY() >= yLowBound) {
			dataSubset.insert(dataId);
		}
	}
	delete dataIt;
	return dataSubset;
}

set<unsigned int> QuantitativeParallelAxis::getDataInSlidersRange() {
	return getDataInRange(bottomSliderCoord.getY(), topSliderCoord.getY());
}

set<unsigned int> QuantitativeParallelAxis::getDataBetweenBoxPlotBounds() {
	if (boxPlotLowBound != NO_VALUE && boxPlotHighBound != NO_VALUE) {
		return getDataInRange(boxPlotValuesCoord[boxPlotLowBound].getY(), boxPlotValuesCoord[boxPlotHighBound].getY());
	} else {
		return set<unsigned int>();
	}
}

void QuantitativeParallelAxis::updateSlidersWithDataSubset(const set<unsigned int> &dataSubset) {
	set<unsigned int>::iterator it;
	Coord max = getBaseCoord();
	Coord min = getBaseCoord() + Coord(0, getAxisHeight());
	for (it = dataSubset.begin() ; it != dataSubset.end() ; ++it) {
		Coord dataCoord = getPointCoordOnAxisForData(*it);
		if (dataCoord.getY() < min.getY()) {
			min = dataCoord;
		}

		if (dataCoord.getY() > max.getY()) {
			max = dataCoord;
		}
	}
	bottomSliderCoord = min;
	topSliderCoord = max;
}

void QuantitativeParallelAxis::setAscendingOrder(const bool ascendingOrder) {

	Coord baseCoord = getBaseCoord();
	float axisHeight = getAxisHeight();

	if (ascendingOrder != hasAscendingOrder()) {

		float spaceBetweenSlider = topSliderCoord.getY() - bottomSliderCoord.getY();

		float axisCenterTopSliderDist = topSliderCoord.getY() - (baseCoord.getY() + axisHeight / 2);
		topSliderCoord.setY((baseCoord.getY() + axisHeight / 2) - axisCenterTopSliderDist + spaceBetweenSlider);

		float axisCenterBottomSliderDist = bottomSliderCoord.getY() - (baseCoord.getY() + axisHeight / 2);
		bottomSliderCoord.setY((baseCoord.getY() + axisHeight / 2) - axisCenterBottomSliderDist - spaceBetweenSlider);

	}

	glQuantitativeAxis->setAscendingOrder(ascendingOrder);
}

}
