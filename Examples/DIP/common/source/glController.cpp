#include "../include/glController.h"
#include "../include/uvc_cam_sdk.h"
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
uint8_t * rgbArrayAddr[MAX_IMAGE_NB_PER_WINDOW];
uint32_t default_timeout_uvc=1000000;
uint32_t uvc_h=480;
uint32_t uvc_w=640;
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
    uvc_w = this->classGlobal_param.uvc_parameter.width;
    uvc_h = this->classGlobal_param.uvc_parameter.height;
    char path[]="/dev/video0";
    int ret=uvc_camera_sdk_init(path,this->classGlobal_param.uvc_parameter.width,this->classGlobal_param.uvc_parameter.height,this->classGlobal_param.uvc_parameter.format_mode);
    default_timeout_uvc=this->classGlobal_param.uvc_parameter.default_timeout;
    for(int i = 0 ; i < this->classGlobal_param.ImageElem_nb ; ++i)
    {
        this->classGlobal_param.imageElems[i].image_width            = param.imageElems[i].image_width;
        this->classGlobal_param.imageElems[i].image_height           = param.imageElems[i].image_height;
        this->classGlobal_param.imageElems[i].image_channel_nb       = param.imageElems[i].image_channel_nb;
        this->classGlobal_param.imageElems[i].image_pixel_array_addr = param.imageElems[i].image_pixel_array_addr;
        glPixelWidth[i]        = this->classGlobal_param.imageElems[i].image_width;
        glPixelHeight[i]       = this->classGlobal_param.imageElems[i].image_height;
        glPixelChannel[i]      = this->classGlobal_param.imageElems[i].image_channel_nb;
        rgbArrayAddr[i]        = this->classGlobal_param.imageElems[i].image_pixel_array_addr;
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
    uvc_camera_sdk_stream_start(default_timeout_uvc);
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
void update_glPixel()
{
    for(int i=0;i<glImageNB;++i)
    {
        uint32_t channel_nb = glPixelChannel[i];
        uint32_t pixel_size  = glPixelHeight[i] * glPixelWidth[i];
        for(int k=0;k<pixel_size*channel_nb;++k)
        {
                GLubyte pixel = rgbArrayAddr[i][k];
                glPixelArray[i][k] = pixel;
        }

    }
}
static inline int minmax(int min, int v, int max)
{
	return (v<min)?min:((max<v)?max:v);
}
void loadStreamYuyv2RGBA(uint8_t * pixel_array,uint8_t* yuyv, uint32_t width, uint32_t height)
{
	int i = 0, j = 0;
	int32_t y0 = 0, y1 = 0, u = 0,  v = 0;
	int index = 0;
    for (i = 0; i < height; ++i)
	{
		for (j = 0; j < width; j += 2)
		{
			index = i * width + j;
			y0 = yuyv[index*2 + 0] << 8;
			u = yuyv[index*2 + 1] - 128;
			y1 = yuyv[index*2 + 2] << 8;
			v = yuyv[index*2 + 3] - 128;
			
			pixel_array[index*3 + 0] = minmax(0, (y0 + 359*v)>>8, 255);
			pixel_array[index*3 + 1] = minmax(0, (y0 + 88*v - 183*u)>>8, 255);
			pixel_array[index*3 + 2] = minmax(0, (y0 + 454*u)>>8, 255);
			pixel_array[index*3 + 3] = minmax(0, (y1 + 359*v)>>8, 255);
			pixel_array[index*3 + 4] = minmax(0, (y1 + 88*v - 183*u)>>8, 255);
			pixel_array[index*3 + 5] = minmax(0, (y1 + 454*u)>>8, 255);
		}
	}
}
void uvc_capture_frame()
{

    static uint32_t captured_id=0;
    camera_t * captured_p=uvc_camera_sdk_stream_captured_once();
    printf("captured_id = %d\n",captured_id++);
	printf("ID[%d]: W=%d; H=%d; buf_length=%d; mean_pixel=%d\n",captured_id++,captured_p->width,captured_p->height,captured_p->head.length,meanOfBuffer(captured_p->head.start,captured_p->head.length));
    loadStreamYuyv2RGBA(rgbArrayAddr[0],captured_p->head.start,uvc_w,uvc_h);
}
void idle(void)
{
    static int captured_id=0;
        // camera_t * captured_p=uvc_camera_sdk_stream_captured_once();
	    // printf("ID[%d]: W=%d; H=%d; buf_length=%d; mean_pixel=%d\n",captured_id++,captured_p->width,captured_p->height,captured_p->head.length,meanOfBuffer(captured_p->head.start,captured_p->head.length));
        // loadStreamYuyv2RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        // loadStreamY82RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        uvc_capture_frame();
        update_glPixel();
        glutPostRedisplay();
        usleep(80000);
}