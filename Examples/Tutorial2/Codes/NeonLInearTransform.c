#include <stdio.h>
#include <string.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <arm_neon.h>
#include <bmp.h>
#include <math.h>
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
#define TABLE_SIZE (256)
unsigned char GammaTBL[TABLE_SIZE];
void generateTable(unsigned char * table,unsigned int size)
{
    for(unsigned int i=0;i<size ;i++)
    {
        table[i] = (uint8_t)round((pow(i/255.f, 1/2.2)*255.f));
        printf("table[%3d]=%3d\n",i,table[i]);
    }
}
void generateTableTalyer(unsigned char * table,unsigned int size)
{
    static int first=1;
    for(unsigned int i=0;i<size && first;i++)
    {
        table[i] = (uint8_t)round((pow(i/255.f, 1/2.2)*255.f));
        printf("table[%3d]=%3d\n",i,table[i]);
    }
    first=0;
}
void GammaCorrection(unsigned char *src,unsigned int width,unsigned char * table)
{
    for(unsigned int i=0;i<width;i++)
    {
        src[i] = table[src[i]];
    }
}
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
    #undef NEON_D_SIMD_LENGTH_BYTES
}
void do_RGB2GRAY_I16_NeonVTBL3(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (76)
    #define I16_WEIGHT_GREEN_LIGHT    (150)
    #define I16_WEIGHT_BLUE_LIGHT     (30)
    #define NEON_D_SIMD_LENGTH_BYTES  (8)
    uint8x8_t v_index_low       = vld1_u8(index_low);
    uint8x8_t v_index_high      = vld1_u8(index_high);
    uint8x8_t v_index_cmb1      = vld1_u8(index_cmb1);
    uint8x8_t v_index_cmb2      = vld1_u8(index_cmb2);
    uint8x8_t v_index_cmb3      = vld1_u8(index_cmb3);
    uint8x8_t v_index_cmb4      = vld1_u8(index_cmb4);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width*3;j+=NEON_D_SIMD_LENGTH_BYTES*6)
        {
            uint8x16x3_t v_load_rgb      = vld3q_u8(src+j);
            uint8x8_t v_r=vget_low_u8(v_load_rgb.val[0]);
            uint8x8_t v_g=vget_low_u8(v_load_rgb.val[1]);
            uint8x8_t v_b=vget_low_u8(v_load_rgb.val[2]);
            uint16x4_t v16_red_l   = vreinterpret_u16_u8(vtbl1_u8(v_r,v_index_low));
            uint16x4_t v16_red_h   = vreinterpret_u16_u8(vtbl1_u8(v_r,v_index_high));
            uint16x4_t v16_green_l = vreinterpret_u16_u8(vtbl1_u8(v_g,v_index_low));
            uint16x4_t v16_green_h = vreinterpret_u16_u8(vtbl1_u8(v_g,v_index_high));
            uint16x4_t v16_blue_l  = vreinterpret_u16_u8(vtbl1_u8(v_b,v_index_low));
            uint16x4_t v16_blue_h  = vreinterpret_u16_u8(vtbl1_u8(v_b,v_index_high));
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
            v_r=vget_high_u8(v_load_rgb.val[0]);
            v_g=vget_high_u8(v_load_rgb.val[1]);
            v_b=vget_high_u8(v_load_rgb.val[2]);
            v16_red_l   = vreinterpret_u16_u8(vtbl1_u8(v_r,v_index_low));
            v16_red_h   = vreinterpret_u16_u8(vtbl1_u8(v_r,v_index_high));
            v16_green_l = vreinterpret_u16_u8(vtbl1_u8(v_g,v_index_low));
            v16_green_h = vreinterpret_u16_u8(vtbl1_u8(v_g,v_index_high));
            v16_blue_l  = vreinterpret_u16_u8(vtbl1_u8(v_b,v_index_low));
            v16_blue_h  = vreinterpret_u16_u8(vtbl1_u8(v_b,v_index_high));
            v16_gray_l  = (v16_red_l*I16_WEIGHT_RED_LIGHT+v16_green_l*I16_WEIGHT_GREEN_LIGHT+v16_blue_l*I16_WEIGHT_BLUE_LIGHT)>>8;
            v16_gray_h  = (v16_red_h*I16_WEIGHT_RED_LIGHT+v16_green_h*I16_WEIGHT_GREEN_LIGHT+v16_blue_h*I16_WEIGHT_BLUE_LIGHT)>>8;
            v8_gray_l   = vreinterpret_u8_u16(v16_gray_l);
            v8_gray_h   = vreinterpret_u8_u16(v16_gray_h);
            v8_dst_1    = vtbl1_u8(v8_gray_l,v_index_cmb1);
            v8_dst_2    = vtbl1_u8(v8_gray_l,v_index_cmb2) | vtbl1_u8(v8_gray_h,v_index_cmb3);
            v8_dst_3    = vtbl1_u8(v8_gray_h,v_index_cmb4);
            vst1_u8(dst+j+24,v8_dst_1);
            vst1_u8(dst+j+32,v8_dst_2);
            vst1_u8(dst+j+40,v8_dst_3);
        }
        src+=width*3;
        dst+=width*3;
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
    #undef NEON_D_SIMD_LENGTH_BYTES
}
void do_RGB2GRAY_I16_NeonOP_Reg(unsigned char *src, unsigned char * dst, unsigned int width, unsigned int height)
{
    #define I16_WEIGHT_RED_LIGHT      (76)
    #define I16_WEIGHT_GREEN_LIGHT    (150)
    #define I16_WEIGHT_BLUE_LIGHT     (30)
    #define NEON_D_SIMD_LENGTH_BYTES  (8)
    uint8x8_t v_uchar8[26];
    uint16x4_t v_ushort4[2];
    uint16x8_t v_ushort8[3];
    v_uchar8[3]= vld1_u8(index_R1);
    v_uchar8[4]= vld1_u8(index_R2);
    v_uchar8[5]= vld1_u8(index_R3);
    v_uchar8[9]= vld1_u8(index_G1);
    v_uchar8[10]= vld1_u8(index_G2);
    v_uchar8[11]= vld1_u8(index_G3); 
    v_uchar8[12]= vld1_u8(index_B1);
    v_uchar8[13]= vld1_u8(index_B2);
    v_uchar8[14]= vld1_u8(index_B3);  
    v_uchar8[15]= vld1_u8(index_low);
    v_uchar8[16]= vld1_u8(index_high);
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width*3;j+=NEON_D_SIMD_LENGTH_BYTES*3)
        {
            v_uchar8[0]=vld1_u8(src+j);// uint8x8_t  v_load1          = vld1_u8(src+j);
            v_uchar8[1]=vld1_u8(src+j+8);// uint8x8_t  v_load2          = vld1_u8(src+j+8);
            v_uchar8[2]=vld1_u8(src+j+16);// uint8x8_t  v_load3          = vld1_u8(src+j+16);
            v_uchar8[6]= vtbl1_u8(v_uchar8[0],v_uchar8[3]) | vtbl1_u8(v_uchar8[1],v_uchar8[4]) | vtbl1_u8(v_uchar8[2],v_uchar8[5]);//uint8x8_t  v8_red_ch        = vtbl1_u8(v_load1,v_index_r1) | vtbl1_u8(v_load2,v_index_r2) | vtbl1_u8(v_load3,v_index_r3);
            v_uchar8[7]= vtbl1_u8(v_uchar8[0],v_uchar8[9]) | vtbl1_u8(v_uchar8[1],v_uchar8[10]) | vtbl1_u8(v_uchar8[2],v_uchar8[11]);// uint8x8_t  v8_green_ch      = vtbl1_u8(v_load1,v_index_g1) | vtbl1_u8(v_load2,v_index_g2) | vtbl1_u8(v_load3,v_index_g3);
            v_uchar8[8]= vtbl1_u8(v_uchar8[0],v_uchar8[12]) | vtbl1_u8(v_uchar8[1],v_uchar8[13]) | vtbl1_u8(v_uchar8[2],v_uchar8[14]);// uint8x8_t  v8_blue_ch       = vtbl1_u8(v_load1,v_index_b1) | vtbl1_u8(v_load2,v_index_b2) | vtbl1_u8(v_load3,v_index_b3);
            v_ushort4[0]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[6],v_uchar8[15]));//uint16x4_t v16_red_l        = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_low));
            v_ushort4[1]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[6],v_uchar8[16]));//uint16x4_t v16_red_h        = vreinterpret_u16_u8(vtbl1_u8(v8_red_ch,v_index_high));
            v_ushort8[0]= vcombine_u16(v_ushort4[0],v_ushort4[1]);//uint16x8_t v16_red_cmb      = vcombine_u16(v16_red_l,v16_red_h);
            v_ushort4[0]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[7],v_uchar8[15]));//uint16x4_t v16_green_l      = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_low));
            v_ushort4[1]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[7],v_uchar8[16]));//uint16x4_t v16_green_h      = vreinterpret_u16_u8(vtbl1_u8(v8_green_ch,v_index_high));
            v_ushort8[1]= vcombine_u16(v_ushort4[0],v_ushort4[1]);//uint16x8_t v16_green_cmb    = vcombine_u16(v16_green_l,v16_green_h);
            v_ushort4[0]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[8],v_uchar8[15]));//uint16x4_t v16_blue_l       = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_low));
            v_ushort4[1]= vreinterpret_u16_u8(vtbl1_u8(v_uchar8[8],v_uchar8[16]));//uint16x4_t v16_blue_h       = vreinterpret_u16_u8(vtbl1_u8(v8_blue_ch,v_index_high));
            v_ushort8[2]= vcombine_u16(v_ushort4[0],v_ushort4[1]);//uint16x8_t v16_blue_cmb     = vcombine_u16(v16_blue_l,v16_blue_h);
            v_ushort8[0]= (v_ushort8[0]*I16_WEIGHT_RED_LIGHT+v_ushort8[1]*I16_WEIGHT_GREEN_LIGHT+v_ushort8[2]*I16_WEIGHT_BLUE_LIGHT)>>8;//uint16x8_t v16_gray         = (v16_red_cmb*I16_WEIGHT_RED_LIGHT+v16_green_cmb*I16_WEIGHT_GREEN_LIGHT+v16_blue_cmb*I16_WEIGHT_BLUE_LIGHT)>>8;
            v_uchar8[0] = vreinterpret_u8_u16(vget_low_u16(v_ushort8[0]));//uint8x8_t  v8_gray_l        = vreinterpret_u8_u16(vget_low_u16(v16_gray));
            v_uchar8[2] = vld1_u8(index_cmb1);
            v_uchar8[6] = vtbl1_u8(v_uchar8[0],v_uchar8[2]);//uint8x8_t  v8_dst_1         = vtbl1_u8(v8_gray_l,v_index_cmb1);
            vst1_u8(dst+j,v_uchar8[6]);//vst1_u8(dst+j,v8_dst_1);
            v_uchar8[2] = vld1_u8(index_cmb2);
            v_uchar8[6] = vld1_u8(index_cmb3);
            v_uchar8[1] = vreinterpret_u8_u16(vget_high_u16(v_ushort8[0]));//uint8x8_t  v8_gray_h        = vreinterpret_u8_u16(vget_high_u16(v16_gray));
            v_uchar8[7] = vtbl1_u8(v_uchar8[0],v_uchar8[2]) | vtbl1_u8(v_uchar8[1],v_uchar8[6]);//uint8x8_t  v8_dst_2         = vtbl1_u8(v8_gray_l,v_index_cmb2) | vtbl1_u8(v8_gray_h,v_index_cmb3);
            vst1_u8(dst+j+8,v_uchar8[7]);//vst1_u8(dst+j+8,v8_dst_2);
            v_uchar8[2] = vld1_u8(index_cmb4);
            v_uchar8[8] = vtbl1_u8(v_uchar8[1],v_uchar8[2]);//uint8x8_t  v8_dst_3         = vtbl1_u8(v8_gray_h,v_index_cmb4);
            vst1_u8(dst+j+16,v_uchar8[8]);//vst1_u8(dst+j+16,v8_dst_3);
        }
        src+=width*3;
        dst+=width*3;
    }
    #undef I16_WEIGHT_RED_LIGHT
    #undef I16_WEIGHT_GREEN_LIGHT
    #undef I16_WEIGHT_BLUE_LIGHT
    #undef NEON_D_SIMD_LENGTH_BYTES
}
#endif
#define MAX_LOAD_LOCAL_BYTES (1024*800)
void log_result(unsigned char * data_p,unsigned int start,unsigned int end, unsigned int step,char * tag)
{
    for(unsigned int i=start;i<end;i+=step)
    {
        printf("[%s]: data[%3d]=%3d\n",tag,i,data_p[i]);
    }
}
void log_result2(unsigned char * data_p1,unsigned char * data_p2,unsigned int start,unsigned int end, unsigned int step,char * tag)
{
    for(unsigned int i=start;i<end;i+=step)
    {
        printf("[%s]: data1[%3d]=%3d ; data2[%3d]=%3d\n",tag,i,data_p1[i],i,data_p2[i]);
    }
}
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
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes);
    gettimeofday(&tpstart,NULL);  
    generateTable(GammaTBL,TABLE_SIZE);
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_Neon(testRGB888,dst,read_width,read_height);
    #else
    do_RGB2GRAY_I16(testRGB888,dst,read_width,read_height);
    #endif
    memcpy(dst,testRGB888,read_width*read_height*read_pixel_bytes);
    GammaCorrection(dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    gettimeofday(&tpend,NULL); 
    // log_result(dst,1000,50000,1500,"Neon");
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN NEON OP Latency = %lu US\n",timeuse_us/times++);
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
    unsigned int read_pixel_bytes = (bmpInfoHeader.bmpInfo_bitcount)>>BITCOUNT_TO_BYTE_SHIFT;
    unsigned char * dst = (unsigned char*)malloc(read_width*read_height*read_pixel_bytes); 
    gettimeofday(&tpstart,NULL);  
    generateTable(GammaTBL,TABLE_SIZE);
    #ifdef  _USE_ARM_NEON_OPT_
    do_RGB2GRAY_I16_NeonVTBL3(testRGB888,dst,read_width,read_height);
    #else
    do_RGB2GRAY_I16_NeonVTBL3(testRGB888,dst,read_width,read_height);
    #endif
    memcpy(dst,testRGB888,read_width*read_height*read_pixel_bytes);
    GammaCorrection(dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    gettimeofday(&tpend,NULL); 
    // log_result(dst,1000,50000,1500,"Arm");
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM  OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    // #define _USE_ARM_NEON_OPT_
}
void test()
{
    unsigned char table[32];
    unsigned char index[8]={0,1,2,3,4,5,6,7};
    unsigned char result[8];
    for(int i=0;i<32;i+=1)
    {
        table[i] = i;
    }
    uint8x8x4_t v_table = vld4_u8(table);
    // uint8x8_t v_index=vld1_u8(index);
    // uint8x8_t v_result = vtbl4_u8(v_table,v_index);
    vst1_u8(result,v_table.val[0]);
    for(int i=0;i<8;i++)
    {
        printf("result[0][%d]=%d\n",i,result[i]);
    }
    vst1_u8(result,v_table.val[1]);
    for(int i=0;i<8;i++)
    {
        printf("result[1][%d]=%d\n",i,result[i]);
    }
    vst1_u8(result,v_table.val[2]);
    for(int i=0;i<8;i++)
    {
        printf("result[2][%d]=%d\n",i,result[i]);
    }
    vst1_u8(result,v_table.val[3]);
    for(int i=0;i<8;i++)
    {
        printf("result[3][%d]=%d\n",i,result[i]);
    }
    for(int i=0;i<32;i++)
    {
        printf("table[%d]=%d\n",i,table[i]);
    }
    
}
// unsigned char input[256];
// unsigned char output[256];
// void gammaTest()
// {
//    generateTable(GammaTBL,TABLE_SIZE);
//    for(unsigned int i=0;i<256;i++)
//    {
//        input[i]=i;
//        output[i]=i;
//    }
//    GammaCorrection(output,256,GammaTBL);
//    log_result2(input,output,0,256,1,"Gamma");
// }
int main()
{
    // gammaTest();
    // test();
    for(int i=0;i<1;i++){
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