#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef USE_CV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statBar = ui->statusBar;

    // Create GLWidget
    glw = new GLWidget();
    ui->rightVLayout->layout()->addWidget( glw );
    glw->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    glw->setMouseTracking(true);
    glw->setStateBar(statBar);
    glw->update();

    // menu
    connect( ui->loadImage, SIGNAL(triggered()), this, SLOT(openImage()));
}

MainWindow::~MainWindow(){
    delete ui;
}

//*--- protected key events
void MainWindow::keyPressEvent  (QKeyEvent *e){

}
void MainWindow::keyReleaseEvent(QKeyEvent *e){
    switch(e->key()){
    case Qt::Key_R:  // 重設縮放與位移 reset transform
        glw->resetTrans();
        break;
    case Qt::Key_A:
        glw->updateParam(glw->getExpo()+0.01f);
        break;
    case Qt::Key_D:
        glw->updateParam(glw->getExpo()-0.01f);
        break;

    default:
        break;
    }
}

//*--- menu functions
bool MainWindow::openImage(){
    qDebug() << "openImage";
    QString fileName = QFileDialog::getOpenFileName(this, 
        "讀取圖片", "./", "*", 0, QFileDialog::DontUseNativeDialog);
    if (fileName.isEmpty()) return false;

#ifdef WIN32 //convert path
    std::string path(fileName.toLocal8Bit().constData());
#else
    std::string path(fileName.toUtf8().constData());
#endif

#ifdef USE_CV //read image with opencv
    glw->log("讀取進度: %s ...", path.c_str());
    cv::Mat img = cv::imread(path.c_str());
    glw->updateTexture(img);
#endif

    return true;
}
