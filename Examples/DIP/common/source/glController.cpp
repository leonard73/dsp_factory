#include "../include/glController.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#define _CLAMP(A,MAX) ( (A>MAX) ? MAX : A )
void display(void);
void reshape(int w, int h);
void idle(void);
void init_pixels(void);
void init(void);
//GLOBAL VARIABLE DEFINATION
GLubyte glPixelArray[MAX_IMAGE_NB_PER_WINDOW][MAX_IMAGE_SIZE_BYTES];
uint32_t glImageNB=MAX_IMAGE_SIZE_BYTES;
uint32_t glPixelWidth[MAX_IMAGE_NB_PER_WINDOW];
uint32_t glPixelHeight[MAX_IMAGE_NB_PER_WINDOW];
uint32_t glPixelChannel[MAX_IMAGE_NB_PER_WINDOW];
//
_GL_CTL_::gl_class::gl_class(gl_ctl_mode_t glMode,gl_class_parameter param)
{
    this->classGlobal_glMode = glMode;
    this->classGlobal_param.argc = param.argc;
    this->classGlobal_param.argv = param.argv;
    this->classGlobal_param.ImageElem_nb = param.ImageElem_nb;
    this->classGlobal_param.Window_Width = param.Window_Width;
    this->classGlobal_param.Window_Height = param.Window_Height;
    this->classGlobal_param.windowName    = param.windowName;
    this->classGlobal_param.ImageElem_nb=_CLAMP(this->classGlobal_param.ImageElem_nb,MAX_IMAGE_NB_PER_WINDOW);
    glImageNB                  = this->classGlobal_param.ImageElem_nb;
    for(int i = 0 ; i < this->classGlobal_param.ImageElem_nb ; ++i)
    {
        this->classGlobal_param.imageElems[i].image_width            = param.imageElems[i].image_width;
        this->classGlobal_param.imageElems[i].image_height           = param.imageElems[i].image_height;
        this->classGlobal_param.imageElems[i].image_channel_nb       = param.imageElems[i].image_channel_nb;
        this->classGlobal_param.imageElems[i].image_pixel_array_addr = param.imageElems[i].image_pixel_array_addr;
        glPixelWidth[i]        = this->classGlobal_param.imageElems[i].image_width;
        glPixelHeight[i]       = this->classGlobal_param.imageElems[i].image_height;
        glPixelChannel[i]      = this->classGlobal_param.imageElems[i].image_channel_nb;
    }
    this->init_gl_api();
}

void _GL_CTL_::gl_class::init_gl_api()
{
    glutInit(&this->classGlobal_param.argc, this->classGlobal_param.argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(this->classGlobal_param.Window_Width, this->classGlobal_param.Window_Height);
    glutCreateWindow(this->classGlobal_param.windowName);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(&idle);
}

void _GL_CTL_::gl_class::start_display_thread()
{
    glutMainLoop();
}
_GL_CTL_::gl_class::~gl_class()
{
    
}
void init_pixels()
{
    for(int i=0;i<MAX_IMAGE_NB_PER_WINDOW;++i)
    {
        memset(glPixelArray[i],0x00,MAX_IMAGE_SIZE_BYTES);
        // GLubyte pixel = i%255;
        // pixel_array[i*3+0] = pixel;
        // pixel_array[i*3+1] = 255-pixel;
        // pixel_array[i*3+2] = pixel;
    }
}
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    init_pixels();
    // load_bmp_data();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    return;
}

void display(void)
{
    
        glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2f(0,0);
        for(int i = 0 ; i < glImageNB ; ++i)
        {
            glDrawPixels(glPixelWidth[i],glPixelHeight[i],GL_RGB,GL_UNSIGNED_BYTE,glPixelArray[i]);
        }
        
        // update_zoom_ratio();
        // glPixelZoom(zoom_w_ratio,zoom_h_ratio);
        glFlush();
    return;
}

void reshape(int w, int h)
{
        glViewport(0,0,(GLsizei)w,(GLsizei)h);
        // height = (GLint) h;
        // width  = (GLint) w;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0,(GLdouble) w, 0.0,(GLdouble) h);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // update_zoom_ratio();
        // glPixelZoom(zoom_w_ratio,zoom_h_ratio);
}

void idle(void)
{
    static int captured_id=0;
        // camera_t * captured_p=uvc_camera_sdk_stream_captured_once();
	    // printf("ID[%d]: W=%d; H=%d; buf_length=%d; mean_pixel=%d\n",captured_id++,captured_p->width,captured_p->height,captured_p->head.length,meanOfBuffer(captured_p->head.start,captured_p->head.length));
        // loadStreamYuyv2RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        // loadStreamY82RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        glutPostRedisplay();
        usleep(10000);
}