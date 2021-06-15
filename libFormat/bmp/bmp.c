#include "bmp.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static unsigned char * bmpFile_memPtr;
static unsigned char * rawDataPtr;
void create_bmp_file(   
                        unsigned char * bmpFileBuf,
                        unsigned char * p_rawBGR,
                        unsigned int bitPerPixel,
                        unsigned int sizeRawData,
                        unsigned int width,
                        unsigned int height
){
    bitmap_file_header bmpFileHeader;
    bmpFileHeader.bmpFile_type=0x4D42;
    bmpFileHeader.bmpFile_size[0]=(BITMAP_HEADER_LENGTH + sizeRawData) & 0xFFFF;
    bmpFileHeader.bmpFile_size[1]=((BITMAP_HEADER_LENGTH + sizeRawData) >> 16) & 0xFFFF;
    bmpFileHeader.bmpFile_reversed[0]=0;
    bmpFileHeader.bmpFile_reversed[1]=0;
	bmpFileHeader.bmpFile_offbits[0] = (BITMAP_INFO_HEADER_LENGTH) & 0xFFFF;
	bmpFileHeader.bmpFile_offbits[1] = ((BITMAP_INFO_HEADER_LENGTH) >> 16) & 0xFFFF;
    bitmap_info_header bmpInfoHeader;
    bmpInfoHeader.bmpInfo_size=BITMAP_INFO_HEADER_LENGTH;
    bmpInfoHeader.bmpInfo_width=width;
    bmpInfoHeader.bmpInfo_height=height;
    bmpInfoHeader.bmpInfo_planes=1;
    bmpInfoHeader.bmpInfo_bitcount=bitPerPixel;
    bmpInfoHeader.bmpInfo_compression=0;
    bmpInfoHeader.bmpInfo_image_size=sizeRawData;
    bmpInfoHeader.bmpInfo_xpels_per_meter=0;
    bmpInfoHeader.bmpInfo_ypels_per_meter=0;
    bmpInfoHeader.bmpInfo_used_clr=0;
    bmpInfoHeader.bmpInfo_important_clr=0;
    memcpy(bmpFileBuf,&bmpFileHeader,sizeof(bmpFileHeader));
    bmpFileBuf+=sizeof(bmpFileHeader);
    memcpy(bmpFileBuf,&bmpInfoHeader,sizeof(bmpInfoHeader));
    bmpFileBuf+=sizeof(bmpInfoHeader);
    memcpy(bmpFileBuf,p_rawBGR,sizeRawData);
}
void cvt_RGB888_BGR888(unsigned char * p_dataRGB888,unsigned int pixelNB)
{
    for(int i=0;i<pixelNB*3;i+=3)
    {
        unsigned char temp = *(p_dataRGB888+i);
        *(p_dataRGB888+i) = *(p_dataRGB888+i+2);
        *(p_dataRGB888+i+2) = temp; 
    }
}
void REVERSE_RGB888(unsigned char * p_dataRGB888,unsigned int width,unsigned int height)
{
    unsigned char temp;
    for(int i=0;i<height/2;i++)
    {
        for(int j=0;j<width*3;j++){
        temp=*(p_dataRGB888+i*width*3+j);
        *(p_dataRGB888+i*width*3+j)=*(p_dataRGB888+(height-i-1)*width*3+j);
        *(p_dataRGB888+(height-i-1)*width*3+j)=temp;
        }
    }
}
unsigned char * create_bmpFile_rgb888(unsigned char * p_dataRGB888,unsigned int size,unsigned int width,unsigned int height)
{
    bmpFile_memPtr=(unsigned char*) malloc(size+BITMAP_HEADER_LENGTH);
    REVERSE_RGB888( p_dataRGB888,width,height);//for bmp stores reverse observe coordinate axis
    cvt_RGB888_BGR888( p_dataRGB888,width*height);
    create_bmp_file(bmpFile_memPtr,p_dataRGB888,24,size,width,height);
    return bmpFile_memPtr;
}
void free_bmpRes()
{
    if(bmpFile_memPtr)
        free(bmpFile_memPtr);
    if(rawDataPtr)
        free(rawDataPtr);
}
void save_RawRGB_bmpFile(char * savePath,unsigned char * p_dataRGB888,unsigned int size,unsigned int width,unsigned int height)
{
    unsigned char * saveBuf = create_bmpFile_rgb888(p_dataRGB888,size,width,height);
    FILE * fp = fopen(savePath,"wb");
    fwrite(saveBuf,size+BITMAP_HEADER_LENGTH,sizeof(unsigned char),fp);
    fclose(fp);
    free_bmpRes();
}
void loadFile(char * loadPath,unsigned int size,unsigned char * buf)
{
    FILE * fp = fopen(loadPath,"rb");
    fread(buf,size,sizeof(unsigned char),fp);
    fclose(fp);
}
void loadBmpFileHeaderPart(char * loadPath,bitmap_file_header * buf)
{
    FILE * fp = fopen(loadPath,"rb");
    fread(buf,1,sizeof(bitmap_file_header),fp);
    fclose(fp);
}
void loadBmpInfoHeaderPart(char * loadPath,bitmap_info_header * buf)
{
    FILE * fp = fopen(loadPath,"rb");
    fseek( fp, BITMAP_FILE_HEADER_LENGTH, SEEK_SET );
    fread(buf,1,sizeof(bitmap_info_header),fp);
    fclose(fp);
}
void loadBmpDataPart(char * loadPath,unsigned int size,unsigned char * buf)
{
    FILE * fp = fopen(loadPath,"rb");
    fseek( fp, BITMAP_HEADER_LENGTH, SEEK_SET );
    fread(buf,size,sizeof(unsigned char),fp);
    fclose(fp);
}
unsigned char* read_bmpFile_RawRGB(
						char * loadPath,
						unsigned int *bitsPerPixel,
						unsigned int *size,
						unsigned int*width,
						unsigned int*height,
                        bitmap_file_header *bmpFileHeader,
                        bitmap_info_header *bmpInfoHeader
){
    
    loadBmpFileHeaderPart(loadPath,bmpFileHeader);
    loadBmpInfoHeaderPart(loadPath,bmpInfoHeader);
    *bitsPerPixel   =   (unsigned int)bmpInfoHeader->bmpInfo_bitcount;
    *width          =   (unsigned int)bmpInfoHeader->bmpInfo_width;
    *height         =   (unsigned int)bmpInfoHeader->bmpInfo_height;
    *size           =   (unsigned int)bmpInfoHeader->bmpInfo_image_size;
    rawDataPtr       =  (unsigned char*)malloc(bmpInfoHeader->bmpInfo_image_size);
    loadBmpDataPart(loadPath,bmpInfoHeader->bmpInfo_image_size,rawDataPtr);
    return rawDataPtr;
}
void readBMPInfo(char * loadPath)
{
    bitmap_file_header bmpFileHeader;
    bitmap_info_header bmpInfoHeader;
    unsigned int bitsPerPixel,size,width,height;
    unsigned char * rgb=read_bmpFile_RawRGB(loadPath,&bitsPerPixel,&size,&width,&height,&bmpFileHeader,&bmpInfoHeader);
    printf("RawData Size            :   %d Bytes\n",size);
    printf("RawData bitsPerPixel    :   %d bits/Pixel\n",bitsPerPixel);
    printf("RawData width           :   %d pixels\n",width);
    printf("RawData height          :   %d pixels\n",height);
    printf("RawData compression     :   %d \n",bmpInfoHeader.bmpInfo_compression);
    printf("RawData bmpInfo_planes  :   %d \n",bmpInfoHeader.bmpInfo_planes);
    cvt_RGB888_BGR888(rgb,width*height);
    REVERSE_RGB888( rgb,width,height);
    free_bmpRes();
}
//************************test************************************//
#define test_rgb_width (640)
#define test_rgb_height (800)
#define test_save_path "test.bmp"
#define test_load_path "frame_4394.bin"
unsigned char testRGB888[test_rgb_width*test_rgb_height*3];
unsigned char testLoadY8[test_rgb_width*test_rgb_height];

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
    readBMPInfo(test_save_path);
}