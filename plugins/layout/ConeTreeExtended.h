//-*-c++-*-
#ifndef _TREE3DLAYOUT_H
#define _TREE3DLAYOUT_H

#include <tulip/TulipPlugin.h>
/** \addtogroup layout */
/*@{*/
/// ConeTreeExtended.h - An implementation of the cone tree layout.
/** This plugin is an extension of the Cone tree layout
 *  algorithm first published as:
 *
 *  A. FJ. Carriere and R. Kazman, \n
 *  "Interacting with Huge Hierarchies: Beyond Cone Trees", \n
 *  In Proceedings of InfoViz'95, \n
 *  IEEE Symposium on Information Visualization \n
 *  pages 74-78, 1995,
 *
 *  Information about the extension can be found in :
 *
 *  D.Auber, PhD Thesis, "Outils de visualisation de larges structures de données",
 *  University Bordeaux, december 2002.
 *
 *  <b> HISTORY</b>
 *
 *  - 01/04/99 Verson 0.0.1: Initial release
 *
 *  \note This algorithm only work on trees.
 *  Let n be the number of nodes, the algorithm complexity is in O(n).
 *
 *  \author David Auber University Bordeaux I France: Email:auber@tulip-software.org
 */
class ConeTreeExtended:public Layout { 
public:
  ConeTreeExtended(const PropertyContext &);
  ~ConeTreeExtended();
  bool run();
  bool check(std::string &);
  void reset();
private:
  double treePlace3D(node n, stdext::hash_map<node,double> *posRelX, stdext::hash_map<node,double> *posRelY);
  void calcLayout(node n, stdext::hash_map<node,double> *px, stdext::hash_map<node,double> *py,
		  double x, double y, int level);
};
/*@}*/
#endif










