#ifndef __FORMAT_SDK_H__
#define __FORMAT_SDK_H__
#include <iostream>
namespace formatSDK
{
    #define BITMAP_FILE_LABEL_LENGTH    (sizeof(unsigned short))
    #define BITMAP_FILE_HEADER_LENGTH   (sizeof(BMP_FILE))
    #define BITMAP_INFO_HEADER_LENGTH   (sizeof(BMP_INFO))
    #define BITMAP_HEADER_LENGTH        (BITMAP_FILE_HEADER_LENGTH+BITMAP_INFO_HEADER_LENGTH+BITMAP_FILE_LABEL_LENGTH)
    #define BITCOUNT_TO_BYTE_SHIFT      (3)
    #define DEFAULT_PIXELS_X_METER      (37795)
    #define DEFAULT_PIXELS_Y_METER      (37795)
    typedef struct BMP_FILE_t
    {
	    unsigned int    bmpFile_size;
	    unsigned short  bmpFile_reversed[2];
	    unsigned int    bmpFile_offbytes;
    }BMP_FILE;//12 bytes
    typedef struct BMP_INFO_t
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
    }BMP_INFO;//40 bytes
    typedef struct BMP_HEADER_t
    {
        BMP_FILE bmp_file_struct;
        BMP_INFO bmp_info_struct;
    }BMP_HEADER;//54 bytes
    typedef struct BMP_DATA_t
    {
        unsigned short  bmpFile_type;
        BMP_HEADER bmp_header_struct;
        unsigned char * pixel_raw_array;
    }BMP_DATA;
    class bmpCtl{
        public: enum bmpMode
        {
            bmpMode_ReadBMP=0,
            bmpMode_SaveBMP=1
        };
        private:  unsigned char bmpFileHeader[BITMAP_FILE_HEADER_LENGTH];
        private:  unsigned char bmpInfoHeader[BITMAP_INFO_HEADER_LENGTH];
        private:  unsigned char * rawDataPtr=0;
        private:  bmpMode bmpModeCfg;
        private:  BMP_DATA global_bmp_data;
        private:  void init_bmp_data_mem(unsigned int raw_data_bytes);
        private:  void free_bmp_data_mem();
        public:   bmpCtl(bmpMode setMode);
        public:   ~bmpCtl();
        public:   void log_bmp_header_info();
        public:   void read_bmp_file(const char * readBmpPath,BMP_DATA & loadBmpData);
        public:   void save_bmp_file(const char * saveBmpPath,BMP_DATA * saveBmpData);
        public:   void save_bgr_bmp(const char * saveBmpPath,unsigned char * bgr,unsigned int pixelW,unsigned int pixelH);
    };
};
#endif //__FORMAT_SDK_H__