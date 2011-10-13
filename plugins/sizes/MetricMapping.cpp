/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include <cmath>
#include <tulip/TulipPluginHeaders.h>

using namespace std;
using namespace tlp;

namespace {
const char * paramHelp[] = {
  // property
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "DoubleProperty" ) \
  HTML_HELP_BODY() \
  "Metric to map to size." \
  HTML_HELP_CLOSE(),
  // input
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "SizeProperty" ) \
  HTML_HELP_BODY() \
  "If not all dimensions (width, height, depth) are checked below, the dimensions not computed are copied from this property." \
  HTML_HELP_CLOSE(),
  // width, height, depth
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "boolean" ) \
  HTML_HELP_DEF( "values", "true/false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "Each checked dimension is adjusted to represent property, each unchecked dimension is copied from input." \
  HTML_HELP_CLOSE(),
  // min
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "double" ) \
  HTML_HELP_BODY() \
  "Minimum value of the range of computed sizes." \
  HTML_HELP_CLOSE(),
  // max
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "double" ) \
  HTML_HELP_BODY() \
  "Maximum value of the range of computed sizes." \
  HTML_HELP_CLOSE(),
  // Mapping type
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "Boolean" ) \
  HTML_HELP_DEF( "values", "true / false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "Type of mapping." \
  "<ul><li>true: linear mapping (min value of property is mapped to min size, max to max size, and a linear interpolation is used inbetween.)</li>" \
  "<li>false: uniform quantification (the values of property are sorted, and the same size increment is used between consecutive values).</li></ul>"                          \
  HTML_HELP_CLOSE(),
  // Mapping type
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "Boolean" ) \
  HTML_HELP_DEF( "values", "true / false" ) \
  HTML_HELP_DEF( "default", "true" ) \
  HTML_HELP_BODY() \
  "If true, compute the size of nodes, else compute the size of edges." \
  HTML_HELP_CLOSE(),
};
}

// error msg for invalid range value
static const char* rangeSizeErrorMsg = "max size must be greater than min size";
static const char* rangeMetricErrorMsg = "All values are the same";

/** \addtogroup size */
/*@{*/
/// Metric Mapping - Compute size of elements according to a metric.
/** This plugin enables to set the size of the graph's elements
 *  according to a metric.
 *
 *  \author David Auber University Bordeaux I France: Email:auber@tulip-software.org
 */
class MetricSizeMapping:public SizeAlgorithm {
public:
  MetricSizeMapping(const PropertyContext &context):SizeAlgorithm(context), entryMetric(NULL), entrySize(NULL), xaxis(true), yaxis(true), zaxis(true),
    min(1), max(10), range(false), shift(DBL_MAX), nodeoredge(true) {
    addParameter<DoubleProperty>("property", paramHelp[0]);
    addParameter<SizeProperty>("input", paramHelp[1]);
    addParameter<bool>("width", paramHelp[2],"true");
    addParameter<bool>("height", paramHelp[2],"true");
    addParameter<bool>("depth", paramHelp[2],"true");
    addParameter<double>("min size",paramHelp[3],"1");
    addParameter<double>("max size",paramHelp[4],"10");
    addParameter<bool>("type", paramHelp[5],"true");
    addParameter<bool>("node/edge", paramHelp[6],"true");
  }

  ~MetricSizeMapping() {}

  void computeNodeSize() {
    Iterator<node> *itN=graph->getNodes();

    while(itN->hasNext()) {
      node itn=itN->next();
      double sizos=min+(entryMetric->getNodeValue(itn)-shift)*(max-min)/range;
      Size res=entrySize->getNodeValue(itn);

      if (xaxis) res[0]=sizos;

      if (yaxis) res[1]=sizos;

      if (zaxis) res[2]=sizos;

      result->setNodeValue(itn, res);
    }

    delete itN;
  }

  void computeEdgeSize() {
    Iterator<edge> *itE=graph->getEdges();

    while(itE->hasNext()) {
      edge ite=itE->next();
      double sizos = min+(entryMetric->getEdgeValue(ite)-shift)*(max-min)/range;
      Size res  = entrySize->getEdgeValue(ite);
      res[0] = sizos;
      res[1] = sizos;
      result->setEdgeValue(ite, res);
    }

    delete itE;
  }

  bool check(std::string &errorMsg) {
    xaxis=yaxis=zaxis=true;
    min=1;
    max=10;
    nodeoredge = true;
    entryMetric=graph->getProperty<DoubleProperty>("viewMetric");
    entrySize=graph->getProperty<SizeProperty>("viewSize");
    mappingType = true;

    if ( dataSet!=0 ) {
      dataSet->get("property",entryMetric);
      dataSet->get("input",entrySize);
      dataSet->get("width",xaxis);
      dataSet->get("height",yaxis);
      dataSet->get("depth",zaxis);
      dataSet->get("min size",min);
      dataSet->get("max size",max);
      dataSet->get("type",mappingType);
      dataSet->get("node/edge",nodeoredge);
    }

    if (min >= max) {
      /*cerr << rangeErrorMsg << endl;
        pluginProgress->setError(rangeSizeErrorMsg); */
      errorMsg = std::string(rangeSizeErrorMsg);
      return false;
    }

    if (nodeoredge)
      range = entryMetric->getNodeMax(graph) - entryMetric->getNodeMin(graph);
    else
      range = entryMetric->getEdgeMax(graph) - entryMetric->getEdgeMin(graph);

    if (!range) {
      errorMsg = std::string(rangeMetricErrorMsg);
      return false;
    }

    return true;
  }

  bool run() {
    DoubleProperty *tmp = 0;

    if (!mappingType) {
      tmp = new DoubleProperty(graph);
      *tmp = *entryMetric;
      tmp->uniformQuantification(300);
      entryMetric = tmp;
    }

    if(nodeoredge) {
      shift = entryMetric->getNodeMin(graph);
      computeNodeSize();
      edge e;
      forEach(e, graph->getEdges())
      result->setEdgeValue(e, entrySize->getEdgeValue(e));
    }
    else {
      shift = entryMetric->getEdgeMin(graph);
      computeEdgeSize();
      node n;
      forEach(n, graph->getNodes())
      result->setNodeValue(n, entrySize->getNodeValue(n));
    }

    if (!mappingType) delete tmp;

    return true;
  }

private:
  DoubleProperty *entryMetric;
  SizeProperty *entrySize;
  bool xaxis,yaxis,zaxis,mappingType;
  double min,max;
  double range;
  double shift;
  bool nodeoredge;
};
/*@}*/
SIZEPLUGIN(MetricSizeMapping,"Metric Mapping","Auber","08/08/2003","","1.0")
