#include <iostream>
#include <time.h>
#include "../include/GL/glut.h"
#include "../include/GL/gl.h"
#include "../include/GL/glu.h"
#include "bmp.h"
#include "../include/glSDK/glHandler.h"

glSDK::glSDK_Controller::glSDK_Controller(glSDK_MODE setMode)
{
    this->gl_sdk_mode =  setMode;
}
glSDK::glSDK_Controller::~glSDK_Controller()
{
    
}
void glSDK::glSDK_Controller::glSDK_Init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glOrtho(-5, 5, -5, 5, 5, 15);
    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
}