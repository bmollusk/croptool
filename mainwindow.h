#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QPixmap>
#include <QtCore>
#include <QtGui>
#include <QApplication>
#include <QGraphicsScene>
#include <QSpinBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QPushButton>
#include <math.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = nullptr);
   ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *e);
private slots:
   void on_actionImport_triggered();

   void on_slider_sliderMoved(int position);

   void graphicChange(int x = 0, int y = 0);
   void on_actionExport_triggered();

   void on_actionExport_All_triggered();

   void on_pushButton_clicked();

   void on_pushButton_2_clicked();


   void on_actionSave_As_New_Preset_triggered();

   void on_actionLoad_Preset_triggered();

private:
   Ui::MainWindow *ui;
   QStringList currentFiles = {};
   QGraphicsScene *scene;
   QGraphicsRectItem *rect;
   QHash<QString, int> slidervalues;
   QHash<QString, int> spinboxvalues;
   QString initSettingsFile;
   QString defaultFileDirectory;
   QVector<QVector<int>> squares;


   int horifill = 0;
   int vertfill = 0;

   int minw = INT_MAX;
   int minh = INT_MAX;
   bool crop = false;
   double asp = 1.0;

   void loadSettings();
   void saveSettings();
};

#endif // MAINWINDOW_H
