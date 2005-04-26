//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 01/09/2003
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#include <tulip/Iterator.h>
#include <tulip/SuperGraph.h>
#include <tulip/tulipconf.h>
#include <list>

#ifndef TULIP_NODEMAPITERATOR_H
#define TULIP_NODEMAPITERATOR_H

/** \addtogroup graphs */ 
/*@{*/


namespace tlp {
  TLP_SCOPE edge nextFaceEdge(SuperGraph* g, edge e, node n);
}
/**
 *
 */
struct TLP_SCOPE NodeMapIterator : public Iterator<node> {
  ///
  NodeMapIterator(SuperGraph *sg, node source, node target);
  ~NodeMapIterator();
  ///Return the next element
  node next();
  ///Return true if it exist a next element
  bool hasNext();
  
private :
  std::list<node> cloneIt;
  std::list<node>::iterator itStl;
};

/**
 *
 */
struct TLP_SCOPE EdgeMapIterator : public Iterator<edge> {
  ///
  EdgeMapIterator(SuperGraph *sg, edge source, node target);
  ///
  ~EdgeMapIterator();
  ///Return the next element
  edge next();
  ///Return true if it exist a next element
  bool hasNext();
  
private :
  std::vector<edge> adj;
  edge start;
  int treat;
  unsigned int pos;
  bool finished;
};
/*@}*/
#endif
