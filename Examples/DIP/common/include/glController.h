#ifndef __GL_CONTROLLER_H__
#define __GL_CONTROLLER_H__
#include <stdint.h>
#define MAX_IMAGE_NB_PER_WINDOW (12)
#define MAX_IMAGE_SIZE_BYTES    (1920*1080*3)
namespace _GL_CTL_
{
    typedef struct gl_class_parameter_imageInfo_t
    {
        uint32_t image_height;
        uint32_t image_width;
        uint32_t image_channel_nb;
        uint8_t* image_pixel_array_addr;
        uint32_t size_perPixel;
    }gl_class_parameter_imageInfo;

    typedef struct gl_class_parameter_t
    {
        gl_class_parameter_imageInfo imageElems[MAX_IMAGE_NB_PER_WINDOW]; 
        uint32_t Window_Height;
        uint32_t Window_Width;
        uint32_t ImageElem_nb;
        int      argc;
        char **  argv;
        const char * windowName;
    }gl_class_parameter;
    enum gl_ctl_mode_t
    {
            GL_CTL_MODE_SHOW_SINGLE_RGB_8888  = 0,
            GL_CTL_MODE_SHOW_DUAL_RGB_8888    = 1,
            GL_CTL_MODE_SHOW_MULTI_RGB_8888   = 2
    };
    class gl_class
    {

        private:    gl_ctl_mode_t       classGlobal_glMode;
        private:    gl_class_parameter  classGlobal_param;
        private:    void init_gl_api();
        public:     void start_display_thread();
        public:     gl_class(gl_ctl_mode_t glMode,gl_class_parameter param);
        public:     ~gl_class();
    };
};



#endif  //__GL_CONTROLLER_H__