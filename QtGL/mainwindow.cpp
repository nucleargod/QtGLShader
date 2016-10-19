#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
    //, _tmpImg() // buffer fields for tonemapping
{
    ui->setupUi(this);
    statBar = ui->statusBar;

    // Create GLWidget
    glw = new GLWidget();
    ui->rightVLayout->layout()->addWidget( glw );
    glw->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    glw->setMouseTracking(true);
    glw->setStateBar(statBar);
    //glw->updateParam(_expo, _bias, _gama, _lwMax);
    glw->update();

    // menu
    connect( ui->loadDBItem, SIGNAL(triggered()), this, SLOT(openDragoList()));
}

MainWindow::~MainWindow(){
    delete ui;
}

//*--- private utils
void MainWindow::updateImg(){
    glw->resetTrans();
    //glw->updateTexture(_tmpImg);
    updateParam();
}

void MainWindow::updateParam(){
    //glw->updateParam(_expo, _bias, _gama, _lwMax);
}

//*--- protected key events
void MainWindow::keyPressEvent  (QKeyEvent *e){

}
void MainWindow::keyReleaseEvent(QKeyEvent *e){
    //*--- 測試用，兼 API 使用範例。有閒的再把它做進 UI
    switch(e->key()){
    case Qt::Key_L:{ // 讀取 HDR 檔案測試
        updateImg();
        break;}
    case Qt::Key_R:  // 重設縮放與位移
        glw->resetTrans();
        break;

    default:
        break;
    }
}

//*--- menu 功能
bool MainWindow::openDragoList(){
    QString fileName = QFileDialog::getOpenFileName(this, "讀取進度", "./", "*", 0, QFileDialog::DontUseNativeDialog);
    if (fileName.isEmpty()) return false;
#ifdef WIN32
    //TMLearn::FilePath path(fileName.toLocal8Bit().constData());
#else
    //TMLearn::FilePath path(fileName.toUtf8().constData());
#endif

    //glw->log("讀取進度: %s ...", path.getFullPath());

    return true;
}
