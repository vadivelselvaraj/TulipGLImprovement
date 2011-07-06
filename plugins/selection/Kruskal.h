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
#ifndef _KRUSKAL_H
#define _KRUSKAL_H

#include <tulip/TulipPlugin.h>

/** \addtogroup selection */
/*@{*/

/// This selection plugins implements the so called Kruskal algorithm. This algorithm enables to find a minimum spanning tree in a connected graph.
/**
 * This selection plugins enables to find all nodes and
 * edges at a fixed distance of a set of nodes.
 *
 * This only works on undirected graphs, (ie. the orientation of edges is omitted).
 *
 * It takes one parameter :
 * - DoubleProperty edge weight, this parameter defines the weight of each edge in the graph.
 *
 *  \author Anthony Don, LaBRI University Bordeaux I France:
 */
class Kruskal:public tlp::BooleanAlgorithm {

public:
  Kruskal(const tlp::PropertyContext& context);
  ~Kruskal();
  bool run();
  bool check(std::string &);
};
/*@}*/
#endif
