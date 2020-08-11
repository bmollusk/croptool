#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CustomQGraphicsScene.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //qInfo()<<QApplication::applicationDirPath();
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings("bbqmollusc","keyframer");
    defaultFileDirectory=settings.value("defaultFileDirectory","").toString();
}

void MainWindow::saveSettings()
{
    QSettings settings("bbqmollusc","keyframer");
    settings.setValue("defaultFileDirectory",defaultFileDirectory);
    qInfo()<<"settings saved "<<settings.fileName();
}

void MainWindow::on_actionImport_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Open Pages",defaultFileDirectory, tr("PNG (*.png);;JPEG (*.jpg)"));

    if(filenames.length()==0){
        return;
    }

    qInfo()<<filenames;
    defaultFileDirectory=filenames[0].left(filenames[0].lastIndexOf("/")+1);
    qInfo()<<defaultFileDirectory;
    saveSettings();
    bool hadshit = currentFiles.length()>0;
    QStringList choices;
    for (int i = 0;i<filenames.length();++i) {
        QFile file(filenames[i]);
        if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
                QMessageBox::warning(this, "Warning", "Cannot open file: " + filenames[i] + file.errorString());
                return;
            }
        QPixmap pic(filenames[i]);
        currentFiles.append(filenames[i]);
    }
    for(int i = 0;i<currentFiles.length();++i){
        QPixmap pic(currentFiles[i]);
        minw = pic.width();
        minh = pic.height();
        double aspect = (double)minw/(double)minh;
        QString aspectstring = QString::number(aspect);
        QString choice = currentFiles[i]+" "+aspectstring;//might not be precise with ints
        choices.append(choice);
    }

    for(int i = 0;i<currentFiles.length();++i){
        //QMessageBox::StandardButton reply;
        QPixmap pic(currentFiles[i]);
        if(fabs(pic.width()/pic.height()-(double)minw/(double)minh)>0.1){
            QMessageBox::StandardButton reply = QMessageBox::question(this,"Warning", "All Pages are not same aspect ratio, would you like to crop all to same aspect ratio?",QMessageBox::Yes|QMessageBox::No);
            crop = reply==QMessageBox::Yes;
            break;
        }
    }

    asp=1.0;
    if(crop){
        QString Item = QInputDialog::getItem(this, "Choose Aspect Ratio", "Pick an aspect ratio from the list based on your chosen images, image name first and aspect ratio second",choices,0);
        QStringList splitted = Item.split(" ");
        asp = splitted[1].toDouble();
        qInfo()<<asp;
    }
    QString currentFile = currentFiles[0];
    QFile file(currentFile);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open file: " + currentFile + file.errorString());
            return;
        }
    QPixmap pic(currentFile);
    if(crop){
        if(asp<1){
            pic = pic.copy(0,0,pic.height()*asp, pic.height());
        }else{
            pic = pic.copy(0,0,pic.width(),pic.width()/asp);
        }
    }
    ui->slider->setEnabled(true);
    ui->slider->setMaximum(currentFiles.size()-1);
    int h=ui->label->height();
    int w=ui->label->width();
    if(pic.height()>pic.width()){
        pic=pic.scaledToHeight(h);
    }else{
        pic=pic.scaledToWidth(w);
    }

    ui->label->setPixmap(pic);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->actionExport->setEnabled(true);
    ui->actionExport_All->setEnabled(true);
    ui->actionLoad_Preset->setEnabled(true);
    ui->actionSave_As_New_Preset->setEnabled(true);

    QSlider *numSliders[8];
    QSpinBox *numSpinBoxes[8];
    for (int i = 0;i<8;i++) {
        QString sliderName = "horizontalSlider_"+QString::number(i);
        QString spinBoxName = "spinBox_"+QString::number(i);
        numSliders[i] = MainWindow::findChild<QSlider *>(sliderName);
        numSliders[i]->setEnabled(true);
        numSpinBoxes[i]=MainWindow::findChild<QSpinBox *>(spinBoxName);
        numSpinBoxes[i]->setEnabled(true);
        connect(numSliders[i], SIGNAL(sliderMoved(int)),this,SLOT(graphicChange()));
        connect(numSpinBoxes[i], SIGNAL(editingFinished()),this,SLOT(graphicChange()));
        int value=numSliders[i]->sliderPosition();
        slidervalues.insert(sliderName, value);
        spinboxvalues.insert(spinBoxName,value);
    }

    //make the safety optional, and add setting to remove
    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();

    /*
    horifill = slidercolumns*(width)+(slidercolumns-1)*horispacing;
    vertfill = sliderrows*heigh+(sliderrows-1)*vertspacing;

    int horileft = pic.width()-horifill;
    int vertleft = pic.height()-vertfill;
    if(horileft<0||vertleft<0){
        QMessageBox::warning(this, "Warning", "File too small");//TODO figure out min size
        return;
    }
    */

    int maxcolumns=(int)(pic.width()-offsetx)/(horispacing+width);//math is inaccurate for all, is probably a bit more complex
    ui->horizontalSlider_5->setMaximum(maxcolumns);
    ui->spinBox_5->setMaximum(maxcolumns);
    int maxrows=(int)(pic.height()-offsety)/(vertspacing+heigh);
    ui->horizontalSlider_4->setMaximum(maxrows);
    ui->spinBox_4->setMaximum(maxrows);
    int maxhori = (int)(pic.width()-slidercolumns*width-offsetx)/slidercolumns;
    ui->horizontalSlider_3->setMaximum(maxhori);
    ui->spinBox_3->setMaximum(maxhori);
    int maxvert = (int)(pic.height()-sliderrows*heigh-offsety)/sliderrows;
    ui->horizontalSlider_2->setMaximum(maxvert);
    ui->spinBox_2->setMaximum(maxvert);
    int maxwidth = (int)(pic.width()-(slidercolumns-1)*horispacing-offsetx)/slidercolumns;
    ui->horizontalSlider_1->setMaximum(maxwidth);
    ui->spinBox_1->setMaximum(maxwidth);
    int maxheigh = (int)(pic.height()-(sliderrows-1)*vertspacing-offsety)/sliderrows;
    ui->horizontalSlider_0->setMaximum(maxheigh);
    ui->spinBox_0->setMaximum(maxheigh);
    int maxoffsetx = (int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width);
    //qInfo()<<(int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width)/slidercolumns;
    ui->horizontalSlider_7->setMaximum(maxoffsetx);
    ui->spinBox_7->setMaximum(maxoffsetx);
    int maxoffsety = (int)(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh);
    //qInfo()<<(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh)/sliderrows;
    ui->horizontalSlider_6->setMaximum(maxoffsety);
    ui->spinBox_6->setMaximum(maxoffsety);

    scene = new CustomQGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    connect(scene,SIGNAL(sendCoord(int,int)),this,SLOT(graphicChange(int,int)));
    QBrush trans(Qt::transparent);
    QPen pen(Qt::black);
    pen.setWidth(2);
    if(!hadshit){
        rect = scene->addRect(0, 0, 10 , 10, pen, trans);
    }else{
        for (int i=0;i<sliderrows;i++) {
            for (int j=0;j<slidercolumns;j++) {
                int xpos = offsetx+(width+horispacing)*j;
                int ypos = offsety+(heigh+vertspacing)*i;
                if(squares[i][j]==1){
                   qInfo()<<"continue";
                }else{
                    rect = scene->addRect(xpos, ypos, width, heigh, pen, trans);
                }
            }
        }
    }

    QVector<int> start;
    start.append(0);
    squares.append(start);
    file.close();
}

void MainWindow::on_slider_sliderMoved(int position)
{
    QString currentFile = currentFiles[position];
    QFile file(currentFile);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open file: " + currentFile + file.errorString());
            return;
        }

    QPixmap pic(currentFile);
    if(crop){
        if(asp<1){
            pic = pic.copy(0,0,pic.height()*asp, pic.height());
        }else{
            pic = pic.copy(0,0,pic.width(),pic.width()/asp);
        }
    }
    int h=ui->label->height();
    int w=ui->label->width();
    if(pic.height()>pic.width()){
        pic=pic.scaledToHeight(h);
    }else{
        pic=pic.scaledToWidth(w);
    }
    ui->label->setPixmap(pic);

    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();



    int maxcolumns=(int)(pic.width()-offsetx)/(horispacing+width);//math is inaccurate for all, is probably a bit more complex
    ui->horizontalSlider_5->setMaximum(maxcolumns);
    ui->spinBox_5->setMaximum(maxcolumns);
    int maxrows=(int)(pic.height()-offsety)/(vertspacing+heigh);
    ui->horizontalSlider_4->setMaximum(maxrows);
    ui->spinBox_4->setMaximum(maxrows);
    int maxhori = (int)(pic.width()-slidercolumns*width-offsetx)/slidercolumns;
    ui->horizontalSlider_3->setMaximum(maxhori);
    ui->spinBox_3->setMaximum(maxhori);
    int maxvert = (int)(pic.height()-sliderrows*heigh-offsety)/sliderrows;
    ui->horizontalSlider_2->setMaximum(maxvert);
    ui->spinBox_2->setMaximum(maxvert);
    int maxwidth = (int)(pic.width()-(slidercolumns-1)*horispacing-offsetx)/slidercolumns;
    ui->horizontalSlider_1->setMaximum(maxwidth);
    ui->spinBox_1->setMaximum(maxwidth);
    int maxheigh = (int)(pic.height()-(sliderrows-1)*vertspacing-offsety)/sliderrows;
    ui->horizontalSlider_0->setMaximum(maxheigh);
    ui->spinBox_0->setMaximum(maxheigh);
    int maxoffsetx = (int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width);
    //qInfo()<<(int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width)/slidercolumns;
    ui->horizontalSlider_7->setMaximum(maxoffsetx);
    ui->spinBox_7->setMaximum(maxoffsetx);
    int maxoffsety = (int)(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh);
    //qInfo()<<(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh)/sliderrows;
    ui->horizontalSlider_6->setMaximum(maxoffsety);
    ui->spinBox_6->setMaximum(maxoffsety);
    file.close();
}

void MainWindow::graphicChange(int x,int y){
    qInfo()<<x<<" "<<y;
    qInfo()<<"triggered";
    QPixmap pic = ui->label->pixmap()->copy();
    QSlider *slider = qobject_cast<QSlider *>(QObject::sender());
    CustomQGraphicsScene *scened = qobject_cast<CustomQGraphicsScene *>(QObject::sender());
    if(!scened){
        if(!slider){
            QSpinBox *spinbox = (QSpinBox *)sender();
            QString name = spinbox->objectName();
            int value = spinbox->value();
            spinboxvalues.insert(name,value);
            QString othername = "horizontalSlider_"+name.right(1);
            if(othername=="horizontalSlider_2")
                qInfo()<<"why tf";
            qInfo()<<othername;
            slidervalues.insert(othername,value);
            QSlider *otherslider=MainWindow::findChild<QSlider *>(othername);
            otherslider->setValue(value);
        }else{
            QString name = slider->objectName();
            int value = slider->sliderPosition();
            slidervalues.insert(name, value);
            QString othername = "spinBox_"+name.right(1);
            qInfo()<<othername;
            spinboxvalues.insert(othername,value);
            QSpinBox *otherspinbox=MainWindow::findChild<QSpinBox *>(othername);
            otherspinbox->setValue(value);
        }
    }
    scene = new CustomQGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    connect(scene,SIGNAL(sendCoord(int,int)),this,SLOT(graphicChange(int,int)));

    QBrush trans(Qt::transparent);
    QPen pen(Qt::black);
    pen.setWidth(2);

    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();

    int maxcolumns=(int)(pic.width()-offsetx)/(horispacing+width);//math is inaccurate for all, is probably a bit more complex
    ui->horizontalSlider_5->setMaximum(maxcolumns);
    ui->spinBox_5->setMaximum(maxcolumns);
    int maxrows=(int)(pic.height()-offsety)/(vertspacing+heigh);
    ui->horizontalSlider_4->setMaximum(maxrows);
    ui->spinBox_4->setMaximum(maxrows);
    int maxhori = (int)(pic.width()-slidercolumns*width-offsetx)/slidercolumns;
    ui->horizontalSlider_3->setMaximum(maxhori);
    ui->spinBox_3->setMaximum(maxhori);
    int maxvert = (int)(pic.height()-sliderrows*heigh-offsety)/sliderrows;
    ui->horizontalSlider_2->setMaximum(maxvert);
    ui->spinBox_2->setMaximum(maxvert);
    int maxwidth = (int)(pic.width()-(slidercolumns-1)*horispacing-offsetx)/slidercolumns;
    ui->horizontalSlider_1->setMaximum(maxwidth);
    ui->spinBox_1->setMaximum(maxwidth);
    int maxheigh = (int)(pic.height()-(sliderrows-1)*vertspacing-offsety)/sliderrows;
    ui->horizontalSlider_0->setMaximum(maxheigh);
    ui->spinBox_0->setMaximum(maxheigh);
    int maxoffsetx = (int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width);
    //qInfo()<<(int)(pic.width()-(slidercolumns-1)*horispacing-slidercolumns*width)/slidercolumns;
    ui->horizontalSlider_7->setMaximum(maxoffsetx);
    ui->spinBox_7->setMaximum(maxoffsetx);
    int maxoffsety = (int)(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh);
    //qInfo()<<(pic.height()-(sliderrows-1)*vertspacing-sliderrows*heigh)/sliderrows;
    ui->horizontalSlider_6->setMaximum(maxoffsety);
    ui->spinBox_6->setMaximum(maxoffsety);
//    for(int i = 0; i<sliderrows;i++){
//        //increase and decrease size
//        QVector<int> temp = QVector<int>();
//        for(int j=0;j<slidercolumns;j++){
//            if(j>squares[0].size()-1){
//                squares[i].append(0);
//            }
//            temp.append(0);
//        }
//        if(i>squares.size()-1){
//            squares.append(temp);
//        }
//    }
    QVector<int> temp = QVector<int>();
    for(int i=0; i<sliderrows;i++){
        if(i<=squares.size()-1){
            for(int j=0;j<slidercolumns;j++){
                if(j>squares[i].size()-1){
                    squares[i].append(0);
                }
                if(i==0){
                    temp.append(0);
                }

            }
        }else{
            squares.append(temp);
        }
    }
    qInfo()<<squares;
    //ui->graphicsView->sceneRect().moveTo(offsetx, offsety);
    for (int i=0;i<sliderrows;i++) {
        for (int j=0;j<slidercolumns;j++) {
            int xpos = offsetx+(width+horispacing)*j;
            int ypos = offsety+(heigh+vertspacing)*i;
            //qInfo()<<squares;
            if(x>xpos & y>ypos & x<xpos+width & y<ypos+heigh & x!=0 & y!=0 & squares[i][j]==0){
                squares[i][j]=1;

            }else if(x>xpos & y>ypos & x<xpos+width & y<ypos+heigh & x!=0 & y!=0 & squares[i][j]==1){
               squares[i][j]=0;
               rect = scene->addRect(xpos, ypos, width, heigh, pen, trans);
            }else if(squares[i][j]==1){
               qInfo()<<"continue";
            }else{
                rect = scene->addRect(xpos, ypos, width, heigh, pen, trans);
            }
        }
    }
}



void MainWindow::on_actionExport_triggered()
{
    boolean pix=ui->label->pixmap() == 0;
    if(pix){
        QMessageBox::warning(this, "Warning", "Cannot export without image, File->Import");
        return;
    }
    QString filenamebase = QFileDialog::getSaveFileName(this, "Export", defaultFileDirectory+"0", tr("PNG (*.png);;JPEG (*.jpg)"));
    if(filenamebase.isNull()){
        return;
    }
    int index=filenamebase.lastIndexOf(".");

    QRegularExpression re("0+");
    QRegularExpressionMatchIterator i = re.globalMatch(filenamebase);
    QRegularExpressionMatch mat;
    while(i.hasNext()){
        mat = i.next();
        QString tem = mat.captured(0);
        qInfo()<<tem;
    }
    QString numbers = mat.captured(0);
    int starting = mat.capturedStart(0);

    QString filename = filenamebase.left(index).left(starting);
    QString fileextension = filenamebase.right(filenamebase.size()-index);



    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();
    //ui->graphicsView->sceneRect().moveTo(offsetx, offsety);
    int curr = 0;
    int f=ui->slider->value();
    QString currentPic = currentFiles[f];
    QFile tempfile(currentPic);
    if (!tempfile.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open file: " + currentPic + tempfile.errorString());
            return;
        }

    QPixmap pic(currentPic);
    if(crop){
        pic = pic.copy(0,0,minw, minh);
    }
    int wscale = pic.width()/ui->label->pixmap()->width();
    int hscale = pic.height()/ui->label->pixmap()->height();
    horispacing *= wscale;
    width *= wscale;
    offsetx *= wscale;
    vertspacing *= hscale;
    heigh *= hscale;
    offsety *= hscale;
    for (int i=0;i<sliderrows;i++) {
        for (int j=0;j<slidercolumns;j++) {
            if(squares[i][j]==1){
                qInfo()<<"continue";
            }else{
                QString ind = QString("%1").arg(curr, numbers.length(),10,QLatin1Char('0'));
                QString currentFile = filename+ind+fileextension;
                QFile file(currentFile);
                if (!file.open(QIODevice::WriteOnly | QFile::Text)) {//figure out if text part matters
                        QMessageBox::warning(this, "Warning", "Cannot open file: " + currentFile + file.errorString());
                        return;
                    }

                QRect rect(offsetx+(width+horispacing)*j, offsety+(heigh+vertspacing)*i, width, heigh);
                QPixmap cropped = pic.copy(rect);
                cropped.save(currentFile);
                file.close();
                curr++;
            }

        }
    }
}

void MainWindow::on_actionExport_All_triggered()
{
    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();
    //ui->graphicsView->sceneRect().moveTo(offsetx, offsety);
    boolean pix=ui->label->pixmap() == 0;
    if(pix){
        QMessageBox::warning(this, "Warning", "Cannot export without image, File->Import");
        return;
    }
    QString filenamebase = QFileDialog::getSaveFileName(this, "Export", defaultFileDirectory+"0", tr("PNG (*.png);;JPEG (*.jpg)"));
    if(filenamebase.isNull()){
        return;//TODO Create error prompt for this
    }
    int index=filenamebase.lastIndexOf(".");

    QRegularExpression re("0+");
    QRegularExpressionMatchIterator i = re.globalMatch(filenamebase);
    QRegularExpressionMatch mat;
    while(i.hasNext()){
        mat = i.next();
        QString tem = mat.captured(0);
        qInfo()<<tem;
    }
    QString numbers = mat.captured(0);
    int starting = mat.capturedStart(0);

    QString filename = filenamebase.left(index).left(starting);
    QString fileextension = filenamebase.right(filenamebase.size()-index);
    int curr = 0;
    for(int f=0;f<currentFiles.size();f++){
        QString currentPic = currentFiles[f];
        QFile tempfile(currentPic);
        if (!tempfile.open(QIODevice::ReadOnly | QFile::Text)) {
                QMessageBox::warning(this, "Warning", "Cannot open file: " + currentPic + tempfile.errorString());
                return;
            }

        QPixmap pic(currentPic);
        if(crop){
            pic = pic.copy(0,0,minw, minh);
        }
        int wscale = pic.width()/ui->label->pixmap()->width();
        int hscale = pic.height()/ui->label->pixmap()->height();
        int horispacings=horispacing * wscale;
        int widths = width * wscale;
        int offsetxs = offsetx * wscale;
        int vertspacings = vertspacing * hscale;
        int heighs = heigh * hscale;
        int offsetys = offsety * hscale;
//        int h=ui->label->height();
//        int w=ui->label->width();
//        if(pic.height()>pic.width()){
//            pic=pic.scaledToHeight(h);
//        }else{
//            pic=pic.scaledToWidth(w);
//        }
        for (int i=0;i<sliderrows;i++) {
            for (int j=0;j<slidercolumns;j++) {
                if(squares[i][j]==1){
                    qInfo()<<"continue";
                }else{
                    QString ind = QString("%1").arg(curr, numbers.length(),10,QLatin1Char('0'));
                    QString currentFile = filename+ind+fileextension;
                    QFile file(currentFile);
                    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {//figure out if text part matters
                            QMessageBox::warning(this, "Warning", "Cannot open file: " + currentFile + file.errorString());
                            return;
                        }

                    QRect rect(offsetxs+(widths+horispacings)*j, offsetys+(heighs+vertspacings)*i, widths, heighs);
                    QPixmap cropped = pic.copy(rect);
                    cropped.save(currentFile);
                    file.close();
                    curr++;
                }
            }
        }
        tempfile.close();
   }
}

void MainWindow::on_pushButton_clicked()
{
    boolean pix=ui->label->pixmap() == 0;
    if(pix){
        QMessageBox::warning(this, "Warning", "You haven't imported an image yet, File->Import");
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this,"Warning", "Are you sure you want to delete this frame?",QMessageBox::Yes|QMessageBox::No);
    if(reply!=QMessageBox::Yes){
        return;
    }
    int currentIndex = ui->slider->sliderPosition();
    QString currentFile;
    if(currentFiles.size()==1){
        //lead to other shit
    }else if(currentIndex<currentFiles.size()-1){
        currentFile = currentFiles[currentIndex+1];
    }else{
        currentFile = currentFiles[currentIndex-1];
    }
    QFile file(currentFile);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open file: " + currentFile + file.errorString());
            return;
        }

    QPixmap pic(currentFile);
    if(crop){
        pic = pic.copy(0,0,minw, minh);
    }
    int h=ui->label->height();
    int w=ui->label->width();
    if(pic.height()>pic.width()){
        pic=pic.scaledToHeight(h);
    }else{
        pic=pic.scaledToWidth(w);
    }
    ui->label->setPixmap(pic);

    file.close();
    currentFiles.removeAt(currentIndex);
    ui->slider->setMaximum(currentFiles.size()-1);

}

void MainWindow::on_pushButton_2_clicked()
{
    boolean pix=ui->label->pixmap() == 0;
    if(pix){
        QMessageBox::warning(this, "Warning", "You haven't imported an image yet, File->Import");
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this,"Warning", "Are you sure you want to delete ALL frames?",QMessageBox::Yes|QMessageBox::No);
    if(reply!=QMessageBox::Yes){
        return;
    }
    ui->slider->setMaximum(0);
    ui->label->clear();
    ui->label->setText("File->Import");
    ui->graphicsView->scene()->clear();
    currentFiles.clear();

}

void MainWindow::on_actionSave_As_New_Preset_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save config file",defaultFileDirectory, tr("INIT (*.ini)"));
    QFile file(filename);
    if(filename.isNull()){
        return;
    }
    int sliderrows = slidervalues.find("horizontalSlider_4").value();
    int slidercolumns = slidervalues.find("horizontalSlider_5").value();
    int horispacing = slidervalues.find("horizontalSlider_3").value();
    int vertspacing = slidervalues.find("horizontalSlider_2").value();
    int width = slidervalues.find("horizontalSlider_1").value();
    int heigh = slidervalues.find("horizontalSlider_0").value();
    int offsetx = slidervalues.find("horizontalSlider_7").value();
    int offsety = slidervalues.find("horizontalSlider_6").value();

    QSettings* settings = new QSettings(filename, QSettings::IniFormat);
    settings->setValue("sliderrows",sliderrows);
    settings->setValue("slidercolumns",slidercolumns);
    settings->setValue("horispacing",horispacing);
    settings->setValue("vertspacing",vertspacing);
    settings->setValue("width",width);
    settings->setValue("heigh",heigh);
    settings->setValue("offsetx",offsetx);
    settings->setValue("offsety",offsety);
    settings->sync();
}


void MainWindow::on_actionLoad_Preset_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open config file",defaultFileDirectory, tr("INIT (*.ini)"));
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open file: " + filename + file.errorString());
            return;
        }
    QSettings* settings = new QSettings(filename, QSettings::IniFormat);
    int horizontal_4 = settings->value("sliderrows",1).toInt();
    qInfo()<<horizontal_4;
    slidervalues.insert("horizontalSlider_4",horizontal_4);
    ui->horizontalSlider_4->setValue(horizontal_4);
    ui->spinBox_4->setValue(horizontal_4);
    int horizontal_5 = settings->value("slidercolumns",1).toInt();
    qInfo()<<horizontal_5;
    slidervalues.insert("horizontalSlider_5",horizontal_5);
    ui->horizontalSlider_5->setValue(horizontal_5);
    ui->spinBox_5->setValue(horizontal_5);
    int horizontal_3 = settings->value("horispacing",0).toInt();
    qInfo()<<horizontal_3;
    slidervalues.insert("horizontalSlider_3",horizontal_3);
    ui->horizontalSlider_3->setValue(horizontal_3);
    ui->spinBox_3->setValue(horizontal_3);
    int horizontal_2 = settings->value("vertspacing",0).toInt();
    qInfo()<<horizontal_2;
    slidervalues.insert("horizontalSlider_2",horizontal_2);
    ui->horizontalSlider_2->setValue(horizontal_2);
    ui->spinBox_2->setValue(horizontal_2);
    int horizontal_1 = settings->value("width",0).toInt();
    qInfo()<<horizontal_1;
    slidervalues.insert("horizontalSlider_1",horizontal_4);
    ui->horizontalSlider_1->setValue(horizontal_1);
    ui->spinBox_1->setValue(horizontal_1);
    int horizontal_0 = settings->value("heigh",0).toInt();
    qInfo()<<horizontal_0;
    slidervalues.insert("horizontalSlider_0",horizontal_4);
    ui->horizontalSlider_0->setValue(horizontal_0);
    ui->spinBox_0->setValue(horizontal_0);
    int horizontal_7 = settings->value("offsetx",0).toInt();
    qInfo()<<horizontal_7;
    slidervalues.insert("horizontalSlider_7",horizontal_7);
    ui->horizontalSlider_7->setValue(horizontal_7);
    ui->spinBox_7->setValue(horizontal_7);
    int horizontal_6 = settings->value("offsety",0).toInt();
    qInfo()<<horizontal_6;
    slidervalues.insert("horizontalSlider_6",horizontal_6);
    ui->horizontalSlider_6->setValue(horizontal_6);
    ui->spinBox_6->setValue(horizontal_6);
}
void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    ui->label->setGeometry(ui->graphicsView->geometry());
//    if(ui->slider->isEnabled()){
//        qInfo()<<"lmao";
//        if(ui->label->pixmap()->height()>ui->label->pixmap()->width()){
//            ui->label->pixmap()->scaledToHeight(ui->label->pixmap()->height());
//        }else{
//            ui->label->pixmap()->scaledToWidth(ui->label->pixmap()->width());
//        }
//        ui->label->pixmap();
//    }

}
