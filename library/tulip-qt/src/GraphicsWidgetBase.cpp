#include "tulip/GraphicsWidgetBase.h"

namespace tlp {

GraphicsWidgetBase::GraphicsWidgetBase() {
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
	setAcceptHoverEvents(true);
#if(QT_VERSION >= QT_VERSION_CHECK(4,5,0))
	setOpacity(0.8);
#endif
}

void GraphicsWidgetBase::wheelEvent(QGraphicsSceneWheelEvent * event) {
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	if (event->modifiers() != Qt::ControlModifier) {
		double sx, sy;
		double scalefactor = 1.1;
		if (numSteps > 0) {
			sx = sy = scalefactor;
		} else {
			sx = sy = 1. / scalefactor;
		}
		scale(sx, sy);
	}
 #if(QT_VERSION >= QT_VERSION_CHECK(4,5,0))
	else {
		double cur_opacity = opacity();
		cur_opacity += numSteps * 0.01;
		if (cur_opacity < 0.2) {
			cur_opacity = 0.2;
		} else if (cur_opacity > 1.0) {
			cur_opacity = 1.0;
		}
		setOpacity(cur_opacity);
	}
#endif
 }

}
