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
#ifndef _Tulip_SNAPSHOTDIALOG_H
#define _Tulip_SNAPSHOTDIALOG_H

#include <tulip/tulipconf.h>

#include "ui_SnapshotDialog.h"

namespace tlp {

class LockLabel;
class GlMainView;

class TLP_QT_SCOPE SnapshotDialog : public QDialog,public Ui::SnapshotDialogData {

  Q_OBJECT

public :

  SnapshotDialog(GlMainView &v,QWidget *parent=NULL);
  ~SnapshotDialog();

public slots :

  void accept();

protected slots :

  void widthSpinBoxValueChanged(int value);
  void heightSpinBoxValueChanged(int value);

  void fileNameTextChanged(const QString &);
  QString browseClicked();
  void copyClicked();

protected :

  void resizeEvent(QResizeEvent *);

  void sizeSpinBoxValueChanged();

  GlMainView *view;

  QGraphicsScene *scene;
  QGraphicsPixmapItem *pixmapItem;
  LockLabel *lockLabel;

  float ratio;

  bool inSizeSpinBoxValueChanged;
};

}

#endif






