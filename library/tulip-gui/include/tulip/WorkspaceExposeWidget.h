#ifndef WORKSPACEEXPOSEWIDGET_H
#define WORKSPACEEXPOSEWIDGET_H

#include <QtGui/QGraphicsObject>
#include <QtGui/QGraphicsView>
#include <QtCore/QMap>
#include <QtGui/QPixmap>
#include <tulip/tulipconf.h>

class QAbstractAnimation;

namespace tlp {
class WorkspacePanel;

class PreviewItem: public QGraphicsObject {
  Q_OBJECT

  static QPixmap* _closeButtonPixmap;
  static QRect _closePixmapRect;

  QPixmap _pixmap;
  tlp::WorkspacePanel* _panel;
  bool _hovered;
  bool _closeButtonHovered;

  int textHeight() const;
public:
  explicit PreviewItem(const QPixmap& pixmap, tlp::WorkspacePanel* panel, QGraphicsItem* parent = 0);
  QRectF boundingRect() const;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
  tlp::WorkspacePanel* panel() const;
  bool shouldClose(const QPointF&);
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent*);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
  void hoverMoveEvent(QGraphicsSceneHoverEvent*);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
signals:
  void opened();
};


class TLP_QT_SCOPE WorkspaceExposeWidget : public QGraphicsView {
  Q_OBJECT

  QAbstractAnimation* _positionAnimation;
  QList<PreviewItem*> _items;
  PreviewItem* _selectedItem;
  QGraphicsRectItem* _placeholderItem;

  int _currentPanelIndex;

  void updatePositions(bool resetScenePos=true);
  static const int MARGIN;

  void finish();

public:
  static QSize previewSize();

  explicit WorkspaceExposeWidget(QWidget *parent = 0);
  int currentPanelIndex() const;
  QVector<WorkspacePanel*> panels() const;

public slots:
  void setData(const QVector<WorkspacePanel*>& panels, int currentPanelIndex);

signals:
  void exposeFinished();

protected:
  void resizeEvent(QResizeEvent *event);
  bool eventFilter(QObject *, QEvent *);
  bool event(QEvent*);

protected slots:
  void updatePositionsAnimationFinished();
  void resetSceneRect();
  void itemOpened();
};

}

#endif // WORKSPACEEXPOSEWIDGET_H

