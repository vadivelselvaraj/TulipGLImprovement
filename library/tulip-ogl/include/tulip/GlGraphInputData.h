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
#ifndef Tulip_GLGRAPHINPUTDATA_H
#define Tulip_GLGRAPHINPUTDATA_H

#include <tulip/tulipconf.h>

#include <tulip/MutableContainer.h>

#include <tulip/GlMetaNodeRenderer.h>
#include <tulip/GlGraphRenderingParameters.h>

namespace tlp {

class LayoutProperty;
class DoubleProperty;
class StringProperty;
class IntegerProperty;
class BooleanProperty;
class SizeProperty;
class ColorProperty;
class PropertyManager;
class Graph;
class Glyph;
class EdgeExtremityGlyph;
class GlVertexArrayManager;

/**
 * Class use to store inputData of the graph
 */
class TLP_GL_SCOPE GlGraphInputData {

public:

  /**
   * GlGraphInputData available properties
   */
  enum PropertyName {
    VIEW_COLOR=0, /**< color of nodes/edges */
    VIEW_LABELCOLOR, /**< color of lables */
    VIEW_SIZE, /**< size of nodes/edges */
    VIEW_LABELPOSITION, /**< position of labels */
    VIEW_SHAPE, /**< shape of nodes/edges */
    VIEW_ROTATION, /**< rotation apply on nodes */
    VIEW_SELECTED, /**< nodes/edges selected */
    VIEW_FONT, /**< font name of labels */
    VIEW_FONTSIZE, /**< font size of labels */
    VIEW_LABEL, /**< text of labels */
    VIEW_LAYOUT, /**< position of nodes */
    VIEW_TEXTURE, /**< texture of nodes/edges */
    VIEW_BORDERCOLOR, /**< border color of nodes/edges */
    VIEW_BORDERWIDTH, /**< border width of nodes/edges */
    VIEW_SRCANCHORSHAPE, /**< shape of source arrow edge extremity */
    VIEW_SRCANCHORSIZE, /**< size of source arrow edge extremity */
    VIEW_TGTANCHORSHAPE, /**< shape of target arrow edge extremity */
    VIEW_TGTANCHORSIZE, /**< size of target arrow edge extremity */
    VIEW_ANIMATIONFRAME /**< animation frame */
  };

  /**
   * Create the inputData with Graph : graph and GlGraphRenderingParameters : parameters
   */
  GlGraphInputData(Graph* graph, GlGraphRenderingParameters* parameters,GlMetaNodeRenderer *renderer = NULL);

  ~GlGraphInputData();

  /**
   * Return the graph of this inputData
   */
  Graph* getGraph() const {
    return graph;
  }

  /**
   * Set metaNode renderer
   * If deleteOldMetaNodeRenderer==true : this function delete old meta node renderer
   */
  void setMetaNodeRenderer(GlMetaNodeRenderer *renderer,bool deleteOldMetaNodeRenderer=true) {
    if(deleteOldMetaNodeRenderer)
      delete metaNodeRenderer;

    metaNodeRenderer = renderer;
  }

  /**
   * Set if the meta node renderer must be deleted at destructor
   */
  void setDeleteMetaNodeRendererAtDestructor(bool deleteAtDestructor) {
    deleteMetaNodeRendererAtDestructor=deleteAtDestructor;
  }

  /**
   * Return metaNode renderer
   */
  GlMetaNodeRenderer *getMetaNodeRenderer() const {
    return metaNodeRenderer;
  }

  /**
   * Return glEdgeDisplayManager
   */
  GlVertexArrayManager *getGlVertexArrayManager() const {
    return glVertexArrayManager;
  }

  /**
   * Set glEdgeDisplayManager
   */
  void setGlVertexArrayManager(GlVertexArrayManager * manager) {
    glVertexArrayManager=manager;
  }

  /**
   * Set if GlVertexArrayManager must be deleted in destructor
   */
  void deleteGlVertexArrayManagerInDestructor(bool del) {
    deleteGlVertexArrayManager=del;
  }

  /**
   * Function to get the pointer for propertyName
   * See PropertyName enum for more details on available properties
   */
  template<typename T>
  T* getProperty(PropertyName propertyName) const {
    T* property=dynamic_cast<T*>(propertiesMap.find(propertyName)->second);
    assert(property);
    return property;
  }

  /**
   * Function to set the pointer for propertyName
   * See PropertyName enum for more details on available properties
   */
  void setProperty(PropertyName propertyName,PropertyInterface *property) {
    _properties.erase(propertiesMap[propertyName]);
    propertiesMap[propertyName]=property;
    _properties.insert(property);
  }

  /**
   * Return a pointer on the property used to elementColor
   */
  ColorProperty * getElementColor() const {
    return getProperty<ColorProperty>(VIEW_COLOR);
  }
  /**
   * Set the pointer on the property used to elementColor
   */
  void setElementColor(ColorProperty *property) {
    setProperty(VIEW_COLOR,property);
  }
  /**
   * Return a pointer on the property used to elementLabelColor
   */
  ColorProperty *getElementLabelColor() const {
    return getProperty<ColorProperty>(VIEW_LABELCOLOR);
  }
  /**
   * Set the pointer on the property used to elementLabelColor
   */
  void setElementLabelColor(ColorProperty *property) {
    setProperty(VIEW_LABELCOLOR,property);
  }
  /**
   * Return a pointer on the property used to elementSize
   */
  SizeProperty *getElementSize() const {
    return getProperty<SizeProperty>(VIEW_SIZE);
  }
  /**
   * Set the pointer on the property used to elementSize
   */
  void setElementSize(SizeProperty *property) {
    setProperty(VIEW_SIZE,property);
  }
  /**
   * Return a pointer on the property used to elementLabelPosition
   */
  IntegerProperty *getElementLabelPosition() const {
    return getProperty<IntegerProperty>(VIEW_LABELPOSITION);
  }
  /**
   * Set the pointer on the property used to elementLabelPosition
   */
  void setElementLabelPosition(IntegerProperty *property) {
    setProperty(VIEW_LABELPOSITION,property);
  }
  /**
   * Return a pointer on the property used to elementShape
   */
  IntegerProperty *getElementShape() const {
    return getProperty<IntegerProperty>(VIEW_SHAPE);
  }
  /**
   * Set the pointer on the property used to elementShape
   */
  void setElementShape(IntegerProperty *property) {
    setProperty(VIEW_SHAPE,property);
  }
  /**
   * Return a pointer on the property used to elementRotation
   */
  DoubleProperty *getElementRotation() const {
    return getProperty<DoubleProperty>(VIEW_ROTATION);
  }
  /**
   * Set the pointer on the property used to elementRotation
   */
  void setElementRotation(DoubleProperty *property) {
    setProperty(VIEW_ROTATION,property);
  }
  /**
   * Return a pointer on the property used to elementSelected
   */
  BooleanProperty *getElementSelected() const {
    return getProperty<BooleanProperty>(VIEW_SELECTED);
  }
  /**
   * Set the pointer on the property used to elementSelected
   */
  void setElementSelected(BooleanProperty *property) {
    setProperty(VIEW_SELECTED,property);
  }
  /**
   * Return a pointer on the property used to elementFont
   */
  StringProperty *getElementFont() const {
    return getProperty<StringProperty>(VIEW_FONT);
  }
  /**
   * Set the pointer on the property used to elementFont
   */
  void setElementFont(StringProperty *property) {
    setProperty(VIEW_FONT,property);
  }
  /**
   * Return a pointer on the property used to elementFontSize
   */
  IntegerProperty  *getElementFontSize() const {
    return getProperty<IntegerProperty>(VIEW_FONTSIZE);
  }
  /**
   * Set the pointer on the property used to elementFontSize
   */
  void setElementFontSize(IntegerProperty *property) {
    setProperty(VIEW_FONTSIZE,property);
  }
  /**
   * Return a pointer on the property used to elementLabel
   */
  StringProperty *getElementLabel() const {
    return getProperty<StringProperty>(VIEW_LABEL);
  }
  /**
   * Set the pointer on the property used to elementLabel
   */
  void setElementLabel(StringProperty *property) {
    setProperty(VIEW_LABEL,property);
  }
  /**
   * Return a pointer on the property used to elementLayout
   */
  LayoutProperty *getElementLayout() const {
    return getProperty<LayoutProperty>(VIEW_LAYOUT);
  }
  /**
   * Set the pointer on the property used to elementLayout
   */
  void setElementLayout(LayoutProperty *property) {
    setProperty(VIEW_LAYOUT,property);
  }
  /**
   * Return a pointer on the property used to elementTexture
   */
  StringProperty *getElementTexture() const {
    return getProperty<StringProperty>(VIEW_TEXTURE);
  }
  /**
   * Set the pointer on the property used to elementTexture
   */
  void setElementTexture(StringProperty *property) {
    setProperty(VIEW_TEXTURE,property);
  }
  /**
   * Return a pointer on the property used to elementBorderColor
   */
  ColorProperty *getElementBorderColor() const {
    return getProperty<ColorProperty>(VIEW_BORDERCOLOR);
  }
  /**
   * Set the pointer on the property used to elementBorderColor
   */
  void setElementBorderColor(ColorProperty *property) {
    setProperty(VIEW_BORDERCOLOR,property);
  }
  /**
   * Return a pointer on the property used to elementBorderWidth
   */
  DoubleProperty *getElementBorderWidth() const {
    return getProperty<DoubleProperty>(VIEW_BORDERWIDTH);
  }
  /**
   * Set the pointer on the property used to elementBorderWidth
   */
  void setElementBorderWidth(DoubleProperty *property) {
    setProperty(VIEW_BORDERWIDTH,property);
  }
  /**
   * Return a pointer on the property used to elementSrcAnchorShape
   */
  IntegerProperty *getElementSrcAnchorShape() const {
    return getProperty<IntegerProperty>(VIEW_SRCANCHORSHAPE);
  }
  /**
   * Set the pointer on the property used to elementSrcAnchorShape
   */
  void setElementSrcAnchorShape(IntegerProperty *property) {
    setProperty(VIEW_SRCANCHORSHAPE,property);
  }
  /**
   * Return a pointer on the property used to elementSrcAnchorSize
   */
  SizeProperty *getElementSrcAnchorSize() const {
    return getProperty<SizeProperty>(VIEW_SRCANCHORSIZE);
  }
  /**
   * Set the pointer on the property used to elementSrcAnchorSize
   */
  void setElementSrcAnchorSize(SizeProperty *property) {
    setProperty(VIEW_SRCANCHORSIZE,property);
  }
  /**
   * Return a pointer on the property used to elementTgtAnchorShape
   */
  IntegerProperty *getElementTgtAnchorShape() const {
    return getProperty<IntegerProperty>(VIEW_TGTANCHORSHAPE);
  }
  /**
   * Set the pointer on the property used to elementTgtAnchorShape
   */
  void setElementTgtAnchorShape(IntegerProperty *property) {
    setProperty(VIEW_TGTANCHORSHAPE,property);
  }
  /**
   * Return a pointer on the property used to elementTgtAnchorSize
   */
  SizeProperty *getElementTgtAnchorSize() const {
    return getProperty<SizeProperty>(VIEW_TGTANCHORSIZE);
  }
  /**
   * Set the property name for elementSourceAnchorSize
   */
  void setElementTgtAnchorSize(SizeProperty *property) {
    setProperty(VIEW_TGTANCHORSIZE,property);
  }
  /**
   * Return a pointer on the property used to elementAnimationFrame
   */
  IntegerProperty *getElementAnimationFrame() const {
    return getProperty<IntegerProperty>(VIEW_ANIMATIONFRAME);
  }
  /**
   * Set the pointer on the property used to elementAnimationFrame
   */
  void setElementAnimationFrame(IntegerProperty *property) {
    setProperty(VIEW_ANIMATIONFRAME,property);
  }

  std::set<tlp::PropertyInterface*> properties() const {
    return _properties;
  }

public :

  Graph* graph;

  GlGraphRenderingParameters* parameters;

  MutableContainer<Glyph *> glyphs;
  MutableContainer<EdgeExtremityGlyph *> extremityGlyphs;

protected:

  std::set<PropertyInterface*> _properties;

  bool deleteGlVertexArrayManager;

  std::map<PropertyName,PropertyInterface*> propertiesMap;

  bool deleteMetaNodeRendererAtDestructor;
  GlMetaNodeRenderer *metaNodeRenderer;
  GlVertexArrayManager *glVertexArrayManager;


};
}

#endif
