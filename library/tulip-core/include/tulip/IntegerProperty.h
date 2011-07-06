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
#ifndef TULIP_INT_H
#define TULIP_INT_H

#include <tulip/tuliphash.h>
#include <tulip/PropertyTypes.h>
#include <tulip/AbstractProperty.h>
#include <tulip/PropertyAlgorithm.h>
#include <tulip/Observable.h>
#include <tulip/ObservableGraph.h>


namespace tlp {

class Graph;
class PropertyContext;

typedef AbstractProperty<tlp::IntegerType, tlp::IntegerType, tlp::IntegerAlgorithm> AbstractIntegerProperty;
/** \addtogroup properties */
/*@{*/
class TLP_SCOPE IntegerProperty:public AbstractIntegerProperty, private GraphObserver {

public :
  IntegerProperty(Graph *, std::string n = "");
  IntegerType::RealType  getNodeMin(Graph *sg=0);
  IntegerType::RealType  getNodeMax(Graph *sg=0);
  IntegerType::RealType  getEdgeMin(Graph *sg=0);
  IntegerType::RealType  getEdgeMax(Graph *sg=0);

  // redefinition of some PropertyInterface methods
  PropertyInterface* clonePrototype(Graph *, const std::string& );
  static const std::string propertyTypename;
  std::string getTypename() const {
    return propertyTypename;
  }


  // redefinition of some AbstractProperty methods
  virtual void setNodeValue(const node n, const int &v);
  virtual void setEdgeValue(const edge e, const int &v);
  virtual void setAllNodeValue(const int &v);
  virtual void setAllEdgeValue(const int &v);

  /**
    * @brief Specific implementation of AbstractProperty::compare(node n1,node n2)
    **/
  int compare(node n1,node n2)const;

  /**
    * @brief Specific implementation of AbstractProperty::compare(edge e1,edge e2)
    **/
  int compare(edge e1,edge e2)const;

protected:
  virtual void clone_handler(AbstractProperty<IntegerType,IntegerType, IntegerAlgorithm> &);

private:
  // override some GraphObserver methods
  void addNode(Graph* graph, const node n);
  void addEdge(Graph* graph, const edge e);
  void delNode(Graph* graph, const node n);
  void delEdge(Graph* graph, const edge e);
  void addSubGraph(Graph* graph, Graph *sub);
  void delSubGraph(Graph* graph, Graph *sub);
  // override Observable::treatEvent
  void treatEvent(const Event&);

  TLP_HASH_MAP<unsigned int, int> maxN,minN,maxE,minE;
  TLP_HASH_MAP<unsigned int, bool> minMaxOkNode;
  TLP_HASH_MAP<unsigned int, bool> minMaxOkEdge;
  void computeMinMaxNode(Graph *sg=0);
  void computeMinMaxEdge(Graph *sg=0);
};

class TLP_SCOPE IntegerVectorProperty:public AbstractVectorProperty<tlp::IntegerVectorType, tlp::IntegerType> {
public :
  IntegerVectorProperty(Graph *g, std::string n =""):AbstractVectorProperty<IntegerVectorType, tlp::IntegerType>(g, n) {}
  // redefinition of some PropertyInterface methods
  PropertyInterface* clonePrototype(Graph *, const std::string& );
  static const std::string propertyTypename;
  std::string getTypename() const {
    return propertyTypename;
  }

};
/*@}*/

}
#endif
