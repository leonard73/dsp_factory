#ifndef __BMP_H__
#define __BMP_H__
#define BITMAP_FILE_HEADER_LENGTH (14)
#define BITMAP_INFO_HEADER_LENGTH (40)
#define BITMAP_HEADER_LENGTH (BITMAP_FILE_HEADER_LENGTH+BITMAP_INFO_HEADER_LENGTH)
#define BITCOUNT_TO_BYTE_SHIFT    (3)
typedef struct bitmap_file_header_t
{
	unsigned short  bmpFile_type;
	unsigned short  bmpFile_size[2];
	unsigned short  bmpFile_reversed[2];
	unsigned short  bmpFile_offbits[2];
}bitmap_file_header;
typedef struct bitmap_info_header_t
{
	unsigned int    bmpInfo_size;
	unsigned int    bmpInfo_width;
	unsigned int    bmpInfo_height;
	unsigned short  bmpInfo_planes;
	unsigned short  bmpInfo_bitcount;
	unsigned int    bmpInfo_compression;
	unsigned int    bmpInfo_image_size;
	unsigned int    bmpInfo_xpels_per_meter;
	unsigned int    bmpInfo_ypels_per_meter;
	unsigned int    bmpInfo_used_clr;
	unsigned int    bmpInfo_important_clr;
}bitmap_info_header;
void create_bmp_file(   
                        unsigned char * bmpFileBuf,
                        unsigned char * p_rawBGR,
                        unsigned int bitPerPixel,
                        unsigned int sizeRawData,
                        unsigned int width,
                        unsigned int height
);
void cvt_RGB888_BGR888( 
						unsigned char * p_dataRGB888,
						unsigned int pixelNB
);
unsigned char * create_bmpFile_rgb888(
						unsigned char * p_dataRGB888,
						unsigned int size,
						unsigned int width,
						unsigned int height
);
void free_bmpRes();
void save_RawRGB_bmpFile(
						char * savePath,
						unsigned char * p_dataRGB888,
						unsigned int size,
						unsigned int width,
						unsigned int height
);
void loadFile(
						char * loadPath,
						unsigned int size,
						unsigned char * buf
);
void loadBmpFileHeaderPart(
						char * loadPath,
						bitmap_file_header * buf
);
void loadBmpInfoHeaderPart(
						char * loadPath,
						bitmap_info_header * buf
);
void loadBmpDataPart(
						char * loadPath,
						unsigned int size,
						unsigned char * buf
);
unsigned char* read_bmpFile_RawRGB(
						char * loadPath,
						unsigned int *bitsPerPixel,
						unsigned int *size,
						unsigned int*width,
						unsigned int*height,
                        bitmap_file_header *bmpFileHeader,
                        bitmap_info_header *bmpInfoHeader
);
void readBMP(			
						char * loadPath,
						bitmap_file_header *bmpFileHeader,
						bitmap_info_header *bmpInfoHeader,
						unsigned char *raw
);
void REVERSE_Gray8(
						unsigned char * p_dataGray8,
						unsigned int width,
						unsigned int height
);
void save_RawGray8_bmpFile(
						char * savePath,
						unsigned char * p_dataGray8,
						unsigned int size,
						unsigned int width,
						unsigned int height
);
#endif //__BMP_H__