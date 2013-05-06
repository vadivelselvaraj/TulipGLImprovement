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

#include <tulip/Glyph.h>
#include <tulip/Iterator.h>

#include <QComboBox>
#include <QPushButton>

#include "GlyphScaleConfigDialog.h"

using namespace std;
using namespace tlp;

GlyphScaleConfigDialog::GlyphScaleConfigDialog(QWidget *parent) : QDialog(parent) {
  setupUi(this);
  tableWidget->setRowCount(5);
  list<string> pluginsList = PluginLister::instance()->availablePlugins<Glyph>();

  for(list<string>::iterator it=pluginsList.begin(); it!=pluginsList.end(); ++it) {
    string glyphName=*it;
    glyphsNameList << QString(glyphName.c_str());
  }

  for (int i = 0 ; i < 5 ; ++i) {
    QComboBox *glyphNameComboBox = new QComboBox(this);
    glyphNameComboBox->addItems(glyphsNameList);
    glyphNameComboBox->setCurrentIndex(i);
    tableWidget->setCellWidget(i, 0, glyphNameComboBox);
  }

  connect(nbGlyphsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(nbGlyphsSpinBoxValueChanged(int)));
}

vector<int> GlyphScaleConfigDialog::getSelectedGlyphsId() const {
  vector<int> ret;

  for (int i = 0 ; i < tableWidget->rowCount() ; ++i) {
    string glyphName = ((QComboBox *)tableWidget->cellWidget(i, 0))->currentText().toStdString();
    ret.push_back(PluginLister::pluginInformations(glyphName).id());
  }

  reverse(ret.begin(), ret.end());
  return ret;
}



void GlyphScaleConfigDialog::nbGlyphsSpinBoxValueChanged(int value) {
  if (value > glyphsNameList.size()) {
    nbGlyphsSpinBox->setValue(value-1);
  }

  int lastCount = tableWidget->rowCount();
  tableWidget->setRowCount(value);

  if (lastCount < value) {
    QComboBox *glyphNameComboBox = new QComboBox(this);
    glyphNameComboBox->addItems(glyphsNameList);
    tableWidget->setCellWidget(value-1, 0, glyphNameComboBox);
  }
}

void GlyphScaleConfigDialog::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  tableWidget->setColumnWidth(0, tableWidget->width() - 23);
}
