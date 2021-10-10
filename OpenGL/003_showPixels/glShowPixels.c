#include <GL/glut.h>
#include <stdio.h>
#include <bmp.h>
#define TEST_PIXEL_WIDTH  (452)
#define TEST_PIXEL_HEIGHT (303)
#define TEST_PIXEL_NUM    (TEST_PIXEL_WIDTH*TEST_PIXEL_HEIGHT)
GLubyte pixel_array[TEST_PIXEL_NUM*3];
GLint width=640,height=800;
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
    readBMP("splash.bmp",&bmpFileHeader,&bmpInfoHeader,testRGB888);
    for(int i=0;i<TEST_PIXEL_NUM*3;i++)
    {
        pixel_array[i] = testRGB888[i];
    }
}
/*********BMP********/
void update_zoom_ratio()
{
    static GLint record_width=452;
    static GLint record_height=303;
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
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // glMatrixMode(GL_PROJECTION);
    // // glOrtho(-5, 5, -5, 5, 5, 15);
    // glMatrixMode(GL_MODELVIEW);
    // gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
    glShadeModel(GL_FLAT);
    init_pixels();
    load_bmp_data();
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    return;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2f(0,0);
    // glPixelZoom(2,2);
    glDrawPixels(TEST_PIXEL_WIDTH,TEST_PIXEL_HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,pixel_array);
    // glRasterPos2i()
    update_zoom_ratio();
    glPixelZoom(zoom_w_ratio,zoom_h_ratio);
    // glColor3f(1.0, 0, 0);
    // glutWireTeapot(3);
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

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(452, 303);
    glutCreateWindow("OpenGL HelloWorld");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}
