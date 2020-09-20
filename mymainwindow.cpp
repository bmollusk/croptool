#include "mymainwindow.h"
#include <QDebug>
#include <QMainWindow>
MyMainWindow::MyMainWindow(QWidget *parent) : QMainWindow(parent)
{
    qDebug() << "shiiit jit";
}
void MyMainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    emit resizing();
}
