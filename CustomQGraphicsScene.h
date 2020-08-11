#ifndef CUSTOMQGRAPHICSSCENE_H
#define CUSTOMQGRAPHICSSCENE_H
#include <QGraphicsScene>
#include <QMouseEvent>

class CustomQGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
     CustomQGraphicsScene(QObject *parent);

protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *e);
public:
signals:
    void sendCoord(int,int);
public:
    int xc;
    int yc;
};
#endif // CUSTOMQGRAPHICSSCENE_H
