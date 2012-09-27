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
///@cond DOXYGEN_HIDDEN

#ifndef Tulip_GLGRAPHLOWDETAILSRENDERER_H
#define Tulip_GLGRAPHLOWDETAILSRENDERER_H

#include <tulip/GlGraphRenderer.h>

#include <vector>

#include <tulip/Observable.h>

#include <tulip/GlGraphRenderingParameters.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/GlNode.h>
#include <tulip/GlEdge.h>
#include <tulip/GlQuadTreeLODCalculator.h>
#include <tulip/GlScene.h>

namespace tlp {

class Graph;

/** \brief Class to display graph with very simple and very fast renderer
 *
 * Very high performance renderer
 * This class display graph with :
 *  - Nodes : quads
 *  - Edges : lines
 * Warning : this renderer doesn't work for selection
 *
 * See GlGraphRenderer documentation for functions documentations
 */
class TLP_GL_SCOPE GlGraphLowDetailsRenderer : public GlGraphRenderer, public Observable {

public:

  GlGraphLowDetailsRenderer(const GlGraphInputData *inputData);

  ~GlGraphLowDetailsRenderer();

  virtual void draw(float lod,Camera* camera);

  void initSelectionRendering(RenderingEntitiesFlag ,std::map<unsigned int, SelectedEntity> &,unsigned int &) {
    assert (false);
  }

protected:

  void initEdgesArray();
  void initTexArray(unsigned int glyph, Vec2f tex[4]);
  void initNodesArray();

  void addObservers();
  void removeObservers();
  void updateObservers();
  void treatEvent(const Event &ev);

  GlScene *fakeScene;

  bool buildVBO;

  std::vector<Vec2f>   points;
  std::vector<Color>   colors;
  std::vector<GLuint>  indices;

  std::vector<Vec2f>   quad_points;
  std::vector<Color>   quad_colors;
  std::vector<GLuint>  quad_indices;

  Graph *observedGraph;
  LayoutProperty *observedLayoutProperty;
  ColorProperty *observedColorProperty;
  SizeProperty *observedSizeProperty;
  BooleanProperty *observedSelectionProperty;
};
}

#endif
///@endcond