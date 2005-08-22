/*
 * Authors: J�r�my Compostella, Jean Darracq, Benjamin Muller,
 *          Fabrice Rochambeau, Fabiani Simplice, Jyl Cristoff Zobeide
 * 
 * Email : jcompost@etu.u-bordeaux1.fr, jdarracq@etu.u-bordeaux1.fr,
 *         bmuller@etu.u-bordeaux1.fr, frochamb@etu.u-bordeaux1.fr,
 *         fsimplic@etu.u-bordeaux1.fr, jczobeid@etu.u-bordeaux1.fr.
 *
 *$Id $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation; either version 2 of the License, or     
 * (at your option) any later version.
 */

#ifndef TULIP_SIMPLETEST_H
#define TULIP_SIMPLETEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if (__GNUC__ < 3)
#include <hash_set>
#else
#include <ext/hash_set>
#endif
#include "tulip/ObservableGraph.h"

class SuperGraph;

/** \addtogroup graph_test */ 
/*@{*/
/// class for testing if the graph is simple
class TLP_SCOPE SimpleTest : public GraphObserver {
 public: 
  static bool isSimple(SuperGraph *graph);
  //static list<edge> simpleEdgeObstruction(SuperGraph *graph);
  
 private:
  SimpleTest();
  bool compute(SuperGraph *graph);
  bool setResult(SuperGraph *graph, bool result);
  void addEdge(SuperGraph *, const edge);
  void delEdge(SuperGraph *, const edge);
  void destroy(SuperGraph *);
  void deleteResult(SuperGraph *graph);
  static SimpleTest *instance;
  stdext::hash_map<unsigned int, bool> resultsBuffer;
};
/*@}*/
#endif
