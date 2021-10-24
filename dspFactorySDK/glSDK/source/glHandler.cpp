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