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

#ifndef PROPERTYVALUESDISPATCHER_H
#define PROPERTYVALUESDISPATCHER_H

#include <tulip/Observable.h>
#include <tulip/ObservableProperty.h>
#include <tulip/ObservableGraph.h>

namespace tlp {
class IntegerVectorProperty;
class BooleanProperty;
class IntegerProperty;
}

#include <set>
#include <string>

class PropertyValuesDispatcher: public tlp::PropertyObserver, public tlp::GraphObserver, public tlp::Observable {
public:
  PropertyValuesDispatcher(tlp::Graph *source, tlp::Graph *target,
                           const std::set<std::string> &sourceToTargetProperties, const std::set<std::string> &targetToSourceProperties,
                           tlp::IntegerVectorProperty *_graphEntitiesToDisplayedNodes, tlp::BooleanProperty *_displayedNodesAreNodes, tlp::IntegerProperty *displayedNodesToGraphEntities);
  virtual void afterSetNodeValue(tlp::PropertyInterface*, const tlp::node);
  virtual void afterSetEdgeValue(tlp::PropertyInterface*, const tlp::edge);
  virtual void afterSetAllNodeValue(tlp::PropertyInterface*);
  virtual void afterSetAllEdgeValue(tlp::PropertyInterface*);
  virtual void addLocalProperty(tlp::Graph*, const std::string&);
  virtual void treatEvent(const tlp::Event&);

private:
  tlp::Graph *_source;
  tlp::Graph *_target;
  tlp::IntegerVectorProperty *_graphEntitiesToDisplayedNodes;
  tlp::BooleanProperty *_displayedNodesAreNodes;
  tlp::IntegerProperty *_displayedNodesToGraphEntities;
  std::set<std::string> _sourceToTargetProperties;
  std::set<std::string> _targetToSourceProperties;
  bool _modifying;
};

#endif // PROPERTYVALUESDISPATCHER_H