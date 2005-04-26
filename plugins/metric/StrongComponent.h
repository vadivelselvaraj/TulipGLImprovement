//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef _StrongComponent_H
#define _StrongComponent_H
#include <stack>
#include <tulip/TulipPlugin.h>
struct NodeInfo{
  NodeInfo(int stra=0,int sta=0):prefixOrder(stra),minAttach(sta){}
  int prefixOrder;
  int minAttach;
};

/// StrongComponent.h - An implementation of the Stongly connected component algorithm.
/** This plugin is an implementation of the Stongly connected algorithm.
 *
 *  HISTORY:
 *
 *  01/12/1999 Verson 0.0.1: Initial release
 *
 *  NOTES:
 *
 *  This algorithm assigns to each node a value defined as following : If two nodes are in the same
 *  strongly connected component they have the same value else they have a 
 *  different value.
 *
 *  AUTHOR:
 *
 *  David Auber University Bordeaux I France: Email:auber@tulip-software.com
 *
 *  LICENCE:
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 2 of the License, or     
 *  (at your option) any later version.
 *
 */
class StrongComponent:public Metric { 
public:
  StrongComponent(const PropertyContext &);
  ~StrongComponent();
  bool run();
  bool check(std::string &);
  void reset();
  
private:
  int  attachNumerotation(node, stdext::hash_map<node,bool> &, stdext::hash_map<node,bool> &,
			  stdext::hash_map<node,int> &, int &, std::stack<node> &, int &);
};

#endif











