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

#ifndef TULIP_SIZES_H
#define TULIP_SIZES_H

#include <tulip/tuliphash.h>
#include <tulip/PropertyTypes.h>
#include <tulip/AbstractProperty.h>
#include <tulip/PropertyAlgorithm.h>

namespace tlp {

class PropertyContext;

typedef AbstractProperty<tlp::SizeType, tlp::SizeType> AbstractSizeProperty;

/**
 * @ingroup Graph
 * @brief The SizeProperty class
 */
class TLP_SCOPE SizeProperty:public AbstractSizeProperty {

public :
  SizeProperty (Graph *, std::string n="");

  Size getMax(Graph *sg=NULL);
  Size getMin(Graph *sg=NULL);
  void scale( const tlp::Vector<float,3>&, Graph *sg=NULL );
  void scale( const tlp::Vector<float,3>&, Iterator<node> *, Iterator<edge> *);

  // redefinition of some PropertyInterface methods
  PropertyInterface* clonePrototype(Graph *, const std::string& );
  static const std::string propertyTypename;
  std::string getTypename() const {
    return propertyTypename;
  }


  // redefinition of some AbstractProperty methods
  virtual void setNodeValue(const node n, const Size &v);
  virtual void setAllNodeValue(const Size &v);

  /**
    * @brief Compare sizes of n1 and n2 using their volume.
    **/
  int compare(const node n1, const node n2)const;

protected:
  void resetMinMax();

private:
  TLP_HASH_MAP<unsigned int, Size> max,min;
  TLP_HASH_MAP<unsigned int, bool> minMaxOk;
  void computeMinMax(Graph * sg=NULL);
};

class TLP_SCOPE SizeVectorProperty:public AbstractVectorProperty<tlp::SizeVectorType, tlp::SizeType> {
public :
  SizeVectorProperty(Graph *g, std::string n=""):AbstractVectorProperty<SizeVectorType, SizeType>(g, n) {}

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
