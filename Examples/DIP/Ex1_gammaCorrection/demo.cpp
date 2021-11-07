#include <glController.h>
#include <iostream>
#define LOG_STR(S) std::cout<< S << std::endl
uint8_t  demo_rgb_pixels[640*480];
int main(int argc, char *argv[])
{
    LOG_STR("Hello Demo!");
    _GL_CTL_::gl_class_parameter glParam;
    glParam.argc=argc;
    glParam.argv=argv;
    glParam.ImageElem_nb=1;
    glParam.Window_Width=640;
    glParam.Window_Height=480;
    glParam.windowName="hello demo";
    glParam.imageElems[0].image_channel_nb=3;
    glParam.imageElems[0].image_height=480;
    glParam.imageElems[0].image_width=640;
    glParam.imageElems[0].image_pixel_array_addr=(&(demo_rgb_pixels[0]));
    _GL_CTL_::gl_class  gl_ctl_obj = _GL_CTL_::gl_class(_GL_CTL_::GL_CTL_MODE_SHOW_SINGLE_RGB_8888,glParam);
    gl_ctl_obj.start_display_thread();
}