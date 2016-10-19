#include "GLWidget.h"
#include <cmath>

#ifdef ANDROID
#include <GLES2/gl2.h>
#define _PX_FMT GL_RGB
#else
#include <GL/gl.h>
#define _PX_FMT GL_RGB32F
#endif

const unsigned char GLWidget::noTex[4] = {0, 0, 255, 1};

GLWidget::GLWidget(QWidget *parent)
    :QOpenGLWidget(parent), statBar(NULL), inited(false),
    needUpdate(false), _zoom(1), _dx(0), _dy(0),
    _shader(NULL)
{
    dLMousePress = false;
    dRMousePress = false;
    dMMousePress = false;

//    _uExpo  =-1; _expo  = 1.0f;
//    _uBias  =-1; _bias  = 0.8f;
//    _uGama  =-1; _gama  = 1.0f;
//    _uLwMax =-1; _lwMax = 1.0f;
//    _uLdMax =-1; _ldMax = 100.0f;
}

void GLWidget::initializeGL()
{
	makeCurrent();
/*
	qDebug() << "OpenGL Versions Supported: " << QGLFormat::openGLVersionFlags();

	QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
	qDebug() << "Driver Version String:" << versionString;
    qDebug() << "Current Context:" << this->format();//*/

	// Set up the rendering context, load shaders and other resources, etc.:
	//QOpenGLFunctions *qoglfunc = QOpenGLContext::currentContext()->functions();
	//qoglfunc->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// 	glShadeModel(GL_SMOOTH);
	// 	glClearDepth(1.0);
	// 	glEnable(GL_DEPTH_TEST);
	
	// GL initialization
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //init texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_tex);

    _texW = 1, _texH = 1;
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexImage2D(GL_TEXTURE_2D, 0, _PX_FMT, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, noTex);
#ifdef ANDROID
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Linear Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// Linear Filtering
#endif

    //init shader
//    bool d = true;
    _shader = new QOpenGLShaderProgram(this);
//    d &= _shader->addShaderFromSourceCode(QOpenGLShader::Vertex  , DRAGO03::vertSrc.c_str());
//    d &= _shader->addShaderFromSourceCode(QOpenGLShader::Fragment, DRAGO03::fragSrc.c_str());
//    d &= _shader->link();
//    qDebug() << "shader compiled:\n" << _shader->log();
//    _uExpo  = _shader->uniformLocation("expo");
//    _uBias  = _shader->uniformLocation("lgBiasDlgHalf");
//    _uGama  = _shader->uniformLocation("gama");
//    _uLwMax = _shader->uniformLocation("lwMax");
//    _uLdMax = _shader->uniformLocation("n");

	inited = true;
    needUpdate = true;
}

void GLWidget::resizeGL(int w, int h)
{
	widgetW = w;
	widgetH = h;
    //makeCurrent();

	//  glViewport(0, 0, (GLint)w, (GLint)h);
	// 
	// 	glMatrixMode(GL_PROJECTION);
	// 	glLoadIdentity();
	// 
	// 	gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();
}

// 60 fps
void GLWidget::paintEvent(QPaintEvent *e)
{
	if(!inited) return;

    if(needUpdate){
        //qDebug() << "paintEvent: " << updateClockGL();
        //更新系統
        makeCurrent();
        paintGL();
        needUpdate = false;
    }
	
	update();
}

void GLWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT);

    //確保圖片長寬比
    float rs = (float)widgetW / (float)widgetH;
    float ri = (float)_texW / (float)_texH;
    float _x=1.0f, _y=1.0f;
    if(rs > ri){ _x = ri / rs; }//y = 1.0
    else { _y = rs / ri;}//x=1.0

    //縮放與位移
    _x = _x*_zoom;
    _y = _y*_zoom;

    //頂點資料
    float vertice[] = {
        _x + _dx,  _y + _dy, 1, 0,
       -_x + _dx,  _y + _dy, 0, 0,
       -_x + _dx, -_y + _dy, 0, 1,
        _x + _dx, -_y + _dy, 1, 1
    };
    const GLubyte allIndices[] = {0, 1, 2, 3};
	
    //畫圖
//    float n = _ldMax * 0.01f * std::log(10.0f) / std::log(_lwMax + 1.0f);
//    float lgBiasDlgHalf = std::log(_bias) / std::log(0.5f);
//    _shader->bind();
//    _shader->setUniformValue(_uExpo , _expo );
//    _shader->setUniformValue(_uBias , lgBiasDlgHalf );
//    _shader->setUniformValue(_uGama , _gama );
//    _shader->setUniformValue(_uLwMax, _lwMax);
//    _shader->setUniformValue(_uLdMax, n);

#ifdef ANDROID
    //* openGLES 3.0
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
#else
    //*opengl 1.1
    glBindTexture(GL_TEXTURE_2D, _tex);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 4*sizeof(float), vertice);
    glTexCoordPointer(2, GL_FLOAT, 4*sizeof(float), vertice+2);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, allIndices);//*/

    /*opengl 1.0
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d( _x + _dx,  _y + _dy); glTexCoord2d(0, 0);
    glVertex2d(-_x + _dx,  _y + _dy); glTexCoord2d(0, 1);
    glVertex2d(-_x + _dx, -_y + _dy); glTexCoord2d(1, 1);
    glVertex2d( _x + _dx, -_y + _dy); glTexCoord2d(1, 0);
    glEnd();//*/
#endif

    //_shader->release();
}

//更新貼圖
void GLWidget::updateTexture(const cv::Mat &rgb){
    if(rgb.empty()){
        _texW = 1, _texH = 1;
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexImage2D(GL_TEXTURE_2D, 0, _PX_FMT, _texW, _texH, 0, GL_RGB, GL_UNSIGNED_BYTE, noTex);
    }
    else{
        _texW = rgb.cols, _texH = rgb.rows;
        makeCurrent();
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexImage2D(GL_TEXTURE_2D, 0, _PX_FMT, _texW, _texH, 0, GL_RGB, GL_FLOAT, rgb.data);
		//glTexImage2D(GL_TEXTURE_2D, 0, _PX_FMT, _texW, _texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb.data);
    }

    needUpdate = true;
    update();
}

void GLWidget::updateParam(float expo, float bias, float gama, float lwMax){
//    _expo  = expo ;
//    _bias  = bias ;
//    _gama  = gama ;
//    _lwMax = lwMax < 1e-5f ? 1e-5f : lwMax;

    needUpdate = true;
    update();
}

//************************************************************
//  Function: mouseEvent (Windows move with mouse press.)
//============================================================
void GLWidget::mousePressEvent(QMouseEvent *event){  

	if(event->button() == Qt::LeftButton){
        dLMousePress = true;
        mLMousePressPos = event->pos();

        /*switch(_state){
        case State::s_none : mLMousePressPos = event->pos();
        case State::s_block: break;

		default: break;
		}//*/
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
        if(dLMousePress){//左鍵位移
            QPoint np = event->pos();
            _dx += ((float)(np.x() - mLMousePressPos.x())*2.0f / (float)widgetW);
            _dy -= ((float)(np.y() - mLMousePressPos.y())*2.0f / (float)widgetH);
            //log("Mouse(Left) move: %f, %f", _dx, _dy);
            mLMousePressPos = np;
            needUpdate = true;
        }
        if(dRMousePress){//右鍵縮放
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
		/*switch(_state){
        case State::s_block:break;

		default: break;
		}//*/
	}

    dLMousePress = false;
    dRMousePress = false;
    dMMousePress = false;
}
//************************************************************

//用 printf 的格式輸出字串到狀態列
void GLWidget::log(const char* s, ...){
    if(statBar == NULL) return;

    va_list args;
    va_start(args, s);

    char str[512];
    vsprintf(str, s, args);
    statBar->showMessage(QString(str), 21000);

    va_end(args);
}
