#include <stdio.h>
#include <string.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <arm_neon.h>
#include <bmp.h>
#ifdef PLATFORM_ANDROID_ARM64
#define TEST_DATA_PATH "/data/local/tmp/splash.bmp"
#define TEST_OUT_ARM_DATA_PATH "/data/local/tmp/splash_out_cpu.bmp"
#define TEST_OUT_NEON_DATA_PATH "/data/local/tmp/splash_out_neon.bmp"
#elif defined (PLATFORM_LINUX_ARM64)
#define TEST_DATA_PATH "tutorial1_data.jpg"
#define TEST_OUT_ARM_DATA_PATH "tutorial1_data_outArm.jpg"
#define TEST_OUT_NEON_DATA_PATH "tutorial1_data_outNEON.jpg"
#endif

#define TEST_DATA_LENGTH_BYTES (116536)
struct timeval tpstart,tpend;
long timeuse_us; 
void load_test_data(uint8_t * load_buf,char* load_path,unsigned int load_length_bytes)
{
    FILE * filePtr;
    filePtr=fopen(load_path,"rb");
    fread(load_buf,load_length_bytes,sizeof(uint8_t),filePtr);
    fclose(filePtr);
}
void store_test_data(uint8_t * store_buf,char* store_path,unsigned int store_length_bytes)
{
    FILE * filePtr;
    filePtr=fopen(store_path,"wb");
    fwrite(store_buf,store_length_bytes,sizeof(uint8_t),filePtr);
    fclose(filePtr);
}
void ARM_Load_Store_NO_Operation(uint8_t * src, uint8_t *dst, unsigned int size_bytes)
{
    unsigned int i=0;
    for(i=0;i<size_bytes;i++)
    {
        *(dst++) = *(src++);
    }
}
void Neon_Load_Store_NO_Operation(uint8_t * src, uint8_t *dst, unsigned int size_bytes)
{
#define SIMD_LENGTH_BYTES_THIS_FUNCTION (16)
#define SIMD_LENGTH_BYTES_THIS_FUNCTION_LOG2 (4) //In order To Shift In Binary
    // Make Sure Whether SIZE is aligned with SIMD_LENGTH
    unsigned int simd_loops = size_bytes >> SIMD_LENGTH_BYTES_THIS_FUNCTION_LOG2;
    unsigned int non_simd_size = size_bytes-(simd_loops << SIMD_LENGTH_BYTES_THIS_FUNCTION_LOG2);
    // unsigned int load_store_index = 0;
    // DO SIMD LOOPS
    for(unsigned int i=0;i<size_bytes;i+=16)
    {
        // uint8x16x4_t load_vec= vld4q_u8(src+i);//Load 16 bytes into 1 128-bit Vector Data Register
        vst1q_u8(dst,vld1q_u8(src));//Store 1 128-bit Vector Data Register into Physical Address
        dst+=16;
        src+=16;
        // load_store_index+=16; // LOAD_STORE ID SHIFT 16 BYTES
    }
    // DO NON SIMD LOOPS
    if(!(non_simd_size))  ARM_Load_Store_NO_Operation(src-16,dst-16,non_simd_size);
#undef SIMD_LENGTH_BYTES_THIS_FUNCTION
#undef SIMD_LENGTH_BYTES_THIS_FUNCTION_LOG2
}
#define TEST_TIMES_REPLICATE 1024
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
    unsigned char index_low[8]  ={0,8,1,8,2,8,3,8};
    unsigned char index_high[8] ={4,8,5,8,6,8,7,8};
    unsigned char index_R1[8]   ={0,3,6,8,8,8,8,8};
    unsigned char index_R2[8]   ={8,8,8,1,4,7,8,8};
    unsigned char index_R3[8]   ={8,8,8,8,8,8,2,5};
    unsigned char index_G1[8]   ={1,4,7,8,8,8,8,8};
    unsigned char index_G2[8]   ={8,8,8,2,5,8,8,8};
    unsigned char index_G3[8]   ={8,8,8,8,8,0,3,6};
    unsigned char index_B1[8]   ={2,5,8,8,8,8,8,8};
    unsigned char index_B2[8]   ={8,8,0,3,6,8,8,8};
    unsigned char index_B3[8]   ={8,8,8,8,8,1,4,7};
    unsigned char index_cmp2[8] ={0,3,6,8,8,8,8,8};
    unsigned char index_cmp3[8] ={0,3,6,8,8,8,8,8};
    unsigned char index_cmb1[8] ={0,0,0,2,2,2,4,4};
    unsigned char index_cmb2[8] ={4,6,6,6,8,8,8,8};
    unsigned char index_cmb3[8] ={8,8,8,8,0,0,0,2};
    unsigned char index_cmb4[8] ={2,2,4,4,4,6,6,6};
void do_RGB2GRAY_I16_Neon(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (76)
    #define I16_WEIGHT_GREEN_LIGHT    (150)
    #define I16_WEIGHT_BLUE_LIGHT     (30)
    #define NEON_D_SIMD_LENGTH_BYTES  (8)
    uint8x8_t v_index_low       = vld1_u8(index_low);
    uint8x8_t v_index_high      = vld1_u8(index_high);
    uint8x8_t v_index_r1        = vld1_u8(index_R1);
    uint8x8_t v_index_r2        = vld1_u8(index_R2);
    uint8x8_t v_index_r3        = vld1_u8(index_R3);
    uint8x8_t v_index_g1        = vld1_u8(index_G1);
    uint8x8_t v_index_g2        = vld1_u8(index_G2);
    uint8x8_t v_index_g3        = vld1_u8(index_G3);
    uint8x8_t v_index_b1        = vld1_u8(index_B1);
    uint8x8_t v_index_b2        = vld1_u8(index_B2);
    uint8x8_t v_index_b3        = vld1_u8(index_B3);
    uint8x8_t v_index_cmb1      = vld1_u8(index_cmb1);
    uint8x8_t v_index_cmb2      = vld1_u8(index_cmb2);
    uint8x8_t v_index_cmb3      = vld1_u8(index_cmb3);
    uint8x8_t v_index_cmb4      = vld1_u8(index_cmb4);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width*3;j+=NEON_D_SIMD_LENGTH_BYTES*3)
        {
            uint8x8_t  v_load1     = vld1_u8(src+j);
            uint8x8_t  v_load2     = vld1_u8(src+j+8);
            uint8x8_t  v_load3     = vld1_u8(src+j+16);
            uint8x8_t  v8_red_ch   = vtbl1_u8(v_load1,v_index_r1) | vtbl1_u8(v_load2,v_index_r2) | vtbl1_u8(v_load3,v_index_r3);
            uint8x8_t  v8_green_ch = vtbl1_u8(v_load1,v_index_g1) | vtbl1_u8(v_load2,v_index_g2) | vtbl1_u8(v_load3,v_index_g3);
            uint8x8_t  v8_blue_ch  = vtbl1_u8(v_load1,v_index_b1) | vtbl1_u8(v_load2,v_index_b2) | vtbl1_u8(v_load3,v_index_b3);
            uint16x4_t v16_red_l   = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_low));
            uint16x4_t v16_red_h   = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_high));
            uint16x4_t v16_green_l = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_low));
            uint16x4_t v16_green_h = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_high));
            uint16x4_t v16_blue_l  = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_low));
            uint16x4_t v16_blue_h  = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_high));
            uint16x4_t v16_gray_l  = (v16_red_l*I16_WEIGHT_RED_LIGHT+v16_green_l*I16_WEIGHT_GREEN_LIGHT+v16_blue_l*I16_WEIGHT_BLUE_LIGHT)>>8;
            uint16x4_t v16_gray_h  = (v16_red_h*I16_WEIGHT_RED_LIGHT+v16_green_h*I16_WEIGHT_GREEN_LIGHT+v16_blue_h*I16_WEIGHT_BLUE_LIGHT)>>8;
            uint8x8_t  v8_gray_l   = vreinterpret_u8_u16(v16_gray_l);
            uint8x8_t  v8_gray_h   = vreinterpret_u8_u16(v16_gray_h);
            uint8x8_t  v8_dst_1    = vtbl1_u8(v8_gray_l,v_index_cmb1);
            uint8x8_t  v8_dst_2    = vtbl1_u8(v8_gray_l,v_index_cmb2) | vtbl1_u8(v8_gray_h,v_index_cmb3);
            uint8x8_t  v8_dst_3    = vtbl1_u8(v8_gray_h,v_index_cmb4);
            vst1_u8(dst+j,v8_dst_1);
            vst1_u8(dst+j+8,v8_dst_2);
            vst1_u8(dst+j+16,v8_dst_3);
        }
        src+=width*3;
        dst+=width*3;
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
}
void do_RGB2GRAY_I16_NeonVTBL3(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (76)
    #define I16_WEIGHT_GREEN_LIGHT    (150)
    #define I16_WEIGHT_BLUE_LIGHT     (30)
    #define NEON_D_SIMD_LENGTH_BYTES  (8)
    uint8x8_t v_index_low       = vld1_u8(index_low);
    uint8x8_t v_index_high      = vld1_u8(index_high);
    uint8x8_t v_index_r1        = vld1_u8(index_R1);
    uint8x8_t v_index_r2        = vld1_u8(index_R2);
    uint8x8_t v_index_r3        = vld1_u8(index_R3);
    uint8x8_t v_index_g1        = vld1_u8(index_G1);
    uint8x8_t v_index_g2        = vld1_u8(index_G2);
    uint8x8_t v_index_g3        = vld1_u8(index_G3);
    uint8x8_t v_index_b1        = vld1_u8(index_B1);
    uint8x8_t v_index_b2        = vld1_u8(index_B2);
    uint8x8_t v_index_b3        = vld1_u8(index_B3);
    uint8x8_t v_index_cmb1      = vld1_u8(index_cmb1);
    uint8x8_t v_index_cmb2      = vld1_u8(index_cmb2);
    uint8x8_t v_index_cmb3      = vld1_u8(index_cmb3);
    uint8x8_t v_index_cmb4      = vld1_u8(index_cmb4);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width*3;j+=NEON_D_SIMD_LENGTH_BYTES*3)
        {
            uint8x8_t  v_load1          = vld1_u8(src+j);
            uint8x8_t  v_load2          = vld1_u8(src+j+8);
            uint8x8_t  v_load3          = vld1_u8(src+j+16);
            uint8x8_t  v8_red_ch        = vtbl1_u8(v_load1,v_index_r1) | vtbl1_u8(v_load2,v_index_r2) | vtbl1_u8(v_load3,v_index_r3);
            uint8x8_t  v8_green_ch      = vtbl1_u8(v_load1,v_index_g1) | vtbl1_u8(v_load2,v_index_g2) | vtbl1_u8(v_load3,v_index_g3);
            uint8x8_t  v8_blue_ch       = vtbl1_u8(v_load1,v_index_b1) | vtbl1_u8(v_load2,v_index_b2) | vtbl1_u8(v_load3,v_index_b3);
            uint16x4_t v16_red_l        = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_low));
            uint16x4_t v16_red_h        = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_high));
            uint16x4_t v16_green_l      = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_low));
            uint16x4_t v16_green_h      = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_high));
            uint16x4_t v16_blue_l       = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_low));
            uint16x4_t v16_blue_h       = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_high));
            uint16x8_t v16_red_cmb      = vcombine_u16(v16_red_l,v16_red_h);
            uint16x8_t v16_green_cmb    = vcombine_u16(v16_green_l,v16_green_h);
            uint16x8_t v16_blue_cmb     = vcombine_u16(v16_blue_l,v16_blue_h);
            uint16x8_t v16_gray         = (v16_red_cmb*I16_WEIGHT_RED_LIGHT+v16_green_cmb*I16_WEIGHT_GREEN_LIGHT+v16_blue_cmb*I16_WEIGHT_BLUE_LIGHT)>>8;
            uint8x8_t  v8_gray_l        = vreinterpret_u8_u16(vget_low_u16(v16_gray));
            uint8x8_t  v8_gray_h        = vreinterpret_u8_u16(vget_high_u16(v16_gray));
            uint8x8_t  v8_dst_1         = vtbl1_u8(v8_gray_l,v_index_cmb1);
            uint8x8_t  v8_dst_2         = vtbl1_u8(v8_gray_l,v_index_cmb2) | vtbl1_u8(v8_gray_h,v_index_cmb3);
            uint8x8_t  v8_dst_3         = vtbl1_u8(v8_gray_h,v_index_cmb4);
            vst1_u8(dst+j,v8_dst_1);
            vst1_u8(dst+j+8,v8_dst_2);
            vst1_u8(dst+j+16,v8_dst_3);
        }
        src+=width*3;
        dst+=width*3;
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
}
#endif
#define MAX_LOAD_LOCAL_BYTES (1024*1024)
void do_neon_test()
{
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)<<BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes);
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_NeonVTBL3(testRGB888,dst,read_width,read_height);
    #else
    do_RGB2GRAY_I16(testRGB888,dst,read_width,read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN NEON OP Latency = %lu US\n",timeuse_us/times++);
    // memcpy(dst,testRGB888,read_width*read_height*read_pixel_bytes);
    save_RawRGB_bmpFile(TEST_OUT_NEON_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
}
void do_arm_test()
{
    // #undef _USE_ARM_NEON_OPT_
    struct timeval tpstart,tpend;
    static long timeuse_us=0,times=1;
    unsigned char * testRGB888 = malloc(MAX_LOAD_LOCAL_BYTES);
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    readBMP(TEST_DATA_PATH,&bmpFileHeader,&bmpInfoHeader,testRGB888);
    unsigned int read_width       = bmpInfoHeader.bmpInfo_width;
    unsigned int read_height      = bmpInfoHeader.bmpInfo_height;
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)<<BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    gettimeofday(&tpstart,NULL);  
    // #ifdef  _USE_ARM_NEON_OPT_
    // do_RGB2GRAY_I16_NeonVTBL3(testRGB888,dst,read_width,read_height);
    // #else
    do_RGB2GRAY_I16(testRGB888,dst,read_width,read_height);
    // #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM  OP Latency = %lu US\n",timeuse_us/times++);
    // memcpy(dst,testRGB888,read_width*read_height*read_pixel_bytes);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    // #define _USE_ARM_NEON_OPT_
}
int main()
{
    for(int i=0;i<50;i++){
        do_neon_test();
        do_arm_test();
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
void compare_cpu_neon_result(unsigned char * dst_cpu,unsigned char * dst_neon,unsigned int size_bytes)
{
    
}