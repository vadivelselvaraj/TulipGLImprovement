//-*-c++-*-
/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 01/09/2003
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef TULIP_TREETEST_H
#define TULIP_TREETEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if (__GNUC__ < 3)
#include <hash_set>
#else
#include <ext/hash_set>
#endif
#include "tulip/ObservableGraph.h"
#include "tulip/MutableContainer.h"
#include "tulip/PluginProgress.h"

namespace tlp {

class Graph;

/** \addtogroup graph_test */ 
/*@{*/
/// Class for testing if the graph is a tree
class TLP_SCOPE TreeTest : private GraphObserver {
public:
  // check if the graph is a rooted tree
  static bool isTree(Graph *graph);
  // check if the graph is a free tree
  static bool isFreeTree(Graph *graph);
  // turns a free tree into a rooted tree
  static void makeRootedTree(Graph *freeTree, node root);
  // synonymous of the makeRootedTree
  static void makeDirectedTree(Graph *freeTree, node root) {
    makeRootedTree(freeTree, root);
  };

  /**
   * Compute a rooted tree from the graph.
   * The algorithm is the following
   * - if the graph is a rooted tree, return the graph
   * - if the graph is a free tree, return a rooted copy
   * - if the graph is connected, make a copy
   *   return a rooted spanning tree of that copy
   * - if the graph is not connected, make a copy,
   *   compute a tree for each of its connected components,
   *   add a simple source and return the copy.
   */  
  static Graph *computeTree(Graph* graph, Graph* rootGraph = 0, bool isConnected=false,
			    PluginProgress *pluginProgress = 0);
  
  /**
   * Clean the graph from a tree previously computed
   * with the computeRootedTree function
   */
  static void cleanComputedTree(Graph *graph, Graph *tree);

private:
  bool compute(Graph *);
  void addEdge(Graph *,const edge);
  void delEdge(Graph *,const edge);
  void reverseEdge(Graph *,const edge);
  void addNode(Graph *,const node);
  void delNode(Graph *,const node);
  void destroy(Graph *);
  TreeTest();
  static TreeTest * instance;
  bool isFreeTree (Graph *graph, node curRoot, node cameFrom,
		   MutableContainer<bool> &visited);
  void makeRootedTree (Graph *graph, node curRoot, node cameFrom);
  stdext::hash_map<unsigned long,bool> resultsBuffer;
};
/*@}*/

}
#endif
