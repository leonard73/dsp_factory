#include <glController.h>
#include <iostream>
#include <thread>
#define _GNU_SOURCE
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#define LOG_STR(S) std::cout<< S << std::endl
uint8_t  demo_rgb_pixels[1024*1024*128];
char path[]="/dev/video0";
void thread_uvc();
void init_gl_parameters(_GL_CTL_::gl_class_parameter & glParam);
void init_gl_parameters(_GL_CTL_::gl_class_parameter & glParam)
{
    glParam.ImageElem_nb=1;
    glParam.Window_Width=640;
    glParam.Window_Height=480;
    glParam.windowName="hello demo";
    glParam.imageElems[0].image_channel_nb=3;
    glParam.imageElems[0].image_height=480;
    glParam.imageElems[0].image_width=640;
    glParam.imageElems[0].image_pixel_array_addr=(&(demo_rgb_pixels[0]));
    glParam.uvc_parameter.dev_video_id=0;
    glParam.uvc_parameter.height=480;
    glParam.uvc_parameter.width=640;
    glParam.uvc_parameter.format_mode=0;
    glParam.uvc_parameter.default_timeout=1000000;
}
int main(int argc, char *argv[])
{
    static int captured_id=0;
    LOG_STR("Hello Demo!");
    {

    }
    _GL_CTL_::gl_class_parameter glParam;
    {
        glParam.argc=argc;
        glParam.argv=argv;
        init_gl_parameters(glParam);
    }
    _GL_CTL_::gl_class  gl_ctl_obj = _GL_CTL_::gl_class(_GL_CTL_::GL_CTL_MODE_SHOW_SINGLE_RGB_8888,glParam);
    gl_ctl_obj.start_display_thread();
}