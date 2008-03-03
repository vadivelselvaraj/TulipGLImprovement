//-*-c++-*-
#ifndef MOUSEMOVESELECTION_H
#define MOUSEMOVESELECTION_H

//#include <tulip/GlRect.h>
#include <tulip/GlCircle.h>
#include <tulip/GlRect.h>
#include <tulip/GlComposite.h>
#include <tulip/Observable.h>
#include <tulip/GWInteractor.h>

/** \addtogroup Mouse_interactor */ 
/*@{*/

namespace tlp {

  /// This interactor allows to move/rotate/stretch the current selection layout
  class TLP_QT_SCOPE MouseSelectionEditor:public GWInteractor {

  public:

    MouseSelectionEditor();
    ~MouseSelectionEditor() {if(glGraphWidget)glGraphWidget->getScene()->getSelectionLayer()->deleteGlEntity("selectionComposite");}
    bool compute(GlGraphWidget *glGraphWidget);
    bool draw(GlGraphWidget *);
    bool eventFilter(QObject *, QEvent *);
    GWInteractor *clone() { return new MouseSelectionEditor(); }

  private:
    enum EditOperation { NONE=0, ROTATE_Z, ROTATE_XY, STRETCH_X, STRETCH_Y, STRETCH_XY, TRANSLATE};
    enum OperationTarget { COORD = 0, SIZE, COORD_AND_SIZE};

    Graph *_graph;
    GlGraphWidget *glGraphWidget;
    LayoutProperty *_layout;
    BooleanProperty *_selection;
    DoubleProperty *_rotation;
    SizeProperty *_sizes;
    LayoutProperty *_copyLayout;
    SizeProperty *_copySizes;
    DoubleProperty *_copyRotation;

    void initProxies(GlGraphWidget *glGraphWidget);
    void saveInfo();
    void restoreInfo();
    void initEdition();
    void undoEdition();
    void stopEdition();

    EditOperation operation;
    OperationTarget mode;

    Coord editCenter;
    Coord editPosition;
    Coord editLayoutCenter;

    //    int x,y;
    //    double centerX, centerY;
    Coord ffdCenter; 

    GlComposite composite;
    GlCircle _controls[8];
    GlRect centerRect;
    Coord _layoutCenter;

    bool computeFFD(GlGraphWidget*);
    void mMouseTranslate(double, double, GlGraphWidget*);
    void mMouseRotate(double, double, GlGraphWidget*);
    void mMouseStretchAxis(double, double, GlGraphWidget*); 


  };

}
/*@}*/

#endif
