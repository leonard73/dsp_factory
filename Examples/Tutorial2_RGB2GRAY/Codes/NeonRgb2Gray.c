#include <stdio.h>
#include <string.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <arm_neon.h>
#include <bmp.h>
#include <math.h>
#ifdef PLATFORM_ANDROID_ARM64
#define TEST_DATA_PATH "/data/local/tmp/splash.bmp"
#define TEST_OUT_ARM_DATA_PATH_I16 "/data/local/tmp/splash_out_cpu_I16.bmp"
#define TEST_OUT_ARM_DATA_PATH_I32 "/data/local/tmp/splash_out_cpu_I32.bmp"
#define TEST_OUT_ARM_DATA_PATH_F32 "/data/local/tmp/splash_out_cpu_F32.bmp"
#define TEST_OUT_NEON_DATA_PATH "/data/local/tmp/splash_out_neon.bmp"
#elif defined (PLATFORM_LINUX_ARM64)
#define TEST_DATA_PATH "tutorial1_data.jpg"
#define TEST_OUT_ARM_DATA_PATH "tutorial1_data_outArm.jpg"
#define TEST_OUT_NEON_DATA_PATH "tutorial1_data_outNEON.jpg"
#endif
#define TEST_DATA_LENGTH_BYTES (116536)
#define TEST_TIMES_REPLICATE 128
void logMenu();
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
void do_RGB2GRAY_F32(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
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
void do_RGB2GRAY_I32(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I32_WEIGHT_RED_LIGHT      (19595)
    #define I32_WEIGHT_GREEN_LIGHT    (38469)
    #define I32_WEIGHT_BLUE_LIGHT     (7472)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            unsigned char gray = (unsigned char)((I32_WEIGHT_RED_LIGHT*(unsigned int)src[i*width*3+j*3+0]
                                                +I32_WEIGHT_GREEN_LIGHT*(unsigned int)src[i*width*3+j*3+1]
                                                +I32_WEIGHT_BLUE_LIGHT*(unsigned int)src[i*width*3+j*3+2])>>16);
            dst[i*width*3+j*3+0] = gray;
            dst[i*width*3+j*3+1] = gray;
            dst[i*width*3+j*3+2] = gray;
        }
    }
    #undef I32_WEIGHT_RED_LIGHT
    #undef I32_WEIGHT_GREEN_LIGHT
    #undef I32_WEIGHT_BLUE_LIGHT
}
void do_RGB2GRAY_I16(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (76)
    #define I16_WEIGHT_GREEN_LIGHT    (150)
    #define I16_WEIGHT_BLUE_LIGHT     (30)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            unsigned char gray = (unsigned char)((I16_WEIGHT_RED_LIGHT*(unsigned short)src[i*width*3+j*3+0]
                                                +I16_WEIGHT_GREEN_LIGHT*(unsigned short)src[i*width*3+j*3+1]
                                                +I16_WEIGHT_BLUE_LIGHT*(unsigned short)src[i*width*3+j*3+2])>>8);
            dst[i*width*3+j*3+0] = gray;
            dst[i*width*3+j*3+1] = gray;
            dst[i*width*3+j*3+2] = gray;
        }
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
}
#ifdef _USE_ARM_NEON_OPT_
void do_RGB2GRAY_I16_Neon(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (0x4c4c4c4c4c4c4c4c) //76
    #define I16_WEIGHT_GREEN_LIGHT    (0x9696969696969696) //150
    #define I16_WEIGHT_BLUE_LIGHT     (0x1e1e1e1e1e1e1e1e) //30
    #define NEON_D_SIMD_LENGTH_BYTES  (8)
    uint8x8_t  v_r_coff  = vcreate_u8(I16_WEIGHT_RED_LIGHT);
    uint8x8_t  v_g_coff  = vcreate_u8(I16_WEIGHT_GREEN_LIGHT);
    uint8x8_t  v_b_coff  = vcreate_u8(I16_WEIGHT_BLUE_LIGHT);
    uint16x8_t v_zero    = vcombine_u16(vcreate_u8(0x0000000000000000),vcreate_u8(0x0000000000000000));
    unsigned int width_pixel_bytes = width*3;
    unsigned int loop_step_bytes   = NEON_D_SIMD_LENGTH_BYTES*3;
    uint8x8x3_t v_store_rgb;
    uint8x8x3_t v_load_rgb ;
    uint16x8_t  v16_result;
    uint8x8_t   v8_result_l;
    for(unsigned int i=0;i<height;i++)
    {
        for(unsigned int j=0;j<width_pixel_bytes;j+=loop_step_bytes)
        {
            v_load_rgb         = vld3_u8(src+j);
            v16_result         = vmlal_u8(v_zero,    v_load_rgb.val[0],v_r_coff);
            v16_result         = vmlal_u8(v16_result,v_load_rgb.val[1],v_g_coff);
            v16_result         = vmlal_u8(v16_result,v_load_rgb.val[2],v_b_coff);
            v8_result_l        = vrshrn_n_u16(v16_result,8);
            v_store_rgb.val[0] = v8_result_l;
            v_store_rgb.val[1] = v8_result_l;
            v_store_rgb.val[2] = v8_result_l;
            vst3_u8(dst+j,v_store_rgb);
        }
        src+=width_pixel_bytes;
        dst+=width_pixel_bytes;
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
    #undef NEON_D_SIMD_LENGTH_BYTES
}
#endif
#define MAX_LOAD_LOCAL_BYTES (1024*1024)
void do_neon_rgb2gray()
{
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes);
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_Neon(testRGB888, dst,read_width, read_height);
    #else
    do_RGB2GRAY_I16(testRGB888, dst,read_width, read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN NEON OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_NEON_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
}
void do_arm_rgb2grayI32()
{
    #undef _USE_ARM_NEON_OPT_
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_Neon(testRGB888, dst,read_width, read_height);
    #else
    do_RGB2GRAY_I32(testRGB888, dst,read_width, read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM I32 OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH_I32,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
}
void do_arm_rgb2grayF32()
{
    #undef _USE_ARM_NEON_OPT_
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_Neon(testRGB888, dst,read_width, read_height);
    #else
    do_RGB2GRAY_F32(testRGB888, dst,read_width, read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM F32 OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH_F32,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
}
void do_arm_rgb2grayI16()
{
    #undef _USE_ARM_NEON_OPT_
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_Neon(testRGB888, dst,read_width, read_height);
    #else
    do_RGB2GRAY_I16(testRGB888, dst,read_width, read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM I16 OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH_I16,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
}
int main()
{
    for(int i=0;i<TEST_TIMES_REPLICATE;i++)
    {
        do_neon_rgb2gray();
        do_arm_rgb2grayF32();
        do_arm_rgb2grayI32();
        do_arm_rgb2grayI16();
    }
    return 0;
}
void logMenu()
{
    printf("############################################################################\n");
    printf("############################################################################\n");
    printf("##############         This is DSPFACTORY T2 TEST          #################\n");
    printf("##############         INPUT:  A 452x302 RGB BMP FILE      #################\n");
    printf("##############         OUTPUT: LinearTransfrom Result      #################\n");
    printf("##############         AIM: VTBL ISA Implementation        #################\n");
    printf("##############         This is DSPFACTORY T2 TEST          #################\n");
    printf("############################################################################\n");
    printf("############################################################################\n");   
}