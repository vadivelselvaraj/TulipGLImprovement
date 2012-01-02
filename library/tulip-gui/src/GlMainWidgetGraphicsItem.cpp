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
#include "tulip/GlMainWidgetGraphicsItem.h"

#include <QtOpenGL/QGLFramebufferObject>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QGraphicsScene>

#include <tulip/GlTextureManager.h>
#include <tulip/GlQuad.h>
#include <tulip/GlTools.h>
#include <tulip/GlMainWidget.h>

using namespace std;
using namespace tlp;

static void setRasterPosition(unsigned int x, unsigned int y) {
  float val[4];
  unsigned char tmp[10];
  glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&val);
  glBitmap(0,0,0,0,-val[0] + x, -val[1] + y, tmp);
  glGetFloatv(GL_CURRENT_RASTER_POSITION, (float*)&val);
  tlp::glTest(__PRETTY_FUNCTION__);
}

GlMainWidgetGraphicsItem::GlMainWidgetGraphicsItem(GlMainWidget *glMainWidget, int width, int height):
  QGraphicsObject(),
  glMainWidget(glMainWidget), redrawNeeded(true), renderingStore(NULL) {

//  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);
  setHandlesChildEvents(false);

  connect(glMainWidget,SIGNAL(viewDrawn(GlMainWidget *,bool)),this,SLOT(glMainWidgetDraw(GlMainWidget *,bool)));
  connect(glMainWidget,SIGNAL(viewRedrawn(GlMainWidget *)),this,SLOT(glMainWidgetRedraw(GlMainWidget *)));

  resize(width, height);
  glMainWidget->installEventFilter(this);
  setHandlesChildEvents(false);
}

GlMainWidgetGraphicsItem::~GlMainWidgetGraphicsItem() {
  delete [] renderingStore;
  glMainWidget->removeEventFilter(this);
}

QRectF GlMainWidgetGraphicsItem::boundingRect() const {
  return QRectF(0, 0, width, height);
}

void GlMainWidgetGraphicsItem::resize(int width, int height) {
  if(this->width == width && this->height == height)
    return;

  this->width = width;
  this->height = height;
  glMainWidget->resize(width,height);
  glMainWidget->resizeGL(width,height);
  redrawNeeded = true;
  delete [] renderingStore;
  renderingStore = new unsigned char[width*height*4];
  prepareGeometryChange();
}

void GlMainWidgetGraphicsItem::glMainWidgetDraw(GlMainWidget *,bool) {
  redrawNeeded=true;
  update();
}

void GlMainWidgetGraphicsItem::glMainWidgetRedraw(GlMainWidget *) {
  update();
}


void GlMainWidgetGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  QRectF rect = boundingRect();

  if (pos().x() < 0 || pos().x()+rect.width() > scene()->width() || pos().y() < 0 || pos().y()+rect.height() > scene()->height())
    redrawNeeded = true;

  QPainterPath path;
  path.addRect(rect.x(),rect.y(),rect.width(),rect.height());
  Color backgroundColor = glMainWidget->getScene()->getBackgroundColor();

  painter->setBrush(QColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]));
  painter->setPen(Qt::transparent);
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->drawPath(path);
  painter->setRenderHint(QPainter::Antialiasing, false);

  painter->beginNativePainting();

  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  rect = rect.translated(pos());

  float vpX = rect.x();
  float vpY = scene()->height() - (rect.y() + rect.height());
  float vpW = rect.width();
  float vpH = rect.height();

  glMainWidget->getScene()->setViewport(vpX,vpY,vpW,vpH);
  glMainWidget->getScene()->setClearBufferAtDraw(true);
  glMainWidget->getScene()->initGlParameters();

  if(redrawNeeded) {
    glMainWidget->computeInteractors();
    glMainWidget->getScene()->draw();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    glReadBuffer(GL_BACK);
    glReadPixels(vpX,vpY,vpW,vpH,GL_RGBA,GL_UNSIGNED_BYTE,renderingStore);
    glFlush();

    redrawNeeded=false;
  }
  else {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    glDrawBuffer(GL_BACK);
    setRasterPosition(vpX,vpY);
    glDrawPixels(vpW,vpH,GL_RGBA,GL_UNSIGNED_BYTE,renderingStore);
    glFlush();
  }

  glMainWidget->drawForegroundEntities();
  glMainWidget->drawInteractors();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glPopAttrib();

  painter->endNativePainting();
}

void GlMainWidgetGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent *eventModif=new QMouseEvent(QEvent::MouseMove,QPoint(event->pos().x(),event->pos().y()), Qt::NoButton, event->buttons(), event->modifiers());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent *eventModif=new QMouseEvent(QEvent::MouseButtonPress,QPoint(event->pos().x(),event->pos().y()), event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent *eventModif=new QMouseEvent(QEvent::MouseButtonDblClick,QPoint(event->pos().x(),event->pos().y()), event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QMouseEvent *eventModif=new QMouseEvent(QEvent::MouseButtonRelease,QPoint(event->pos().x(),event->pos().y()), event->button(), event->buttons(), event->modifiers());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event) {
  QWheelEvent *eventModif=new QWheelEvent(QPoint(event->pos().x(),event->pos().y()), event->delta(),event->buttons(), event->modifiers(),event->orientation());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent * event) {
  QMouseEvent *eventModif=new QMouseEvent(QEvent::MouseMove,QPoint(event->pos().x(),event->pos().y()), Qt::NoButton, Qt::NoButton, event->modifiers());
  QApplication::sendEvent(glMainWidget,eventModif);
}

void GlMainWidgetGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event) {
  QContextMenuEvent *eventModif = new QContextMenuEvent(static_cast<QContextMenuEvent::Reason>(event->reason()), QPoint(event->pos().x(),event->pos().y()));
  QApplication::sendEvent(glMainWidget,eventModif);
}

bool GlMainWidgetGraphicsItem::eventFilter(QObject *, QEvent *evt) {
  if (evt->type() == QEvent::CursorChange)
    setCursor(glMainWidget->cursor());

  return false;
}
