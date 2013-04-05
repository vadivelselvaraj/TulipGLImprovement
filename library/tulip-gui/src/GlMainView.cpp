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
#include "tulip/GlMainView.h"
#include <tulip/GlMainWidget.h>
#include <tulip/SceneConfigWidget.h>
#include <tulip/SceneLayersConfigWidget.h>
#include <tulip/GlOverviewGraphicsItem.h>
#include <tulip/QuickAccessBar.h>
#include <tulip/GlGraphComposite.h>
#include <tulip/SnapshotDialog.h>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsView>
#include "tulip/GlMainWidgetGraphicsItem.h"

using namespace tlp;


GlMainView::GlMainView(): _glMainWidget(NULL), _overviewItem(NULL), isOverviewVisible(true), _quickAccessBarItem(NULL), _quickAccessBar(NULL), _sceneConfigurationWidget(NULL), _sceneLayersConfigurationWidget(NULL) {
}

GlMainView::~GlMainView() {
  delete _sceneConfigurationWidget;
  delete _sceneLayersConfigurationWidget;
  delete _overviewItem;
}

void GlMainView::draw() {
  _glMainWidget->draw();
}

void GlMainView::redraw() {
  _glMainWidget->redraw();
}

void GlMainView::refresh() {
  _glMainWidget->draw(false);
}

void GlMainView::drawOverview(bool generatePixmap) {
  if(_overviewItem == NULL) {
    _overviewItem=new GlOverviewGraphicsItem(this,*_glMainWidget->getScene());
    addToScene(_overviewItem);
    generatePixmap=true;
    // used to set the overview at the right place
    sceneRectChanged(QRectF(QPoint(0, 0), graphicsView()->size()));
  }

  _overviewItem->draw(generatePixmap);
}

void GlMainView::assignNewGlMainWidget(GlMainWidget *glMainWidget, bool deleteOldGlMainWidget) {
  _glMainWidget=glMainWidget;

  if(_sceneLayersConfigurationWidget == NULL) {
    _sceneLayersConfigurationWidget = new SceneLayersConfigWidget();
  }

  _sceneLayersConfigurationWidget->setGlMainWidget(_glMainWidget);
  connect(_sceneLayersConfigurationWidget,SIGNAL(drawNeeded()),this,SIGNAL(drawNeeded()));

  setCentralWidget(_glMainWidget,deleteOldGlMainWidget);
  GlMainWidgetGraphicsItem *glMainWidgetGraphicsItem=dynamic_cast<GlMainWidgetGraphicsItem*>(centralItem());
  delete _sceneConfigurationWidget;
  _sceneConfigurationWidget = new SceneConfigWidget();
  _sceneConfigurationWidget->setGlMainWidget(_glMainWidget);

  connect(glMainWidgetGraphicsItem,SIGNAL(widgetPainted(bool)),this,SLOT(glMainViewDrawn(bool)));
  connect(graphicsView()->scene(),SIGNAL(sceneRectChanged(QRectF)),this,SLOT(sceneRectChanged(QRectF)));
}

GlOverviewGraphicsItem *GlMainView::overviewItem() const {
  return _overviewItem;
}

void GlMainView::setupWidget() {
  graphicsView()->viewport()->parentWidget()->installEventFilter(this);
  assignNewGlMainWidget(new GlMainWidget(NULL,this),true);

  _forceRedrawAction=new QAction(trUtf8("Force redraw"),this);
  connect(_forceRedrawAction,SIGNAL(triggered()),this,SLOT(redraw()));
  _forceRedrawAction->setShortcut(tr("Ctrl+Shift+R"));
  _forceRedrawAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  _centerViewAction=new QAction(trUtf8("Center view"),this);
  connect(_centerViewAction,SIGNAL(triggered()),this,SLOT(centerView()));
  _centerViewAction->setShortcut(tr("Ctrl+Shift+C"));
  _centerViewAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

  graphicsView()->addAction(_centerViewAction);
  graphicsView()->addAction(_forceRedrawAction);
}

GlMainWidget* GlMainView::getGlMainWidget() const {
  return _glMainWidget;
}

void GlMainView::centerView(bool graphChanged) {
  getGlMainWidget()->centerScene(graphChanged);
}

void GlMainView::glMainViewDrawn(bool graphChanged) {
  if(isOverviewVisible)
    drawOverview(graphChanged);
}

QList<QWidget*> GlMainView::configurationWidgets() const {
  return QList<QWidget*>() << _sceneConfigurationWidget << _sceneLayersConfigurationWidget;
}

void GlMainView::setOverviewVisible(bool display) {
  isOverviewVisible=display;

  if(display) {
    drawOverview(true);
    _overviewItem->setVisible(true);
  }
  else if(_overviewItem)
    _overviewItem->setVisible(false);
}

bool GlMainView::overviewVisible() const {
  return isOverviewVisible;
}

void GlMainView::setViewOrtho(bool viewOrtho) {
  getGlMainWidget()->getScene()->setViewOrtho(viewOrtho);
  _glMainWidget->draw(false);
}

void GlMainView::setQuickAccessBarVisible(bool visible) {
  if (!visible) {
    delete _quickAccessBar;
    _quickAccessBar = NULL;
    _quickAccessBarItem = NULL;
  }

  else if (!quickAccessBarVisible()) {
    _quickAccessBarItem = new QGraphicsProxyWidget();
    _quickAccessBar = new QuickAccessBar(_quickAccessBarItem);
    connect(_quickAccessBar,SIGNAL(settingsChanged()),_sceneConfigurationWidget,SLOT(resetChanges()));
    connect(_sceneConfigurationWidget,SIGNAL(settingsApplied()),_quickAccessBar,SLOT(reset()));

    _quickAccessBar->setGlMainView(this);
    _quickAccessBarItem->setWidget(_quickAccessBar);
    addToScene(_quickAccessBarItem);
    _quickAccessBarItem->setZValue(10);
    sceneRectChanged(QRectF(QPoint(0, 0), graphicsView()->size()));
  }
}

bool GlMainView::quickAccessBarVisible() const {
  return _quickAccessBarItem != NULL;
}

void GlMainView::sceneRectChanged(const QRectF& rect) {
  if (_quickAccessBar != NULL) {
    _quickAccessBarItem->setPos(0,rect.height()-_quickAccessBarItem->size().height());
    _quickAccessBarItem->resize(rect.width(),_quickAccessBarItem->size().height());
  }
  if (_overviewItem != NULL)
    // put overview in the bottom right corner
    _overviewItem->setPos(rect.width() - _overviewItem->getWidth() - 1, rect.height() - _overviewItem->getHeight() - ((_quickAccessBar != NULL) ? _quickAccessBarItem->size().height() : 0));
}

QPixmap GlMainView::snapshot(const QSize &outputSize) const {
  if (_glMainWidget == NULL)
    return QPixmap();

  QSize realSize = outputSize;

  if (!realSize.isValid())
    realSize = _glMainWidget->size();

  return QPixmap::fromImage(_glMainWidget->createPicture(realSize.width(),realSize.height(),false));
}

void GlMainView::openSnapshotDialog() {
  SnapshotDialog dlg(this, getGlMainWidget()->parentWidget());
  dlg.exec();
}

void GlMainView::undoCallback() {
  getGlMainWidget()->getScene()->centerScene();
  draw();
}

void GlMainView::fillContextMenu(QMenu *menu, const QPointF &) {
  menu->addAction(trUtf8("View"))->setEnabled(false);
  menu->addSeparator();
  menu->addAction(_forceRedrawAction);
  menu->addAction(_centerViewAction);

  QAction* viewOrtho = menu->addAction(trUtf8("Use orthogonal projection"));
  viewOrtho->setCheckable(true);
  viewOrtho->setChecked(_glMainWidget->getScene()->isViewOrtho());
  connect(viewOrtho,SIGNAL(triggered(bool)),this,SLOT(setViewOrtho(bool)));

  menu->addAction(trUtf8("Take snapshot"),this,SLOT(openSnapshotDialog()));

  menu->addSeparator();
  menu->addAction(trUtf8("Augmented display"))->setEnabled(false);
  menu->addSeparator();

  QAction* a = menu->addAction(trUtf8("Show overview"),this,SLOT(setOverviewVisible(bool)));
  a->setCheckable(true);
  a->setChecked(overviewVisible());

  QAction* quickbarAction = menu->addAction(trUtf8("Show quick access bar"),this,SLOT(setQuickAccessBarVisible(bool)));
  quickbarAction->setCheckable(true);
  quickbarAction->setChecked(quickAccessBarVisible());
}

void GlMainView::applySettings() {
  _sceneConfigurationWidget->applySettings();
}

bool GlMainView::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::Resize) {
    // ensure automatic resize of the viewport
    QResizeEvent *resizeEvent = static_cast<QResizeEvent *>(event);
    graphicsView()->viewport()->setFixedSize(resizeEvent->size());
    // same for the configuration widgets
    QList<QWidget *> list = configurationWidgets();

    if(!list.isEmpty()) {   //test if the current view has a configuration widget
      QWidget *pqw = list.first()->parentWidget()->parentWidget();
      QSize sSize = pqw->size();
      sSize.setHeight(resizeEvent->size().height() - 50);
      pqw->resize(sSize);
      sSize.setHeight(resizeEvent->size().height() - 60);
      sSize = list.first()->size();
      foreach(QWidget *c, list) {   //resize each configuration widget
        c->resize(sSize);
      }
    }

    return true;
  }

  // standard event processing
  return ViewWidget::eventFilter(obj, event);
}
