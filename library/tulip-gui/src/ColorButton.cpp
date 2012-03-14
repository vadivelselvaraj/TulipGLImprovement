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
#include "tulip/ColorButton.h"

#include <QtGui/QColorDialog>
#include <QtGui/QMainWindow>
#include <QtGui/QStylePainter>
#include <tulip/TlpQtTools.h>
#include <tulip/Perspective.h>

using namespace tlp;

ColorButton::ColorButton(QWidget *parent): QPushButton(parent), _color(Qt::black), _dialogParent(this) {
  connect(this,SIGNAL(clicked()),this,SLOT(chooseColor()));
}

QColor ColorButton::color() const {
  return _color;
}

void ColorButton::setDialogParent(QWidget *w) {
  _dialogParent = w;
}

tlp::Color ColorButton::tulipColor() const {
  return QColorToColor(_color);
}

void ColorButton::paintEvent(QPaintEvent *event) {
  QPushButton::paintEvent(event);
  QStylePainter p(this);
  float tickW = width() / 4.;
  float tickH = height() / 4;
  p.setPen(Qt::black);
  p.setBrush(_color);
  p.drawRect(tickW, tickH, tickW * 2, tickH * 2);
}

void ColorButton::setColor(const QColor& c) {
  _color = c;
  repaint();
  emit colorChanged(_color);
  emit tulipColorChanged(QColorToColor(_color));
}

void ColorButton::setTulipColor(const tlp::Color& c) {
  setColor(colorToQColor(c));
}

void ColorButton::chooseColor() {
  QColor c = QColorDialog::getColor(_color,_dialogParent,trUtf8("Choose a color"),QColorDialog::ShowAlphaChannel);

  if (c.isValid())
    setColor(c);
}
