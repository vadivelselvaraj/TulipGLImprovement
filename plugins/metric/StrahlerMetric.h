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
#ifndef _STRAHLERGRAPH2METRIC_H
#define _STRAHLERGRAPH2METRIC_H

#include <tulip/tuliphash.h>
#include <tulip/TulipPlugin.h>
struct Strahler {
  Strahler(int stra=1,int sta=0,int used=0):strahler(stra),stacks(sta),usedStack(used) {}
  int strahler;
  int stacks;
  int usedStack;
};

/** \addtogroup metric */
/*@{*/
/** This plugin is an implementation of the Strahler Parameter.
 *  algorithm first published as:
 *
 *  A.N. Strahler, \n
 *  "Hypsomic analysis of erosional topography",\n
 *  "Bulletin Geological Society of America 63,pages 1117-1142.",\n
 *   1952.\n
 *
 *  Extended to graphs in :
 *
 *  D. Auber, \n
 *  Using Strahler numbers for real time visual exploration of huge graphs, \n
 *  ICCVG, International Conference on Computer Vision and Graphics, \n
 *  pages 56-69, \n
 *  2002, September.
 *
 *  \note This algorithm works on general graphs, if no node is selected,
 *  the algorithm automatically choose the nodes with higher outdegree as
 *  starting nodes.
 *
 *  The algorithm use the (bool)"viewSelection" property to determine
 *  the starting nodes.
 *
 */
class StrahlerMetric:public tlp::DoubleAlgorithm {
public:
  StrahlerMetric(const tlp::PropertyContext &);
  bool run();
  bool check(std::string &);

private:
  Strahler topSortStrahler(tlp::node n,int &curPref,
                           TLP_HASH_MAP<tlp::node,int> &tofree,
                           TLP_HASH_MAP<tlp::node,int> &prefix,
                           TLP_HASH_MAP<tlp::node,bool> &visited,
                           TLP_HASH_MAP<tlp::node,bool> &finished,
                           TLP_HASH_MAP<tlp::node,Strahler> &cachedValues);
  bool allNodes;
};
/*@}*/
#endif











