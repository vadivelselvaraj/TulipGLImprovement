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

#ifndef TULIP_INT_H
#define TULIP_INT_H

#include <tulip/tuliphash.h>
#include <tulip/PropertyTypes.h>
#include <tulip/AbstractProperty.h>
#include <tulip/PropertyAlgorithm.h>
#include <tulip/Observable.h>
#include <tulip/minmaxproperty.h>

namespace tlp {

class Graph;
class PropertyContext;

typedef MinMaxProperty<tlp::IntegerType, tlp::IntegerType> IntegerMinMaxProperty;

/**
 * @ingroup Graph
 * @brief A graph property that maps an integer value to graph elements.
 */
class TLP_SCOPE IntegerProperty : public IntegerMinMaxProperty {

public :
  IntegerProperty(Graph *, std::string n = "");

  PropertyInterface* clonePrototype(Graph *, const std::string& );
  static const std::string propertyTypename;
  std::string getTypename() const {
    return propertyTypename;
  }
  virtual void setNodeValue(const node n, const int &v);
  virtual void setEdgeValue(const edge e, const int &v);
  virtual void setAllNodeValue(const int &v);
  virtual void setAllEdgeValue(const int &v);

  int compare(const node n1, const node n2) const;
  int compare(const edge e1, const edge e2) const;

protected:
  virtual void clone_handler(AbstractProperty<tlp::IntegerType, tlp::IntegerType> &);

private:
  // override Observable::treatEvent
  void treatEvent(const Event&);
};

/**
 * @ingroup Graph
 * @brief A graph property that maps a std::vector<int> value to graph elements.
 */
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


}
#endif
///@endcond
