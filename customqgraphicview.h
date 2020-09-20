#ifndef CUSTOMGRAPHICVIEW_H
#define CUSTOMGRAPHICVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>

class CustomQGraphicsView : public QGraphicsView
{
   Q_OBJECT
public:
   explicit CustomQGraphicsView(QWidget *parent = 0);

signals:

public slots:
   void mousePressEvent(QMouseEvent * e);
   // void mouseReleaseEvent(QMouseEvent * e);
   // void mouseDoubleClickEvent(QMouseEvent * e);
   // void mouseMoveEvent(QMouseEvent * e);
private:
   QGraphicsScene * scene;
};
#endif // CUSTOMGRAPHICVIEW_H
