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
void generateTable(unsigned char * table,unsigned int size)
{
    for(unsigned int i=0;i<size ;i++)
    {
        table[i] = (uint8_t)round((pow(i/255.f, 1/2.2)*255.f));
        // printf("table[%3d]=%3d\n",i,table[i]);
    }
}
void GammaCorrection(unsigned char *src,unsigned char *dst,unsigned int width,unsigned char * table)
{
    for(unsigned int i=0;i<width;i++)
    {
        dst[i] = table[src[i]];
    }
}
unsigned char step32_array[8]     ={32,32,32,32,32,32,32,32};
unsigned char mask_init_array[8]  ={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char array_const31[8]    ={31,31,31,31,31,31,31,31};
unsigned char array_ones[8]       ={1,1,1,1,1,1,1,1}; 
void test_vload()
{
    #define TEST_ELEM_BYTES  (24)
    #define SIMD_Q_LEN_BYTES (16)
    #define SIMD_D_LEN_BYTES (8)
    uint8_t      dst_array[8];
    uint8_t      src_array[24]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
    uint8_t      index_array[8]={4,7,5,2,10,28,5,32};
    uint8x8_t    v8_result    = vtbl3_u8(vld3_u8(src_array),vld1_u8(index_array));
    vst1_u8(dst_array,v8_result);
    for(int i=0;i<8;i++)
    {
        printf("dst[%3d] = %3d\n",i,dst_array[i]);
    }
}
void test()
{
    #define TEST_ELEM_BYTES  (256)
    #define SIMD_Q_LEN_BYTES (16)
    #define SIMD_D_LEN_BYTES (8)
    uint8_t BGR888_src[TEST_ELEM_BYTES]={1,2,3,4,5,6,7,8,9,10,11,12,13,
                        14,15,16,17,18,19,20,21,22,23,24};
    uint16_t BGRhex3_dst[TEST_ELEM_BYTES];
    uint8_t adder_array[SIMD_D_LEN_BYTES]={240,240,240,240,240,240,240,240};
    uint8x8_t v8_adder = vld1_u8(adder_array); 
    for(int i=0;i<TEST_ELEM_BYTES;i+=SIMD_D_LEN_BYTES)
    {
        uint8x8_t  v8_load   = vld1_u8(BGR888_src+i);
        uint16x8_t v16_result = vaddl_u8(v8_load,v8_adder);
        vst1q_u16(&BGRhex3_dst[i],v16_result);
    }
    for(int i=0;i<TEST_ELEM_BYTES;i++)
    {
        printf("src[%3d] = %3d; dst[%3d] = %3d\n",i,BGR888_src[i],i,BGRhex3_dst[i]);
    }
}
void GammaCorrection_Neon(unsigned char *src,unsigned char *dst,unsigned int width,unsigned char * table)
{
    //load table into vector 256 level needs 32x8 8*vec4
    uint8x8x4_t v8_tbl[8];
    for(unsigned int i=0;i<8;i++)
    {
        v8_tbl[i].val[0] = vld1_u8(table+32*i);
        v8_tbl[i].val[1] = vld1_u8(table+32*i+8);
        v8_tbl[i].val[2] = vld1_u8(table+32*i+16);
        v8_tbl[i].val[3] = vld1_u8(table+32*i+24);
    }
    uint8x8_t v_result = vcreate_u8(0x00000000);
    uint8x8_t v_step32 = vld1_u8(step32_array);
    uint8x8_t v_step31 = vld1_u8(array_const31);
    uint8x8_t v_mask   = vld1_u8(mask_init_array);
    uint8x8_t v_ones   = vld1_u8(array_ones);
    for(unsigned int i=0;i<width;i+=8)
    {
        uint8x8_t v_index  =  vld1_u8(src+i);
        for(unsigned int k=0;k<8;k++)
        {
            
            v_result &= vtbl4_u8(v8_tbl[k],v_index);
            v_index   = vsub_u8(v_index,v_step32);
            // v_mask   = vmax_u8(v_index,v_step32) ;
            // v_mask   = 255*vmax_u8((~(255*vsub_u8(v_mask,v_step31))&v_mask),v_ones);
            // v_mask   = v_mask & v_result;
            // v_index  = vsub_u8(v_index,v_step32);
        }
        vst1_u8(dst+i,v_result);
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
    do_RGB2GRAY_I16_NeonVTBL3(testRGB888, dst,read_width, read_height);
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
void do_arm_rgb2gray()
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
    do_RGB2GRAY_I16_NeonVTBL3(testRGB888, dst,read_width, read_height);
    #else
    do_RGB2GRAY_I16(testRGB888, dst,read_width, read_height);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM  OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(testRGB888);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
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
    unsigned char *GammaTBL= (unsigned char*)malloc(TABLE_SIZE);
    gettimeofday(&tpstart,NULL);  
    generateTable(GammaTBL,TABLE_SIZE);
    #ifdef  _USE_ARM_NEON_OPT_
    generateTable(GammaTBL,TABLE_SIZE);
    GammaCorrection_Neon(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #else
    generateTable(GammaTBL,TABLE_SIZE);
    GammaCorrection(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN NEON OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_NEON_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(GammaTBL);
    free(testRGB888);
    free_bmpRes();
}
void do_arm_test()
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
    unsigned char *GammaTBL= (unsigned char*)malloc(TABLE_SIZE);
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    generateTable(GammaTBL,TABLE_SIZE);
    GammaCorrection(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #else
    generateTable(GammaTBL,TABLE_SIZE);
    GammaCorrection(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM  OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free(GammaTBL);
    free(testRGB888);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
}
int main()
{
    // for(int i=0;i<128;i++){
    //     // do_neon_test();
    //     // do_arm_test();
    //     do_neon_rgb2gray();
    //     do_arm_rgb2gray();
    // }
    // test();
    test_vload();
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