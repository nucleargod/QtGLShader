#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_3_Core>
#include <QMatrix4x4>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QStatusBar>

#include <opencv2/core/core.hpp>

class GLWidget: public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
	Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);

    virtual void setStateBar(QStatusBar *loger){statBar = loger;}
    virtual void updateTexture(const cv::Mat &img);
    virtual void updateParam(float expo, float bias, float gama, float lwMax);
    inline void resetTrans(){
        _zoom=(1), _dx=(0), _dy=(0);
        if(inited) needUpdate=true;
    }

    //用 printf 的格式輸出字串到狀態列
    void log(const char* s, ...);

protected:
	//QT GL Function use
	void initializeGL();
	void resizeGL(int w, int h);
	void paintEvent(QPaintEvent *e);
	void paintGL();

    QStatusBar *statBar;
	bool  inited;
    bool  needUpdate;

    //shader
    QOpenGLShaderProgram *_shader;
    //float  _expo , _bias , _gama , _lwMax , _ldMax ;
    //GLuint _uExpo, _uBias, _uGama, _uLwMax, _uLdMax;

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

    static const unsigned char noTex[4];
};
