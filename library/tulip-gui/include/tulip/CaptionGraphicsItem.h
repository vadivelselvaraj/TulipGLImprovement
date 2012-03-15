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
#ifndef CAPTIONGRAPHICSITEM_H
#define CAPTIONGRAPHICSITEM_H


#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QComboBox>
#include <QtOpenGL/QGLFramebufferObject>
#include <tulip/View.h>
#include <tulip/Color.h>

#include "tulip/CaptionGraphicsSubItems.h"


#include <QtGui/QGraphicsSceneMouseEvent>

namespace tlp {

class GlMainView;

class CaptionGraphicsItem : public QObject {

  Q_OBJECT

public:

  CaptionGraphicsItem(View *view);
  ~CaptionGraphicsItem();

  void loadConfiguration();

  void generateColorCaption(const QGradient &activeGradient, const QGradient &hideGradient, const std::string &propertyName, double minValue, double maxValue);

  void generateSizeCaption(const std::vector< std::pair <double,float> > &metricToSizeFilteredList,const std::string &propertyName, double minValue, double maxValue);

  CaptionGraphicsBackgroundItem *getCaptionItem() const {
    return _rondedRectItem;
  }

  std::string usedProperty();

signals :

  void filterChanged(float begin, float end);
  void selectedPropertyChanged(std::string propertyName);

  void interactionsActivated();
  void interactionsRemoved();

protected slots :

  void filterChangedSlot(float begin, float end);
  void selectedPropertyChangedSlot(const QString &propertyName);

  void emitInteractionsActivated() {
    emit interactionsActivated();
  }

  void emitInteractionsRemoved() {
    emit interactionsRemoved();
  }

private :

  void constructConfigWidget();

  View *_view;

  CaptionGraphicsBackgroundItem *_rondedRectItem;

  QGraphicsProxyWidget *_confPropertySelectionItem;
  QComboBox *_confPropertySelectionWidget;
};

}

#endif // CAPTIONGRAPHICSITEM_H
