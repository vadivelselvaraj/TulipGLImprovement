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
#include "tulip/AbstractView.h"

#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QFrame>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QMenu>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMenuBar>
#include <QtGui/QImageWriter>
#include <QtGui/QFileDialog>
#include <QtCore/QDebug>

#include "tulip/Interactor.h"

using namespace std;

namespace tlp {

AbstractView::AbstractView() :
  View3(), centralWidget(NULL), activeInteractor(NULL) {

}

AbstractView::~AbstractView() {
  for (list<Interactor *>::iterator it = interactors.begin(); it != interactors.end(); ++it) {
    delete (*it);
  }
}

QWidget *AbstractView::construct(QWidget *parent) {
  widget = new QWidget(parent);
  QGridLayout *gridLayout = new QGridLayout(widget);
  gridLayout->setSpacing(0);
  gridLayout->setMargin(0);

  mainLayout = new QVBoxLayout;

  gridLayout->addLayout(mainLayout, 0, 0, 1, 1);

  //Build output image list
  exportImageMenu = new QMenu("&Export in EPS or SVG ",widget);

  set<string> imgFormats;
  imgFormats.insert("EPS");
  imgFormats.insert("SVG");

  for (set<string>::iterator it = imgFormats.begin(); it != imgFormats.end(); ++it) {
    exportImageMenu->addAction(QString::fromStdString(*it));
  }

  connect(exportImageMenu, SIGNAL(triggered(QAction*)), SLOT(exportImage(QAction*)));

  widget->installEventFilter(this);

  return widget;
}

void AbstractView::setInteractors(const std::list<Interactor *> &interactorsList) {
  interactors = interactorsList;

  for (list<Interactor *>::iterator it = interactors.begin(); it != interactors.end(); ++it) {
    (*it)->setView(this);
  }
}

list<Interactor *> AbstractView::getInteractors() {
  return interactors;
}

void AbstractView::setActiveInteractor(Interactor *interactor) {
  Interactor *currentInteractor = activeInteractor;
  activeInteractor = NULL;

  if (currentInteractor)
    currentInteractor->uninstall();

  interactor->install(centralWidget);
  activeInteractor = interactor;
}

void AbstractView::setCentralWidget(QWidget *widget) {
  if (centralWidget) {
    mainLayout->removeWidget(centralWidget);
    centralWidget->setParent(0);
  }

  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  mainLayout->addWidget(widget);
  centralWidget = widget;

  if (activeInteractor)
    setActiveInteractor(activeInteractor);

  centralWidget->activateWindow();
  centralWidget->setFocus();
  centralWidget->show();
}

bool AbstractView::eventFilter(QObject *object, QEvent *event) {
  specificEventFilter(object, event);

  if (event->type() == QEvent::ContextMenu) {
    QContextMenuEvent *me = (QContextMenuEvent*) event;
    QMenu contextMenu(getWidget());
    buildContextMenu(object, me, &contextMenu);

    if (!contextMenu.actions().isEmpty()) {
      QAction* menuAction = contextMenu.exec(me->globalPos());

      if (menuAction)
        computeContextMenuAction(menuAction);
    }
  }

  return false;
}

void AbstractView::exportImage(QAction* action) {
  QString extension = action->text().toLower();
  QString s(QFileDialog::getSaveFileName(NULL, QString("Save Picture as ") + extension + " file", QString(), QString("Images (*.") + extension + ")"));

  if (s.isNull()) {
    return;
  }

  // If the format extension is not found automatically add it
  if(!s.endsWith(QString('.').append(extension))) {
    s.append('.');
    s.append(extension);
  }

  createPicture(s.toStdString(), centralWidget->size().width(),
                centralWidget->size().height(),false);
}

void AbstractView::buildContextMenu(QObject *, QContextMenuEvent *, QMenu *contextMenu) {
  if (!exportImageMenu->isEmpty())
    contextMenu->addMenu(exportImageMenu);
}

bool AbstractView::savePicture(const string& pictureName, int width, int height, bool center, int zoom , int xOffset , int yOffset ) {
  QImage image = createPicture(width,height,center,zoom,xOffset,yOffset);

  if(!image.isNull()) {
    return image.save(pictureName.c_str());
  }
  else {
    return false;
  }
}

}
