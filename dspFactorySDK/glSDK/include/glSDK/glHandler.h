#ifndef __GL_HANDLER_H__
#define __GL_HANDLER_H__
namespace glSDK{

    class glSDK_Controller{
        public: enum glSDK_MODE
        {
            glSDK_MODE_SHOW_SINGLE_RGB=0,
            glSDK_MODE_SHOW_DUAL_RGB=1
        };
        private: glSDK_MODE gl_sdk_mode;
        private: void glSDK_Init();
        public: glSDK_Controller(glSDK_MODE setMode);
        public: ~glSDK_Controller();
    };
};


#endif //__GL_HANDLER_H__