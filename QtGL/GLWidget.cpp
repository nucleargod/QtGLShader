#include "GLWidget.h"
#include <QGLFormat>
#include <iostream>
#include <cmath>

//default texture: blue
const unsigned char GLWidget::noTex[4] = {0, 0, 255, 255};

GLWidget::GLWidget(QWidget *parent)
    :QOpenGLWidget(parent), statBar(NULL), inited(false),
    needUpdate(false), _zoom(1), _dx(0), _dy(0),
    _shader(NULL)
{
    dLMousePress = false;
    dRMousePress = false;
    dMMousePress = false;

    _uExpo  =-1; _expo  = 1.0f;
}

//*--- QT GL Function overide
void GLWidget::initializeGL(){
    makeCurrent();
    initializeOpenGLFunctions();

    //*--- debug output for checking your opengl version
    qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();
    QString versionString(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    qDebug() << "Driver Version String:" << versionString;
    qDebug() << "Current Context:" << this->format();//*/
	
    //*--- GL initialization
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //init texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_tex);

    _texW = 1, _texH = 1;
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, noTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Linear Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// Linear Filtering

    //*--- init shader
    bool d = true;
    _shader = new QOpenGLShaderProgram(this);
    d &= _shader->addShaderFromSourceCode(QOpenGLShader::Vertex  , vertSrc.c_str());
    d &= _shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc.c_str());
    d &= _shader->link();
    qDebug() << "shader compiled:\n" << _shader->log();
    _uExpo  = _shader->uniformLocation("expo");

	inited = true;
    needUpdate = true;
}

void GLWidget::resizeGL(int w, int h){
	widgetW = w;
    widgetH = h;
}

void GLWidget::paintEvent(QPaintEvent *e){
    // 60 hz
	if(!inited) return;

    if(needUpdate){ //更新系統 do update
        makeCurrent();
        paintGL();
        needUpdate = false;
    }
	
	update();
}

void GLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);

    //確保圖片長寬比 keep aspect ratio of image
    float rs = (float)widgetW / (float)widgetH;
    float ri = (float)_texW / (float)_texH;
    float _x=1.0f, _y=1.0f;
    if(rs > ri){ _x = ri / rs; }//y = 1.0
    else { _y = rs / ri;}//x=1.0

    //縮放與位移 moving & scaling
    _x = _x*_zoom;
    _y = _y*_zoom;

    //頂點資料 vertex data
    float vertice[] = {
        _x + _dx,  _y + _dy, 1, 0,
       -_x + _dx,  _y + _dy, 0, 0,
       -_x + _dx, -_y + _dy, 0, 1,
        _x + _dx, -_y + _dy, 1, 1
    };
    const GLubyte allIndices[] = {0, 1, 2, 3};
	
    //畫圖 draw
    _shader->bind();
    _shader->setUniformValue(_uExpo , _expo );

    //* openGLES 3.0 == opengl 4.3
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tex);
    _shader->setUniformValue(_shader->uniformLocation("my_texture"), 0);
    glEnableVertexAttribArray(_shader->attributeLocation("a_position"));
    glVertexAttribPointer(_shader->attributeLocation("a_position"),
        4, GL_FLOAT, false, 4*sizeof(float), vertice);
    glEnableVertexAttribArray(_shader->attributeLocation("a_texCoord"));
    glVertexAttribPointer(_shader->attributeLocation("a_texCoord"),
        4, GL_FLOAT, false, 4*sizeof(float), vertice+2);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, allIndices);
    //*/
    
    /*opengl 1.2
    glBindTexture(GL_TEXTURE_2D, _tex);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 4*sizeof(float), vertice);
    glTexCoordPointer(2, GL_FLOAT, 4*sizeof(float), vertice+2);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, allIndices);//*/

    /*opengl 1.0
    glBindTexture(GL_TEXTURE_2D, _tex);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d( _x + _dx,  _y + _dy); glTexCoord2d(0, 0);
    glVertex2d(-_x + _dx,  _y + _dy); glTexCoord2d(0, 1);
    glVertex2d(-_x + _dx, -_y + _dy); glTexCoord2d(1, 1);
    glVertex2d( _x + _dx, -_y + _dy); glTexCoord2d(1, 0);
    glEnd();//*/

    _shader->release();
}

//*--- update utils
#ifdef USE_CV
void GLWidget::updateTexture(const cv::Mat &rgb){
    qDebug() << "updateTexture";
    if(rgb.empty()){
        qDebug() << "updateTexture with noTex";
        _texW = 1, _texH = 1;
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texW, _texH, 0, GL_RGB, GL_UNSIGNED_BYTE, noTex);
    }
    else{
        qDebug() << "updateTexture with mat";
        _texW = rgb.cols, _texH = rgb.rows;
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, _tex);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texW, _texH, 0, GL_RGB, GL_FLOAT, rgb.data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texW, _texH, 0, GL_BGR, GL_UNSIGNED_BYTE, rgb.data);
    }

    needUpdate = true;
    update();
}
#endif

void GLWidget::updateParam(float expo){
    _expo  = expo ;
    log("updateParam: %g", expo);

    needUpdate = true;
    update();
}

//*--- Mouse events
void GLWidget::mousePressEvent(QMouseEvent *event){  

	if(event->button() == Qt::LeftButton){
        dLMousePress = true;
        mLMousePressPos = event->pos();
	}
    else if(event->button() == Qt::RightButton){
        dRMousePress = true;
        mRMousePressPos = event->pos();
	}
    else if(event->button() == Qt::MidButton){
        dMMousePress = true;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event){
        if(dLMousePress){//左鍵位移 left key for moving
            QPoint np = event->pos();
            _dx += ((float)(np.x() - mLMousePressPos.x())*2.0f / (float)widgetW);
            _dy -= ((float)(np.y() - mLMousePressPos.y())*2.0f / (float)widgetH);
            //log("Mouse(Left) move: %f, %f", _dx, _dy);
            mLMousePressPos = np;
            needUpdate = true;
        }
        if(dRMousePress){//右鍵縮放 right key for scaling
            QPoint np = event->pos();
            _zoom += ((float)(np.y() - mRMousePressPos.y()) / (float)widgetH);
            mRMousePressPos = np;
            needUpdate = true;
        }
    //*/
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event){

	if(event->button() == Qt::LeftButton){
        mLMousePressPos = event->pos();
        //qDebug() << "Mouse(Left) clicked: " << mMouseLeftPressPos;
        log("Mouse(Left) clicked: %d, %d", mLMousePressPos.x(), mLMousePressPos.y());
	}

    dLMousePress = false;
    dRMousePress = false;
    dMMousePress = false;
}

//*--- debug utils
// 用 printf 的格式輸出字串到狀態列
// log formated string to status bar
void GLWidget::log(const char* s, ...){
    if(statBar == NULL) return;

    va_list args;
    va_start(args, s);

    char str[512];
    vsprintf(str, s, args);
    statBar->showMessage(QString(str), 21000);

    va_end(args);
}
