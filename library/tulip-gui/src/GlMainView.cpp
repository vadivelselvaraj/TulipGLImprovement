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
#include <tulip/GlOverviewGraphicsItem.h>
#include <tulip/QuickAccessBar.h>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QGraphicsView>

using namespace tlp;


GlMainView::GlMainView(): _overviewVisible(false), _glMainWidget(NULL), _overviewItem(NULL), _sceneConfigurationWidget(NULL), _quickAccessBar(NULL), _quickAccessBarItem(NULL),_colorCaption(NULL),_sizeCaption(NULL) {
}

GlMainView::~GlMainView() {
  delete _sceneConfigurationWidget;
}

void GlMainView::draw(tlp::PluginProgress* progress) {
  _glMainWidget->draw();
}

void GlMainView::refresh(PluginProgress *) {
  _glMainWidget->draw(false);
}

void GlMainView::drawOverview(bool generatePixmap) {
  if(!_overviewVisible)
    return;

  if(_overviewItem == NULL) {
    _overviewItem=new GlOverviewGraphicsItem(this,*_glMainWidget->getScene());
    addToScene(_overviewItem);
    _overviewItem->setPos(QPointF(0,0));
    generatePixmap=true;
  }

  if(!_overviewItem->isVisible()) {
    return;
  }

  _overviewItem->draw(generatePixmap);
}

void GlMainView::showHideCaption(CaptionItem::CaptionType captionType) {
  if(_colorCaption==NULL) {
    _colorCaption=new CaptionItem(this);
    _colorCaption->create(CaptionItem::ColorCaption);
    addToScene(_colorCaption->captionGraphicsItem());
    _colorCaption->captionGraphicsItem()->setVisible(false);
  }

  if(_sizeCaption==NULL) {
    _sizeCaption=new CaptionItem(this);
    _sizeCaption->create(CaptionItem::SizeCaption);
    addToScene(_sizeCaption->captionGraphicsItem());
    _sizeCaption->captionGraphicsItem()->setVisible(false);
    connect(_sizeCaption->captionGraphicsItem(),SIGNAL(interactionsActivated()),_colorCaption->captionGraphicsItem(),SLOT(removeInteractions()));
    connect(_colorCaption->captionGraphicsItem(),SIGNAL(interactionsActivated()),_sizeCaption->captionGraphicsItem(),SLOT(removeInteractions()));
  }

  if(captionType==CaptionItem::ColorCaption) {
    //_colorCaption->activateInteractions(!_colorCaption->captionGraphicsItem()->isVisible());

    if(_colorCaption->captionGraphicsItem()->isVisible()) {
      _colorCaption->captionGraphicsItem()->setVisible(false);

      if(_sizeCaption->captionGraphicsItem()->isVisible())
        _sizeCaption->captionGraphicsItem()->setPos(_captionPos);
    }
    else {
      _colorCaption->captionGraphicsItem()->setVisible(true);

      if(_sizeCaption->captionGraphicsItem()->isVisible()) {
        _colorCaption->captionGraphicsItem()->setPos(_captionPos+QPoint(130,0));
      }
      else {
        _colorCaption->captionGraphicsItem()->setPos(_captionPos);
      }
    }
  }
  else {
    //_sizeCaption->activateInteractions(!_colorCaption->captionGraphicsItem()->isVisible());

    if(_sizeCaption->captionGraphicsItem()->isVisible()) {
      _sizeCaption->captionGraphicsItem()->setVisible(false);

      if(_colorCaption->captionGraphicsItem()->isVisible())
        _colorCaption->captionGraphicsItem()->setPos(_captionPos);
    }
    else {
      _sizeCaption->captionGraphicsItem()->setVisible(true);

      if(_colorCaption->captionGraphicsItem()->isVisible()) {
        _sizeCaption->captionGraphicsItem()->setPos(_captionPos+QPoint(130,0));
      }
      else {
        _sizeCaption->captionGraphicsItem()->setPos(_captionPos);
      }
    }
  }
}

void GlMainView::setupWidget() {
  _glMainWidget = new GlMainWidget(NULL,this);
  setCentralWidget(_glMainWidget);
  delete _sceneConfigurationWidget;
  _sceneConfigurationWidget = new SceneConfigWidget();
  _sceneConfigurationWidget->setGlMainWidget(_glMainWidget);
  connect(_glMainWidget,SIGNAL(viewDrawn(GlMainWidget*,bool)),this,SLOT(glMainViewDrawn(GlMainWidget*,bool)));
  connect(graphicsView()->scene(),SIGNAL(sceneRectChanged(QRectF)),this,SLOT(sceneRectChanged(QRectF)));
}

GlMainWidget* GlMainView::getGlMainWidget() const {
  return _glMainWidget;
}

void GlMainView::centerView() {
  getGlMainWidget()->getScene()->centerScene();
}

void GlMainView::glMainViewDrawn(GlMainWidget *, bool graphChanged) {
  drawOverview(graphChanged);
}

QList<QWidget*> GlMainView::configurationWidgets() const {
  return QList<QWidget*>() << _sceneConfigurationWidget;
}

void GlMainView::setOverviewVisible(bool display) {
  if (!display) {
    delete _overviewItem;
    _overviewItem = NULL;
  }

  _overviewVisible = display;
}

bool GlMainView::overviewVisible() const {
  return _overviewVisible;
}

void GlMainView::setQuickAccessBarVisible(bool visible) {
  if (!visible) {
    delete _quickAccessBar;
    delete _quickAccessBarItem;
    _quickAccessBar = NULL;
    _quickAccessBarItem = NULL;
  }

  else if (!quickAccessBarVisible()) {
    _quickAccessBar = new QuickAccessBar();
    _quickAccessBar->setGlMainView(this);
    _quickAccessBarItem = new QGraphicsProxyWidget();
    _quickAccessBarItem->setWidget(_quickAccessBar);
    addToScene(_quickAccessBarItem);
    _quickAccessBarItem->setZValue(10);
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

  if(_quickAccessBar != NULL)
    _captionPos=QPointF(0,rect.height()-230-_quickAccessBarItem->size().height());
  else
    _captionPos=QPointF(0,rect.height()-230);
}

QPixmap GlMainView::snapshot(const QSize &outputSize) {
  if (_glMainWidget == NULL)
    return QPixmap();

  QSize realSize = outputSize;

  if (!realSize.isValid())
    realSize = _glMainWidget->size();

  return QPixmap::fromImage(_glMainWidget->createPicture(realSize.width(),realSize.height()));
}

void GlMainView::graphDeleted() {
  setGraph(NULL);
}

void GlMainView::applySettings() {
  _sceneConfigurationWidget->applySettings();
}
