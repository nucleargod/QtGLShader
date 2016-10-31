#include "GLWidget.h"

const std::string GLWidget::vertSrc = 
    "attribute vec2 a_position;      \n"
    "attribute vec2 a_texCoord;      \n"
    "varying vec2 texture_coordinate;\n"
    
    "void main(){                    \n"
    "   gl_Position = vec4(a_position, 1, 1);\n"
    "   texture_coordinate = a_texCoord;     \n"
    "}";
    
const std::string GLWidget::fragSrc =
    "varying vec2 texture_coordinate;\n"
    "uniform sampler2D my_texture;   \n"
    "uniform float expo;              \n"

    "void main(){\n"
    "   vec4 c = texture2D(my_texture, texture_coordinate);             \n"
    "   gl_FragColor = c*expo;                                          \n"
    "}";
