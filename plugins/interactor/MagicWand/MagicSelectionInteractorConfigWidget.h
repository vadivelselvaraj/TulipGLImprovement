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
#ifndef MAGICSELECTIONINTERACTORCONFIGWIDGET_H
#define MAGICSELECTIONINTERACTORCONFIGWIDGET_H

#include <QtGui/QWidget>
#include "ui_MagicSelectionInteractorConfigWidget.h"
#include <tulip/Observable.h>

namespace tlp {
class Graph;
class PropertyInterface;
}

enum Selectionbehavior {
  Replace = 0,
  Add = 1,
  Remove = 2,
  Intersect = 3
};

class MagicSelectionInteractorConfigWidget : public QWidget, public Ui_MagicSelectionInteractorConfigWidget, public tlp::Observable {
  Q_OBJECT
public:
  MagicSelectionInteractorConfigWidget(QWidget* parent = NULL);

  void setGraph(tlp::Graph* graph);

  virtual void observableDestroyed(tlp::Observable* ) {}
  virtual void treatEvents(const std::vector<tlp::Event> &);

  QString propertyName() const;
  Selectionbehavior selectionBehavior() const;
  bool directedSelection() const;
  double upperBound() const;
  double lowerBound() const;

private:
  tlp::Graph* _graph;
  tlp::PropertyInterface* _currentProperty;
  void updateAvailableProperties();
public slots:
  void pushButtontoggled(bool);
  void selectedPropertyChanged(const QString&);
};

#endif // MAGICSELECTIONINTERACTORCONFIGWIDGET_H
