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
#ifndef EXPANDABLEGROUPBOX_H
#define EXPANDABLEGROUPBOX_H

#include <QtGui/QGroupBox>
#include <QtCore/QMap>

class ExpandableGroupBox : public QGroupBox {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ expanded WRITE setExpanded)
  bool _expanded;

  Q_PROPERTY(QWidget * widget READ widget WRITE setWidget)
  QWidget *_widget;

public:
  explicit ExpandableGroupBox(const QString &title=QString(),QWidget *parent = 0);
  virtual ~ExpandableGroupBox();
  bool expanded() const {
    return _expanded;
  }

  QWidget *widget() const {
    return _widget;
  }
  QWidget *takeWidget();

signals:

public slots:
  void setExpanded(bool e);
  void setWidget(QWidget *);
};

#endif // EXPANDABLEGROUPBOX_H
