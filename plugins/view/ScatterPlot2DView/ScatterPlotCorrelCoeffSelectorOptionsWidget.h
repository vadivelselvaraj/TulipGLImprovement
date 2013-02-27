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

#ifndef SCATTERPLOTCORRELCOEFFSELECTOROPTIONSWIDGET_H_
#define SCATTERPLOTCORRELCOEFFSELECTOROPTIONSWIDGET_H_

#include <tulip/Color.h>

#include "ui_ScatterPlotCorrelCoeffSelectorOptionsWidget.h"

namespace tlp {

class ScatterPlotCorrelCoeffSelectorOptionsWidget : public QWidget, public Ui::ScatterPlotCorrelCoeffSelectorOptionsWidgetData {

  Q_OBJECT

public :

  ScatterPlotCorrelCoeffSelectorOptionsWidget(QWidget *parent = 0);

  Color getMinusOneColor() const;
  Color getZeroColor() const;
  Color getOneColor() const;

protected :

  void showEvent(QShowEvent * event);

private slots :

  void pressMinusOneColorButton();
  void pressZeroColorButton();
  void pressOneColorButton();

private :

  void setButtonBackgroundColor(QPushButton *button, const Color &color);
  void changeButtonBackgroundColor(QPushButton *button);
  void updateColorScale();
  Color getButtonColor(QPushButton *button) const;

};

}

#endif /* SCATTERPLOTCORRELCOEFFSELECTOROPTIONSWIDGET_H_ */
