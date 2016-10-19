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

    // controlls
    //cv::Mat _tmpImg; // for tone mapped image

    // utils
    void updateImg();
    void updateParam();

protected:
    // keyboard event
    virtual void keyPressEvent  (QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

public slots:
    // menu
    bool openDragoList();
};

#endif // MAINWINDOW_H
