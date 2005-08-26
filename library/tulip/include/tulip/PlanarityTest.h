//-*-c++-*-
/*
 Author: Sophie BARDET & Alexandre BURNETT
 Email : sophie.bardet@gmail.com, aburnett397@gmail.com
 Last modification : 19/06/2005
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef TULIP_PLANARITYTESTX_H
#define TULIP_PLANARITYTESTX_H

#include <tulip/tulipconf.h>
#include <ext/hash_map>
#include <list>
#include <tulip/ObservableGraph.h>
#include <tulip/Edge.h> 

class SuperGraph;

/** \addtogroup graph_test */ 
/*@{*/
/// class for testing the planarity of a graph
class TLP_SCOPE PlanarityTest : public GraphObserver {
public:
  static bool isPlanar(SuperGraph *graph);
  static bool planarEmbedding(SuperGraph *graph);
  static std::list<edge> getObstructionsEdges(SuperGraph *graph);
  
private:
  void addEdge(SuperGraph *,const edge);
  void delEdge(SuperGraph *,const edge);
  void reverseEdge(SuperGraph *,const edge);
  void addNode(SuperGraph *,const node);
  void delNode(SuperGraph *,const node);
  void destroy(SuperGraph *);
  bool compute(SuperGraph *graph);
  PlanarityTest(){}
  static PlanarityTest * instance;
  stdext::hash_map<unsigned int,bool> resultsBuffer;
};
/*@}*/
#endif
