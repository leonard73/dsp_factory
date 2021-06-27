#include "bmp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//************************test************************************//
#define test_rgb_width (640)
#define test_rgb_height (800)
#define test_save_path "test.bmp"
#define test_load_path "frame_4394.bin"
#define bmp_read_path "splash.bmp"
#define bmp_save_path "splash_save.bmp"
unsigned char testRGB888[test_rgb_width*test_rgb_height*3];
unsigned char testLoadY8[test_rgb_width*test_rgb_height];
void do_setR255(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            dst[i*width*3+j*3+0] = src[i*width*3+j*3+0];
            dst[i*width*3+j*3+1] = src[i*width*3+j*3+0];
            dst[i*width*3+j*3+2] = src[i*width*3+j*3+0];
        }
    }
}
void do_RGB2GRAY(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define FLOAT_WEIGHT_RED_LIGHT      (0.29900)
    #define FLOAT_WEIGHT_GREEN_LIGHT    (0.58700)
    #define FLOAT_WEIGHT_BLUE_LIGHT     (0.11400)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            unsigned char gray = (unsigned char)(FLOAT_WEIGHT_RED_LIGHT*(float)src[i*width*3+j*3+0]
                                                +FLOAT_WEIGHT_GREEN_LIGHT*(float)src[i*width*3+j*3+1]
                                                +FLOAT_WEIGHT_BLUE_LIGHT*(float)src[i*width*3+j*3+2] );
            dst[i*width*3+j*3+0] = gray;
            dst[i*width*3+j*3+1] = gray;
            dst[i*width*3+j*3+2] = gray;
        }
    }
    #undef FLOAT_WEIGHT_RED_LIGHT
    #undef FLOAT_WEIGHT_GREEN_LIGHT
    #undef FLOAT_WEIGHT_BLUE_LIGHT
}
void main()
{
    // loadFile(test_load_path,test_rgb_width*test_rgb_height,testLoadY8);
    // for(int i=0;i<test_rgb_width*test_rgb_height;i++)
    // {
    //     testRGB888[i*3]   = testLoadY8[i];
    //     testRGB888[i*3+1] = testLoadY8[i];
    //     testRGB888[i*3+2] = testLoadY8[i];
    // }
    // save_RawRGB_bmpFile(test_save_path,testRGB888,test_rgb_width*test_rgb_height*3,test_rgb_width,test_rgb_height);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(bmp_read_path,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)<<BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    do_setR255(testRGB888,dst,read_width,read_height);
    // memcpy(dst,testRGB888,read_width*read_height*read_pixel_bytes);
    save_RawGray8_bmpFile(bmp_save_path,dst,read_width*read_height,read_width,read_height);
    free(dst);
    sleep(1);
    free_bmpRes();
}