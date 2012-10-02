/*
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

#ifndef GRAPHOBSERVABLE_H
#define GRAPHOBSERVABLE_H
#include <list>
#include <map>
#include <algorithm>

#include <tulip/Node.h>
#include <tulip/Edge.h>
#include "tulip/Observable.h"

namespace tlp {

class Graph;

//=========================================================

/**
 * @ingroup Observation
 * \class GraphObserver
 * \brief That class receives a message (call back/handler function)
 * after each modification of a Graph.
 *
 * To receive a notification from an ObservableGraph, the GraphObserver must
 * first be connected to the ObservableGraph.
 * This is done by calling ObservableGraph::addGraphObserver.
 *
 * Depending on which messages you want to receive, you need to override different functions of GraphObserver.
 *
 * If manageObservables is set to true, the GraphObserver will automatically
 * unregister from an ObservableGraph when deleted. (ie, unregistering is done
 * automatically)
 *
 * The Observer pattern is described pp293-304 of the book 'Design Patterns' by Gamma, Helm, Johnson, and Vlissides.
 *
 * PrintObserver This is a small example of an observer that displays a message on the standard output each time a
 * a node is added to a graph.
 * \code
 * class PrintObs : public GraphObserver {
 *    void addNode(Graph *g, const node n) {
 *       qDebug() << "node " << n.id << " added in " << g << endl << flush;
 *    }
 * };
 *
 * int main() {
 *   Graph *g = tlp::newGraph();
 *   PrintObs obs;
 *   g->addListener(&obs);
 *   g->addNode(); //output node 0 added in 0x
 *   g->addNode(); //output node 0 added in 0x
 * }
 * \endcode
 */
class _DEPRECATED  TLP_SCOPE GraphObserver {

public:
  virtual ~GraphObserver() {}
  virtual void addNode(Graph *, const node ) {}
  virtual void addEdge(Graph *, const edge ) {}
  virtual void beforeSetEnds(Graph *, const edge) {}
  virtual void afterSetEnds(Graph *, const edge) {}
  virtual void delNode(Graph *,const node ) {}
  virtual void delEdge(Graph *,const edge ) {}
  virtual void reverseEdge(Graph *,const edge ) {}
  virtual void destroy(Graph *) {}
  virtual void addSubGraph(Graph *, Graph *) {}
  virtual void delSubGraph(Graph *, Graph *) {}
  virtual void addLocalProperty(Graph*, const std::string&) {}
  virtual void delLocalProperty(Graph*, const std::string&) {}
  virtual void beforeSetAttribute(Graph*, const std::string&) {}
  virtual void afterSetAttribute(Graph*, const std::string&) {}
  virtual void removeAttribute(Graph*, const std::string&) {}
protected:
  void treatEvent(const Event&);
};

}


#endif

///@endcond