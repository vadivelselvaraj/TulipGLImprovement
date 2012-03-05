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
#include "tulip/Interactor.h"

#include <tulip/ForEach.h>
#include <tulip/View.h>
#include <tulip/Glyph.h>
#include <QtCore/QDebug>

using namespace std;
using namespace tlp;

QMap<std::string,QList<std::string> > InteractorLister::_compatibilityMap = QMap<std::string,QList<std::string> >();

bool interactorLessThan(Interactor* a, Interactor* b) {
  return a->priority() > b->priority();
}

void InteractorLister::initInteractorsDependencies() {
  _compatibilityMap.clear();

  QMap<Interactor*,string> interactorToName;

  std::list<std::string> glyphs(PluginLister::instance()->availablePlugins<Glyph>());

  for(std::list<std::string>::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it) {
    string interactorName(*it);
    interactorToName[PluginLister::instance()->getPluginObject<Interactor>(interactorName,NULL)] = interactorName;
  }

  std::list<std::string> views(PluginLister::instance()->availablePlugins<View>());

  for(std::list<std::string>::const_iterator it = views.begin(); it != views.end(); ++it) {
    string viewName(*it);
    QList<Interactor*> compatibleInteractors;
    foreach(Interactor* i,interactorToName.keys()) {
      if (i->isCompatible(viewName))
        compatibleInteractors << i;
    }
    qSort(compatibleInteractors.begin(),compatibleInteractors.end(),interactorLessThan);

    QList<string> compatibleNames;
    foreach(Interactor* i,compatibleInteractors) {
      compatibleNames << interactorToName[i];
    }
    _compatibilityMap[viewName] = compatibleNames;

  }

  foreach(Interactor* i, interactorToName.keys()) {
    delete i;
  }
}

QList<string> InteractorLister::compatibleInteractors(const std::string &viewName) {
  return _compatibilityMap[viewName];
}
