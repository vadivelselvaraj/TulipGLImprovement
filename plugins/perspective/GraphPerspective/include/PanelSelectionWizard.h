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
#ifndef PANELSELECTIONWIZARD_H
#define PANELSELECTIONWIZARD_H

#include <QtCore/QModelIndex>
#include <QtGui/QWizard>

namespace tlp {
class Graph;
class Plugin;
class GraphHierarchiesModel;
class View;
}

namespace Ui {
class PanelSelectionWizard;
class PanelSelectionItem;
}

class PanelSelectionItem: public QWidget {
  Q_OBJECT
  Ui::PanelSelectionItem* _ui;
  QString _viewName;
public:
  explicit PanelSelectionItem(const tlp::Plugin* infos, QWidget* parent = 0);
  virtual ~PanelSelectionItem();
  QString viewName() const;
signals:
  void focused(bool);
  void selected();
public slots:
  void setFocus(bool);
protected:
  void mousePressEvent(QMouseEvent *);
  void mouseDoubleClickEvent(QMouseEvent *);
};

class PanelSelectionWizard : public QWizard {
  Q_OBJECT

  QWizardPage* _placeHolder;
  Ui::PanelSelectionWizard* _ui;
  tlp::GraphHierarchiesModel* _model;
  tlp::View* _view;
  PanelSelectionItem* _currentItem;

  void createView();
  void clearView();
  QList<PanelSelectionItem*> items() const;
public:
  explicit PanelSelectionWizard(tlp::GraphHierarchiesModel* model, QWidget *parent = 0);
  virtual ~PanelSelectionWizard();

  tlp::Graph* graph() const;
  QString panelName() const;
  tlp::View* panel() const;
  void setSelectedGraph(tlp::Graph*);

protected slots:
  void panelFocused(bool);
  void pageChanged(int);
protected:
  void done(int result);
};

#endif // PANELSELECTIONWIZARD_H
