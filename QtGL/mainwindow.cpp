#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef USE_CV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), img(NULL), clmt()
  #ifdef USE_CV
    , cvImg(2, 2, CV_8UC4)
  #endif
{
    ui->setupUi(this);
    statBar = ui->statusBar;

    printf("setup\n"); fflush(stdout);
    // image
    img = new unsigned char[6*4];
    img[0 ]= 255; img[1 ]= 0; img[2 ]= 0; img[3 ]= 0; img[4 ]= 128; img[5 ]= 0; img[6 ]= 0; img[7 ]= 0;
    img[8 ]= 0; img[9 ]= 255; img[10]= 0; img[11]= 0; img[12]= 0; img[13]= 128; img[14]= 0; img[15]= 0;
#ifdef USE_CV
    memcpy(cvImg.data, img, 4*4);
#endif
    printf("image\n"); fflush(stdout);

    // Create GLWidget
    glw = new GLWidget();
    ui->rightVLayout->layout()->addWidget( glw );
    glw->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    glw->setMouseTracking(true);
    glw->setStateBar(statBar);
    //glw->updateParam(_expo, _bias, _gama, _lwMax);
    glw->update();

    // menu
    connect( ui->loadImage, SIGNAL(triggered()), this, SLOT(openImage()));
}

MainWindow::~MainWindow(){
    delete ui;
    delete glw;
    delete img;
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
    case Qt::Key_T:
#ifdef USE_CV
    {
        unsigned char *p = cvImg.data;
        clmt.transpose(p, p, cvImg.cols, cvImg.rows);
        size_t tmp = cvImg.rows;
        cvImg.rows = cvImg.cols;
        cvImg.cols = tmp;
        glw->updateTexture(cvImg);
    }
#else
        if(!clmt.transpose(img, img, 2, 2)) qDebug() << "fail!";
        glw->updateTexture(img, 2, 2);
#endif
    if(true){
        double t = clmt.kernelTime(), mt = clmt.memoryTime();
        glw->log("kernel: %lg s, memory: %lg s", t, mt);
    }
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
#ifdef WIN32
    std::string path(fileName.toLocal8Bit().constData());
#else
    std::string path(fileName.toUtf8().constData());
#endif

#ifdef USE_CV
    glw->log("讀取進度: %s ...", path.c_str());
    cv::Mat img = cv::imread(path.c_str());
    cv::cvtColor(img, cvImg, CV_BGR2BGRA);
    glw->updateTexture(cvImg);
#endif

    return true;
}
