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
#include "tulip/CaptionGraphicsItem.h"

#include <QtGui/QGraphicsProxyWidget>

#include <tulip/DoubleProperty.h>
#include <tulip/ColorProperty.h>

using namespace std;

namespace tlp {

CaptionGraphicsItem::CaptionGraphicsItem(View *view):_view(view) {

  _rondedRectItem=new CaptionGraphicsBackgroundItem(QRect(QPoint(0,0),QSize(130,220)));
  _rondedRectItem->setBrush(QBrush(QColor(255,255,255,180)));
  connect(_rondedRectItem,SIGNAL(filterChanged(float,float)),this,SLOT(filterChangedSlot(float,float)));
  connect(_rondedRectItem,SIGNAL(configurationIconPressed()),this,SLOT(configurationIconPressed()));

  _confBackgroundRectItem = new QGraphicsRectItem(130,0,170,100);
  _confBackgroundRectItem->setParentItem(_rondedRectItem);
  _confBackgroundRectItem->setBrush(QBrush(QColor(255,255,255,180)));

  _confBackgroundRectItem->setVisible(false);

  _confPropertySelectionWidget = new QComboBox();
  _confPropertySelectionWidget->resize(QSize(150,30));
  _confPropertySelectionItem = new QGraphicsProxyWidget(_confBackgroundRectItem);
  _confPropertySelectionItem->setWidget(_confPropertySelectionWidget);
  _confPropertySelectionItem->setPos(140,10);
}

void CaptionGraphicsItem::loadConfiguration(){
  constructConfigWidget();
}

void CaptionGraphicsItem::generateCaption(const QGradient &activeGradient, const QGradient &hideGradient, const string &propertyName, double minValue, double maxValue){
  _rondedRectItem->generateCaption(activeGradient,hideGradient,propertyName,minValue,maxValue);
}

void CaptionGraphicsItem::constructConfigWidget() {

  disconnect(_confPropertySelectionWidget,SIGNAL(currentIndexChanged (const QString &)),this,SLOT(selectedPropertyChangedSlot(const QString &)));
  QString selectedItem=_confPropertySelectionWidget->currentText();
  int index=-1;
  _confPropertySelectionWidget->clear();
  QStringList properties;
  Iterator<PropertyInterface*>* itP=_view->graph()->getLocalObjectProperties();
  while(itP->hasNext()){
    PropertyInterface *property=itP->next();
    if(property->getTypename()=="double"){
      if(property->getName().c_str() == selectedItem)
        index=properties.size();
      properties << property->getName().c_str() ;
    }
  }
  cout << properties.size() << endl;
  _confPropertySelectionWidget->addItems(properties);
  if(index!=-1)
    _confPropertySelectionWidget->setCurrentIndex(index);
  connect(_confPropertySelectionWidget,SIGNAL(currentIndexChanged (const QString &)),this,SLOT(selectedPropertyChangedSlot(const QString &)));
}

string CaptionGraphicsItem::usedProperty() {
  return _confPropertySelectionWidget->currentText().toStdString();
}

CaptionGraphicsItem::~CaptionGraphicsItem() {
}

void CaptionGraphicsItem::filterChangedSlot(float begin,float end) {
  emit filterChanged(begin,end);
}

void CaptionGraphicsItem::selectedPropertyChangedSlot(const QString &propertyName){
  emit selectedPropertyChanged(propertyName.toStdString());
}

void CaptionGraphicsItem::configurationIconPressed() {
  _confBackgroundRectItem->setVisible(!_confBackgroundRectItem->isVisible());
  cout << "press" << endl;
}

}