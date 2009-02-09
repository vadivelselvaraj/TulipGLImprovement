#include "tulip/GlGraphInputData.h"

#include <tulip/Graph.h>

#include "tulip/GlyphManager.h"
#include "tulip/GlGraphRenderingParameters.h"

namespace tlp {

  GlGraphInputData::GlGraphInputData(Graph* graph,GlGraphRenderingParameters* parameters):
    elementColorPropName("viewColor"),elementLabelColorPropName("viewLabelColor"),elementSizePropName("viewSize"),elementLabelPositionPropName("viewLabelPosition"),
    elementShapePropName("viewShape"),elementRotationPropName("viewRotation"),elementSelectedPropName("viewSelection"),elementLabelPropName("viewLabel"),
    elementTexturePropName("viewTexture"),elementBorderColorPropName("viewBorderColor"),elementBorderWidthPropName("viewBorderWidth"),
    elementLayoutPropName(""),
    graph(graph),parameters(parameters) {
    reloadAllProperties();

    GlyphManager::getInst().initGlyphList(&this->graph,this,glyphs);
  }

  GlGraphInputData::~GlGraphInputData() {
    GlyphManager::getInst().clearGlyphList(&this->graph,this,glyphs);
  }

  void GlGraphInputData::reloadLayoutProperty() {
    if(elementLayoutPropName==""){
      if (!graph->getAttribute("viewLayout", elementLayout))
        elementLayout = graph->getProperty<LayoutProperty>("viewLayout");
    }else{
      elementLayout = graph->getProperty<LayoutProperty>(elementLayoutPropName);
    }
  }

  void GlGraphInputData::reloadAllProperties() {
    reloadLayoutProperty();

    elementRotation = graph->getProperty<DoubleProperty>(elementRotationPropName);
    elementSelected = graph->getProperty<BooleanProperty>(elementSelectedPropName);
    elementLabel = graph->getProperty<StringProperty>(elementLabelPropName);
    elementLabelColor = graph->getProperty<ColorProperty>(elementLabelColorPropName);
    elementLabelPosition = graph->getProperty<IntegerProperty>(elementLabelPositionPropName);
    elementColor = graph->getProperty<ColorProperty>(elementColorPropName);
    elementShape = graph->getProperty<IntegerProperty>(elementShapePropName);
    elementSize = graph->getProperty<SizeProperty>(elementSizePropName);
    elementTexture = graph->getProperty<StringProperty>(elementTexturePropName);
    elementBorderColor = graph->getProperty<ColorProperty>(elementBorderColorPropName);
    elementBorderWidth = graph->getProperty<DoubleProperty>(elementBorderWidthPropName);
  }

}
