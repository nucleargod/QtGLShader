#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QMatrix4x4>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QStatusBar>

#ifdef USE_CV
#include <opencv2/core/core.hpp>
#endif

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);

    //field access
    virtual float getExpo(){return _expo;}
    virtual void setStateBar(QStatusBar *loger){statBar = loger;}

    //update utils
#ifdef USE_CV
    virtual void updateTexture(const cv::Mat &img);
#endif
    virtual void updateParam(float expo);
    inline void resetTrans(){
        _zoom=(1), _dx=(0), _dy=(0);
        if(inited) needUpdate=true;
    }

    //debug utils
    void log(const char* s, ...);

protected:
    //QT GL Function overide
	void initializeGL();
	void resizeGL(int w, int h);
	void paintEvent(QPaintEvent *e);
	void paintGL();

    QStatusBar *statBar;
	bool  inited;
    bool  needUpdate;

    //shader
    QOpenGLShaderProgram *_shader;
    float  _expo ;
    GLuint _uExpo;

	//Mouse event
	virtual void mousePressEvent  (QMouseEvent *event);
	virtual void mouseMoveEvent   (QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    
    static const std::string vertSrc;
    static const std::string fragSrc;

private:
	//GLWidget Width & Height
	int widgetW, widgetH;

    //Mouse event parameter
    QPoint mLMousePressPos;
    QPoint mRMousePressPos;
    bool dLMousePress;
    bool dRMousePress;
    bool dMMousePress;

    //texture
    GLuint _tex;
    int _texW, _texH;
    float _zoom, _dx, _dy;

    //default texture
    static const unsigned char noTex[4];
};

#endif
