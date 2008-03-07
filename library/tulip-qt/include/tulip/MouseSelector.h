//-*-c++-*-
#ifndef MOUSESELECTION_H
#define MOUSESELECTION_H

#include <tulip/GWInteractor.h>

class QMouseEvent;
class QKeyEvent;

namespace tlp {

class Graph;
/** \addtogroup Mouse_interactor */ 
/*@{*/
class TLP_QT_SCOPE MouseSelector:public GWInteractor
{
private:
  Qt::MouseButton mButton;
  Qt::KeyboardModifier kModifier;
  unsigned int x,y;
  int w,h;
  bool started;
  Graph *graph;
public:
  MouseSelector(Qt::MouseButton button = Qt::LeftButton,
		Qt::KeyboardModifier modifier = Qt::NoModifier);
  ~MouseSelector() {}
  bool draw(GlMainWidget *);
  bool eventFilter(QObject *, QEvent *);
  GWInteractor *clone() { return new MouseSelector(mButton, kModifier); }
};
/*@}*/

}
#endif
