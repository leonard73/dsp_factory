#include <stdio.h>
#include <string.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <arm_neon.h>
#define TEST_DATA_PATH "tutorial1_data.jpg"
#define TEST_OUT_ARM_DATA_PATH "tutorial1_data_outArm.jpg"
#define TEST_OUT_NEON_DATA_PATH "tutorial1_data_outNEON.jpg"
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
#define TEST_TIMES_REPLICATE 10240
int main()
{
    uint8_t * load_buf = (uint8_t*) malloc(TEST_DATA_LENGTH_BYTES);
    uint8_t * store_buf_neon = (uint8_t*) malloc(TEST_DATA_LENGTH_BYTES);
    uint8_t * store_buf_arm = (uint8_t*) malloc(TEST_DATA_LENGTH_BYTES);
    load_test_data(load_buf,TEST_DATA_PATH,TEST_DATA_LENGTH_BYTES);
    //Test Neon MEM COPY 
    gettimeofday(&tpstart,NULL); 
    for(int i=0;i<TEST_TIMES_REPLICATE;i++)
        Neon_Load_Store_NO_Operation(load_buf,store_buf_neon,TEST_DATA_LENGTH_BYTES);
    gettimeofday(&tpend,NULL); 
    store_test_data(store_buf_neon,TEST_OUT_NEON_DATA_PATH,TEST_DATA_LENGTH_BYTES);
    timeuse_us=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("NEON MEMCPY TOTAL_TIME=%ld us; MOVE SIZE = %d times * %d bytes\n",timeuse_us,TEST_TIMES_REPLICATE,TEST_DATA_LENGTH_BYTES);
    //Test CPU MEM CPY
    gettimeofday(&tpstart,NULL); 
    for(int i=0;i<TEST_TIMES_REPLICATE;i++)
        ARM_Load_Store_NO_Operation (load_buf,store_buf_arm,TEST_DATA_LENGTH_BYTES);
    gettimeofday(&tpend,NULL); 
    store_test_data(store_buf_arm,TEST_OUT_ARM_DATA_PATH,TEST_DATA_LENGTH_BYTES);
    timeuse_us=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
    printf("ARM MEMCPY TOTAL_TIME=%ld us; MOVE SIZE = %d times * %d bytes\n",timeuse_us,TEST_TIMES_REPLICATE,TEST_DATA_LENGTH_BYTES);
    //Free Dynamic LOAD Memory
    free(load_buf);
    free(store_buf_arm);
    free(store_buf_neon);
    return 0;
}