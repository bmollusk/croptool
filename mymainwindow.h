#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>

class MyMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MyMainWindow(QWidget *parent = nullptr);
protected:
    void resizeEvent(QResizeEvent *e);
signals:
    void resizing();
public slots:
};

#endif // MYMAINWINDOW_H
