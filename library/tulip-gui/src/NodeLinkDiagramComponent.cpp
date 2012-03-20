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
#include "tulip/NodeLinkDiagramComponent.h"

#include "ui_GridOptionsWidget.h"

#include <QtGui/QGraphicsView>
#include <QtGui/QActionGroup>
#include <QtGui/QDialog>

#include <tulip/GlGrid.h>
#include <tulip/DrawingTools.h>
#include <tulip/TulipItemDelegate.h>
#include <tulip/ParameterListModel.h>
#include <tulip/GlMainWidget.h>
#include <tulip/GlGraphComposite.h>
#include <tulip/GlGraphInputData.h>

using namespace tlp;
using namespace std;

NodeLinkDiagramComponent::NodeLinkDiagramComponent(const tlp::PluginContext*): _grid(NULL), _gridOptions(NULL) {
}

void NodeLinkDiagramComponent::setState(const tlp::DataSet& data) {
  ParameterDescriptionList gridParameters;
  gridParameters.add<StringCollection>("Grid mode","","No grid;Space divisions;Fixed size",true);
  gridParameters.add<Size>("Grid size","","(1,1,1)",false);
  gridParameters.add<Size>("Margins","","(0.5,0.5,0.5)",false);
  gridParameters.add<Color>("Grid color","","(0,0,0,255)",false);
  ParameterListModel* model = new ParameterListModel(gridParameters,NULL,this);

  Ui::GridOptionsWidget* ui = new Ui::GridOptionsWidget;
  _gridOptions = new QDialog(graphicsView());
  ui->setupUi(_gridOptions);
  ui->tableView->setModel(model);
  ui->tableView->setItemDelegate(new TulipItemDelegate);
  connect(ui->tableView, SIGNAL(destroyed()), ui->tableView->itemDelegate(), SLOT(deleteLater()));

  getGlMainWidget()->setData(graph(), data);
  registerTriggers();

  graphicsView()->setContextMenuPolicy(Qt::ActionsContextMenu);

  QAction* overviewAction = new QAction(trUtf8("Overview"),this);
  overviewAction->setCheckable(true);
  overviewAction->setChecked(true);
  connect(overviewAction,SIGNAL(triggered(bool)),this,SLOT(setOverviewVisible(bool)));
  graphicsView()->addAction(overviewAction);

  QAction* viewSeparator = new QAction(trUtf8("View"),this);
  viewSeparator->setSeparator(true);
  graphicsView()->addAction(viewSeparator);

  QAction* redrawAction = new QAction(trUtf8("Redraw"),this);
  redrawAction->setShortcut(trUtf8("Ctrl+Shift+R"));
  connect(redrawAction,SIGNAL(triggered()),this,SLOT(redraw()));
  graphicsView()->addAction(redrawAction);

  QAction* centerAction = new QAction(trUtf8("Center"),this);
  centerAction->setShortcut(trUtf8("Ctrl+Shift+C"));
  connect(centerAction,SIGNAL(triggered()),getGlMainWidget(),SLOT(centerScene()));
  graphicsView()->addAction(centerAction);

  QAction* augmentedSeparator = new QAction(trUtf8("Augmented display"),this);
  augmentedSeparator->setSeparator(true);
  graphicsView()->addAction(augmentedSeparator);

  QAction* zOrderAction = new QAction(trUtf8("Z Ordering"),this);
  zOrderAction->setCheckable(true);
  zOrderAction->setChecked(false);
  connect(redrawAction,SIGNAL(triggered(bool)),this,SLOT(setZOrdering(bool)));
  graphicsView()->addAction(zOrderAction);

  QAction* gridAction = new QAction(trUtf8("Grid"),this);
  connect(gridAction,SIGNAL(triggered()),this,SLOT(showGridControl()));
  graphicsView()->addAction(gridAction);

  setOverviewVisible(true);
  setQuickAccessBarVisible(true);
  drawOverview(true);
}

void NodeLinkDiagramComponent::graphChanged(tlp::Graph* graph) {
  getGlMainWidget()->setGraph(graph);
  registerTriggers();
  emit drawNeeded();
}

tlp::DataSet NodeLinkDiagramComponent::state() const {
  return getGlMainWidget()->getData();
}

void NodeLinkDiagramComponent::registerTriggers() {
  clearRedrawTriggers();

  if (graph() == NULL)
    return;

  addRedrawTrigger(getGlMainWidget()->getGraph());
  std::set<tlp::PropertyInterface*> properties = getGlMainWidget()->getScene()->getGlGraphComposite()->getInputData()->properties();

  for(std::set<tlp::PropertyInterface*>::iterator it = properties.begin(); it != properties.end(); ++it) {
    addRedrawTrigger(*it);
  }
}

bool NodeLinkDiagramComponent::isLayoutProperty(tlp::PropertyInterface* pi) const {
  return pi == getGlMainWidget()->getScene()->getGlGraphComposite()->getInputData()->getElementLayout();
}

void NodeLinkDiagramComponent::redraw() {
  getGlMainWidget()->redraw();
}

void NodeLinkDiagramComponent::setZOrdering(bool f) {
  getGlMainWidget()->getScene()->getGlGraphComposite()->getRenderingParametersPointer()->setElementZOrdered(f);
}

void NodeLinkDiagramComponent::showGridControl() {
}

PLUGIN(NodeLinkDiagramComponent)
