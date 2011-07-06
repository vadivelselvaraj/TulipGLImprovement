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
#ifndef Tulip_SPANNINGTREESELECTION_H
#define Tulip_SPANNINGTREESELECTION_H

#include <tulip/TulipPlugin.h>

/** \addtogroup selection */
/*@{*/

/**
 * This selection plugins enables to find a subgraph of G that is a forest (a set of trees).
 *
 *  \author David Auber, LaBRI University Bordeaux I France:
 *   auber@tulip-software.org
 */
class SpanningTreeSelection:public tlp::BooleanAlgorithm {
public:
  SpanningTreeSelection(const tlp::PropertyContext &);
  ~SpanningTreeSelection();
  bool run();
};
/*@}*/
#endif
