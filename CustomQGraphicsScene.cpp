#include "CustomQGraphicsScene.h"
#include "mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

CustomQGraphicsScene::CustomQGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
    qDebug() << "shiii jit";
    xc=0;
    yc=0;

}

void CustomQGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug() << e->scenePos().x() << " " << e->scenePos().y();
    emit sendCoord(e->scenePos().x(),e->scenePos().y());
    this->update();
}
