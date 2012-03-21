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

#include "ReachableSubGraphSelection.h"

using namespace tlp;

PLUGIN(ReachableSubGraphSelection)

namespace {
const char * paramHelp[] = {
  // direction
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "int" ) \
  HTML_HELP_DEF( "values", "{0,1,2}" ) \
  HTML_HELP_DEF( "default", "0" ) \
  HTML_HELP_BODY() \
  "Computation direction. The following values are allowable:" \
  "<ul><li>0: follow ouput edges (directed);</li>" \
  "<li>1: follow input edges (reverse-directed);</li>" \
  "<li>2: all edges (the graph is considered undirected).</li></ul>" \
  HTML_HELP_CLOSE(),

  // startingNodes
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "Selection" ) \
  HTML_HELP_DEF( "default", "\"viewSelection\"" ) \
  HTML_HELP_BODY() \
  "Starting set of nodes." \
  HTML_HELP_CLOSE(),

  // maxdepth
  HTML_HELP_OPEN() \
  HTML_HELP_DEF( "type", "int" ) \
  HTML_HELP_DEF( "values", "[0,1000000]" ) \
  HTML_HELP_DEF( "default", "10" ) \
  HTML_HELP_BODY() \
  "Maximal distance: nodes farther than this are not added to the selection." \
  HTML_HELP_CLOSE(),
};
}


ReachableSubGraphSelection::ReachableSubGraphSelection(const tlp::PluginContext* context):BooleanAlgorithm(context) {
  addInParameter<int> ("direction",paramHelp[0],"0");
  addInParameter<BooleanProperty> ("startingnodes",paramHelp[1],"viewSelection");
  addInParameter<int> ("distance",paramHelp[2],"5");
}

ReachableSubGraphSelection::~ReachableSubGraphSelection() {}

///===========================================================
bool ReachableSubGraphSelection::run() {
  unsigned int maxDistance = 5;
  unsigned int direction = 0;
  BooleanProperty * startNodes=graph->getProperty<BooleanProperty>("viewSelection");

  if ( dataSet!=0) {
    dataSet->get("distance", maxDistance);
    dataSet->get("direction", direction);
    dataSet->get("startingnodes", startNodes);
  }

  result->setAllEdgeValue(false);
  result->setAllNodeValue(false);

  if (startNodes) {
    Iterator<node>* itN = startNodes->getNodesEqualTo(true);
    std::set<node> reachables;

    // iterate on startNodes add them and their reachables
    while (itN->hasNext()) {
      node current = itN->next();
      reachables.insert(current);
      reachableNodes(graph, current, reachables, maxDistance,
                     (EDGE_TYPE) direction);
    }

    delete itN;

    std::set<node>::const_iterator itr = reachables.begin();
    std::set<node>::const_iterator ite = reachables.end();

    // select nodes
    while (itr != ite) {
      result->setNodeValue((*itr), true);
      ++itr;
    }

    // select corresponding edges
    Iterator<edge> *itE = graph->getEdges();

    while(itE->hasNext()) {
      edge e = itE->next();
      const std::pair<node, node>& ends = graph->ends(e);

      if (result->getNodeValue(ends.first) &&
          result->getNodeValue(ends.second))
        result->setEdgeValue(e, true);
    }

    delete itE;
  }

  return true;
}




