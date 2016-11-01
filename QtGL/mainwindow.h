#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QListView>
#include <QStringList>
#include <QStringListModel>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include "GLWidget.h"
#include "ocl/CLfunc.h"

#ifdef USE_CV
#include <opencv2/core/core.hpp>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStatusBar *statBar;

    // Widget for opengl
    GLWidget *glw;

    // image
    unsigned char *img;
#ifdef USE_CV
    cv::Mat cvImg;
#endif

    // opencl
    CLMatTranspose clmt;

protected:
    // keyboard event
    virtual void keyPressEvent  (QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

public slots:
    // menu
    bool openImage();
};

#endif // MAINWINDOW_H
