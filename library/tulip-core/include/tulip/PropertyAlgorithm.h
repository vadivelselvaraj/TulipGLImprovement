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

/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
*/

#ifndef TULIP_PROPERTY_H
#define TULIP_PROPERTY_H

#include <tulip/PluginContext.h>
#include <tulip/Graph.h>
#include <tulip/DataSet.h>
#include <tulip/WithParameter.h>
#include <tulip/WithDependency.h>
#include <tulip/PluginLister.h>

namespace tlp {

/**
 * \addtogroup plugins
 */
/*@{*/
///
/**
 * @brief This base class describes plug-ins who only modify one property, e.g. selection.
 **/
class TLP_SCOPE PropertyAlgorithm: public WithParameter, public WithDependency {
public :
  /**
   * @brief Builds a new plug-in that modifies a single property.
   *
   * @param context The context containing the graph and PropertyInterface this plug-in has access to, as well as a PluginProgress.
   **/
  PropertyAlgorithm(const tlp::PropertyContext & context) :
    graph(context.graph),
    pluginProgress(context.pluginProgress),
    dataSet(context.dataSet) {
  }
  ///
  virtual ~PropertyAlgorithm() {}

  /**
   * @brief Runs the algorithm.
   * It is a good practice to report progress through the PluginProgress, Even if your algorithm is very fast.
   * Keep in mind that Tulip can handle very large graphs.
   * The PluginProgress should also be used to report errors, if any.
   *
   * @return bool Whether the algorithm execution was successful or not.
   **/
  virtual bool run() {
    return true;
  }

  /**
   * @brief Checks whether the algorithm can be applied on this graph or not.
   * If not, the reason why should be reported through the PluginProgress.
   *
   * @param errorMessage A string whose value will be modified to an error message, if the check fails.
   * @return bool Whether the plug-in can run on this Graph.
   **/
  virtual bool check(std::string &errorMessage) {
    (void)errorMessage;
    return true;
  }
  ///
  bool preservePropertyUpdates(PropertyInterface* prop) {
    return graph->nextPopKeepPropertyUpdates(prop);
  }

  Graph *graph;
  PluginProgress *pluginProgress;
  DataSet *dataSet;
};

template<class Property>
class TLP_SCOPE TemplateAlgorithm : public PropertyAlgorithm {
public:
  Property* result;
  virtual ~TemplateAlgorithm() {}
protected:
  TemplateAlgorithm (const tlp::PropertyContext  &context) : tlp::PropertyAlgorithm(context) {
    result = (Property*)context.propertyProxy;
  }
};

class SizeProperty;
class IntegerProperty;
class LayoutProperty;
class ColorProperty;
class DoubleProperty;
class StringProperty;
class BooleanProperty;
typedef TemplateAlgorithm<SizeProperty> SizeAlgorithm;
typedef TemplateAlgorithm<IntegerProperty> IntegerAlgorithm;
typedef TemplateAlgorithm<LayoutProperty> LayoutAlgorithm;
typedef TemplateAlgorithm<ColorProperty> ColorAlgorithm;
typedef TemplateAlgorithm<DoubleProperty> DoubleAlgorithm;
typedef TemplateAlgorithm<StringProperty> StringAlgorithm;
typedef TemplateAlgorithm<BooleanProperty> BooleanAlgorithm;

#ifdef WIN32

#ifdef _MSC_VER //MSVC needs explicit instantiations of the templates
template class TemplateAlgorithm<SizeProperty>;
template class TemplateAlgorithm<IntegerProperty>;
template class TemplateAlgorithm<LayoutProperty>;
template class TemplateAlgorithm<ColorProperty>;
template class TemplateAlgorithm<DoubleProperty>;
template class TemplateAlgorithm<StringProperty>;
template class TemplateAlgorithm<BooleanProperty>;
#endif

template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<SizeProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<IntegerProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<LayoutProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<ColorProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<DoubleProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<StringProperty>, PropertyContext>;
template class TLP_SCOPE tlp::PluginLister<TemplateAlgorithm<BooleanProperty>, PropertyContext>;
#endif

/*@}*/

}
#endif
