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
///@cond DOXYGEN_HIDDEN


#ifndef TULIPFONTDIALOG_H
#define TULIPFONTDIALOG_H

#include <QtGui/QDialog>
#include <tulip/tulipconf.h>
#include <tulip/TulipFont.h>

namespace Ui {
class TulipFontDialog;
}

namespace tlp {

class TLP_QT_SCOPE TulipFontDialog : public QDialog {
  Q_OBJECT

  Ui::TulipFontDialog* _ui;

public:
  explicit TulipFontDialog(QWidget *parent = NULL);
  TulipFont font() const;
  int fontSize() const;

  static TulipFont getFont(QWidget* parent = NULL,const TulipFont& selectedFont=TulipFont());

public slots:
  void selectFont(const TulipFont&);

protected slots:
  void fontChanged();
};

}

#endif // TULIPFONTDIALOG_H
///@endcond