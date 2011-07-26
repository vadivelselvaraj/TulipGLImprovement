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
#ifndef _DRAWINGTOOLS_H
#define _DRAWINGTOOLS_H

#include <vector>
#include <tulip/Coord.h>
#include <tulip/BoundingBox.h>
#include <tulip/Iterator.h>

namespace tlp {

class Graph;
class LayoutProperty;
class SizeProperty;
class DoubleProperty;
class BooleanProperty;

/**
  *
  * Computes the bounding box of a graph according to nodes positions, edges bends,
  * nodes z-rotations and sizes of elements.
  *
  */
TLP_SCOPE   BoundingBox computeBoundingBox(const Graph *graph,
                                           const LayoutProperty *layout,
                                           const SizeProperty *size,
                                           const DoubleProperty *rotation,
                                           const BooleanProperty *selection = 0);


/**
 * Compute the bounding box of graph elements according to node positions, edges bends,
 * nodes z-rotations and sizes of elements.
 *
 * Iterator itN and itE will be deleted after the computations (i.e. no need to delete them yourself).
 */
TLP_SCOPE BoundingBox computeBoundingBox(Iterator<node> *itN,
										 Iterator<edge> *itE,
									     const LayoutProperty *layout,
									     const SizeProperty *size,
									     const DoubleProperty *rotation,
									     const BooleanProperty *selection = 0);
/**
  *
  * Computes a bounding sphere (or a bounding circle if the graph has a 2D layout) of a graph according to nodes positions, edges bends,
  * nodes z-rotations and sizes of elements.
  *
  * Returns a pair of tlp::Coord whose first member is the center of the bounding sphere (circle for 2D layout)
  * and second member is the farthest point from the center (computed from graph elements positions).
  * To get the bounding radius, you have to compute the distance between the two members of the pair
  * (use the dist method from tlp::Coord).
  *
  */

TLP_SCOPE   std::pair<Coord, Coord> computeBoundingRadius (const Graph *graph,
                                                           const LayoutProperty *layout,
                                                           const SizeProperty *size,
                                                           const DoubleProperty *rotation,
                                                           const BooleanProperty *selection = 0);
/**
  *
  * Computes a convex hull of a graph according to nodes positions, edges bends,
  * nodes z-rotations, and sizes of elements. Only works with 2D layouts.
  *
  * Returns a vector of tlp::Coord containing the vertices of the graph convex hull correctly ordered.
  *
  */
TLP_SCOPE   std::vector<Coord> computeConvexHull (const Graph *graph,
                                                  const LayoutProperty *layout,
                                                  const SizeProperty *size,
                                                  const DoubleProperty *rotation,
                                                  const BooleanProperty *selection = 0);

}


#endif
