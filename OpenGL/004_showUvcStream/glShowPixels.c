#include <GL/glut.h>
#include <stdio.h>
#include <bmp.h>
#include <uvc_cam_sdk.h>
#define TEST_PIXEL_WIDTH  (1280)
#define TEST_PIXEL_HEIGHT (480)
#define TEST_PIXEL_NUM    (TEST_PIXEL_WIDTH*TEST_PIXEL_HEIGHT)
GLubyte pixel_array[TEST_PIXEL_NUM*3];
GLint width=TEST_PIXEL_WIDTH,height=TEST_PIXEL_HEIGHT;
float zoom_h_ratio =1.0;
float zoom_w_ratio =1.0;
/*********BMP********/
unsigned char * testRGB888;
void load_bmp_data()
{
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    testRGB888 = malloc(TEST_PIXEL_NUM*3);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP("test.bmp",&bmpFileHeader,&bmpInfoHeader,testRGB888);
    for(int i=0;i<TEST_PIXEL_NUM*3;i++)
    {
        pixel_array[i] = testRGB888[i];
    }
}
/*********BMP********/
void update_zoom_ratio()
{
    static GLint record_width=TEST_PIXEL_WIDTH;
    static GLint record_height=TEST_PIXEL_HEIGHT;
    zoom_w_ratio = (float)width/(float)record_width;
    zoom_h_ratio = (float)height/(float)record_height;
    // record_width=width;
    // record_height=height;
    printf("zoom_w_ratio = %f; zoom_h_ratio=%f;\n",zoom_w_ratio,zoom_h_ratio);
}
void init_pixels()
{
    for(int i=0;i<TEST_PIXEL_NUM;++i)
    {
        GLubyte pixel = i%255;
        pixel_array[i*3+0] = pixel;
        pixel_array[i*3+1] = 255-pixel;
        pixel_array[i*3+2] = pixel;
    }
}
static inline int minmax(int min, int v, int max)
{
	return (v<min)?min:((max<v)?max:v);
}
void loadStreamYuyv2RGBA(uint8_t* yuyv, uint32_t width, uint32_t height)
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
void loadStreamY82RGBA(uint8_t* yuyv, uint32_t width, uint32_t height)
{
	int i = 0, j = 0;
	int32_t y = 0;
	int index = 0;
    for (i = 0; i < height; ++i)
	{
		for (j = 0; j < width; j +=1)
		{
			index = i * width + j;
			y = yuyv[index];			
			pixel_array[index*3 + 0] = minmax(0, y, 255);
			pixel_array[index*3 + 1] = minmax(0, y, 255);
			pixel_array[index*3 + 2] = minmax(0, y, 255);
		}
	}
}
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    init_pixels();
    load_bmp_data();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    int ret=uvc_camera_sdk_init("/dev/video0",TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT,0);
	uvc_camera_sdk_stream_start(1000212);
    return;
}

void display(void)
{
    
        glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2f(0,0);
        glDrawPixels(TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,pixel_array);
        update_zoom_ratio();
        glPixelZoom(zoom_w_ratio,zoom_h_ratio);
        glFlush();
    return;
}

void reshape(int w, int h)
{
        glViewport(0,0,(GLsizei)w,(GLsizei)h);
        height = (GLint) h;
        width  = (GLint) w;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0,(GLdouble) w, 0.0,(GLdouble) h);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        update_zoom_ratio();
        glPixelZoom(zoom_w_ratio,zoom_h_ratio);
}

void idle(void)
{
    static int captured_id=0;
        camera_t * captured_p=uvc_camera_sdk_stream_captured_once();
	    printf("ID[%d]: W=%d; H=%d; buf_length=%d; mean_pixel=%d\n",captured_id++,captured_p->width,captured_p->height,captured_p->head.length,meanOfBuffer(captured_p->head.start,captured_p->head.length));
        loadStreamYuyv2RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        // loadStreamY82RGBA(captured_p->head.start,TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT);
        glutPostRedisplay();
        usleep(10000);
}
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(TEST_PIXEL_WIDTH, TEST_PIXEL_HEIGHT);
    glutCreateWindow("OpenGL HelloWorld");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(&idle);
    glutMainLoop();

    return 0;
}
