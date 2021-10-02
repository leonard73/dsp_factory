#include <GL/glut.h>
typedef struct shape_rectangle_t
{
    float x;
    float y;
    float z;
}shape_rectangle;
shape_rectangle global_rect1;
// void generate_rect1()
// {
//     global_rect1.x = 
// }
float angle_f=0.0f;
void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    // glOrtho(-5, 5, -5, 5, 5, 15);
    // glMatrixMode(GL_MODELVIEW);
    // gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

    return;
}
 
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    // glLoadIdentity();
    glRotatef(angle_f/360.0f,1,0,0);
    glBegin(GL_TRIANGLES);
    glColor3f(0.3, 0.6, 0.1);
    glVertex2f(-0.2f,-0.2f);
    glVertex2f(-0.2f,0.2f);
    glVertex2f(0.2f,0.2f);

    glVertex2f(-0.2f,-0.2f);
    glVertex2f(0.2f,-0.2f);
    glVertex2f(0.2f,0.2f);
    glEnd();
    angle_f+=10;
    usleep(60000);
    // glutWireTeapot(3);
    glFlush();

    return;
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL HelloWorld");
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
