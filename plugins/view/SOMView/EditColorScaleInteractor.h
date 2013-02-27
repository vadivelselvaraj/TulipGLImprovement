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

#ifndef EDITCOLORSCALEINTERACTOR_H_
#define EDITCOLORSCALEINTERACTOR_H_

#include <tulip/GLInteractor.h>
#include <tulip/GlColorScale.h>
#include <tulip/DoubleProperty.h>
#include "SOMView.h"
#include "GlLabelledColorScale.h"
namespace tlp {
class GlLayer;
}

class EditColorScaleInteractor: public tlp::GLInteractorComponent {
public:

  EditColorScaleInteractor();
  virtual ~EditColorScaleInteractor();
  bool eventFilter(QObject *, QEvent *);
  void viewChanged(View *view);
  bool compute(GlMainWidget *);
  bool draw(tlp::GlMainWidget *glMainWidget);
  InteractorComponent *clone() {
    return new EditColorScaleInteractor();
  }

protected:
  virtual void propertyChanged(SOMView* somView,const std::string& propertyName, tlp::DoubleProperty *newProperty);
  virtual void screenSizeChanged(SOMView* somView);
  tlp::DoubleProperty *currentProperty;
  GlLabelledColorScale *colorScale;
  float widthPercent;
  float heightPercent;
  float heightPosition;
  int glMainWidgetWidth;
  int glMainWidgetHeight;

  tlp::GlLayer *selectionLayer;
};

#endif /* EDITCOLORSCALEINTERACTOR_H_ */
