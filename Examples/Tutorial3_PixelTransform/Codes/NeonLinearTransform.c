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
uint8x8x4_t vTable[TABLE_SIZE>>5];
uint8_t GammaTBL[TABLE_SIZE];
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
#ifdef  _USE_ARM_NEON_OPT_
void generateTable_NeonVec(uint8x8x4_t vTables[],unsigned int size)
{
    uint8_t * pTable     = (uint8_t*)malloc(size);
    for(unsigned int i=0;i<size ;i++)
    {
        pTable[i] = (uint8_t)round((pow(i/255.f, 1/2.2)*255.f));
    }
    unsigned int vTable_nb = size >> 5;
    uint8_t * load_table_ptr=&pTable[0];
    for(unsigned int id=0;id<vTable_nb;id++)
    {
        vTables[id].val[0] = vld1_u8(load_table_ptr);
        vTables[id].val[1] = vld1_u8(load_table_ptr+8);
        vTables[id].val[2] = vld1_u8(load_table_ptr+16);
        vTables[id].val[3] = vld1_u8(load_table_ptr+24);
        load_table_ptr+=32;
    }
    free(pTable);
}
void GammaCorrection_NeonAlign8(unsigned char * src,unsigned char * dst,unsigned int width,uint8x8x4_t vTables[],unsigned int vTable_nb)
{
    uint8x8_t v_gap = vcreate_u8(0x2020202020202020);// |32|32|32|32|32|32|32|32|
    for(unsigned int i=0;i<width;i+=16)
    {
        uint8x16_t v_index       = vld1q_u8(src+i);
        uint8x8_t  v_index_low   = vget_low_u8(v_index);
        uint8x8_t  v_index_high  = vget_high_u8(v_index);
        uint8x8_t  v_result_low  = vtbl4_u8(vTables[0],v_index_low);
        uint8x8_t  v_result_high = vtbl4_u8(vTables[0],v_index_high);
        for(unsigned int id=1;id<vTable_nb;id++)
        {
            v_index_low  -=v_gap;
            v_index_high -=v_gap;
            v_result_low  |= vtbl4_u8(vTables[id],v_index_low);
            v_result_high |= vtbl4_u8(vTables[id],v_index_high);
        }
        vst1q_u8(dst+i,vcombine_u8(v_result_low,v_result_high));
    }
}
#endif
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
#define TEST_TIMES_REPLICATE 1024
void logMenu();
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
    #ifdef  _USE_ARM_NEON_OPT_
    GammaCorrection_NeonAlign8(testRGB888,dst,read_width*read_height*read_pixel_bytes,vTable,TABLE_SIZE>>5);
    #else
    GammaCorrection(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN NEON OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_NEON_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
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
    gettimeofday(&tpstart,NULL);  
    #ifdef  _USE_ARM_NEON_OPT_
    GammaCorrection_NeonAlign8(testRGB888,dst,read_width*read_height*read_pixel_bytes,vTable,TABLE_SIZE>>5);
    #else
    GammaCorrection(testRGB888,dst,read_width*read_height*read_pixel_bytes,GammaTBL);
    #endif
    gettimeofday(&tpend,NULL); 
    timeuse_us+=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("MEAN ARM  OP Latency = %lu US\n",timeuse_us/times++);
    save_RawRGB_bmpFile(TEST_OUT_ARM_DATA_PATH,dst,read_width*read_height*read_pixel_bytes,read_width,read_height);
    free(dst);
    free_bmpRes();
    #define _USE_ARM_NEON_OPT_
}
int main()
{
    generateTable_NeonVec(vTable,TABLE_SIZE);
    generateTable(GammaTBL,TABLE_SIZE);
    for(int i=0;i<128;i++){
        do_neon_test();
        do_arm_test();
    }
    // test();
    // test_vload();
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