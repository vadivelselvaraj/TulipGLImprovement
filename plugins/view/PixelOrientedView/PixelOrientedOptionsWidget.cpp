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

#include <QtGui/QColorDialog>

#include "PixelOrientedOptionsWidget.h"

using namespace std;

PixelOrientedOptionsWidget::PixelOrientedOptionsWidget(QWidget *parent) : QWidget(parent),oldValuesInitialized(false) {
	setupUi(this);
	setBackgroundColor(Color(255,255,255));
  connect(backColorButton, SIGNAL(clicked()), this, SLOT(pressBackgroundColorButton()));
  connect(applyButton,SIGNAL(clicked()),this,SLOT(applySettings()));
}

Color PixelOrientedOptionsWidget::getBackgroundColor() const {
	QString buttonStyleSheet = backColorButton->styleSheet();
	QString backgroundColorCodeHex = buttonStyleSheet.mid(buttonStyleSheet.indexOf("#") + 1, 6);
	bool ok;
	return Color(backgroundColorCodeHex.mid(0, 2).toInt(&ok, 16),
			backgroundColorCodeHex.mid(2, 2).toInt(&ok, 16),
			backgroundColorCodeHex.mid(4, 2).toInt(&ok, 16));
}

void PixelOrientedOptionsWidget::setBackgroundColor(const Color &color) {
	QString colorStr;
	QString str;
	str.setNum(color.getR(),16);
	if(str.size()!=2)
		str.insert(0,"0");
	colorStr.append(str);

	str.setNum(color.getG(),16);
	if(str.size()!=2)
		str.insert(0,"0");
	colorStr.append(str);

	str.setNum(color.getB(),16);
	if(str.size()!=2)
		str.insert(0,"0");
	colorStr.append(str);
	backColorButton->setStyleSheet("QPushButton { background-color: #"+colorStr +"}");
}

void PixelOrientedOptionsWidget::pressBackgroundColorButton() {
  QColor newColor = QColorDialog::getColor(backColorButton->palette().color(QPalette::Button));
	if (newColor.isValid()) {
		setBackgroundColor(Color(newColor.red(), newColor.green(), newColor.blue()));
	}
}


string PixelOrientedOptionsWidget::getLayoutType() const {
	return layoutTypeCB->currentText().toStdString();
}

void PixelOrientedOptionsWidget::setLayoutType(const string layoutType) {
	int idx = layoutTypeCB->findText(QString(layoutType.c_str()));
	if (idx != -1) {
		layoutTypeCB->setCurrentIndex(idx);
	}
}

bool PixelOrientedOptionsWidget::configurationChanged() {
  bool confChanged=false;
  if(oldValuesInitialized){
    if(oldBackgroundColor!=getBackgroundColor() ||
       oldLayoutType!=getLayoutType() ){
      confChanged=true;
    }
  }else{
    confChanged=true;
    oldValuesInitialized=true;
  }

  if(confChanged){
    oldBackgroundColor=getBackgroundColor();
    oldLayoutType=getLayoutType();
  }

  return confChanged;
}
