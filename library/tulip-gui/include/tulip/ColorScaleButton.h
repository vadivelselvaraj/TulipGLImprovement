/*
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
///@cond DOXYGEN_HIDDEN

#ifndef COLORSCALEBUTTON_H
#define COLORSCALEBUTTON_H

#include <QPushButton>


#include <tulip/ColorScale.h>

namespace tlp {

class ColorScaleConfigDialog;

class TLP_QT_SCOPE ColorScaleButton: public QPushButton {
  Q_OBJECT
  ColorScale _colorScale;
public:
  static void paintScale(QPainter*,const QRect&, const ColorScale&);

  ColorScaleButton(ColorScale colorScale=ColorScale(), QWidget* parent=NULL);

  const ColorScale& colorScale() const;
  void setColorScale(const ColorScale&) const;
  void paintEvent(QPaintEvent *event);
  void editColorScale(const ColorScale& colorScale);

public slots:
  void editColorScale();
};

}
#endif // COLORSCALEBUTTON_H
///@endcond
