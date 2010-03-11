//-*-c++-*-
/*
 Author: David Auber
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by  
 the Free Software Foundation; either version 3 of the License, or     
 (at your option) any later version.
*/
#ifndef DOXYGEN_NOTFOR_DEVEL

#ifndef _TULIPIDMANAGER_H
#define _TULIPIDMANAGER_H
#include <set>
#include <iostream>
#include "tulip/Iterator.h"

namespace tlp {

/// class for the management of the identifiers : node, edge
class TLP_SCOPE IdManager {

public:
  IdManager();
  /**
   * Returns true if the id is not used else false.
   */
  bool is_free(unsigned int) const;
  /**
   * Free the id given in parameter.
   */
  void free(const unsigned int);
  /**
   * Returns a new id, the returned id is choosen to minimize
   * the memory space of the free list, and the fragmetation
   * of the ids.
   */
  unsigned int get();
  /**
   * assuming the given id is free.
   * remove it from free ids
   * (used to ensure the same id when loading a graph with subgraphs)
   */
  void getFreeId(unsigned int id);
  /**
   * Returns an iterator on all the used ids. Warning, if
   * the idManager is modified (free, get) this iterator 
   * will be invalidate.
   */
  Iterator<unsigned int>* getIds() const;
  /**
   * indicates if space of ids is fragmented
   *
   */
  bool hasFreeIds() const;

  friend std::ostream& operator<<(std::ostream &,const IdManager &);
  friend class IdManagerIterator;
  friend class IdManagerTest;

  private:
  std::set<unsigned int> freeIds;  
  unsigned int nextId;
  unsigned int firstId;
};
std::ostream& operator<<(std::ostream &,const IdManager &);

}

#endif
#endif //DOXYGEN_NOTFOR_DEVEL
