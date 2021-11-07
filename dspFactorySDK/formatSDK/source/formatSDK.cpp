#include "../include/formatSDK.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define LOG_STR2(S1,S2)  (std::cout<<S1<<S2<<std::endl)  
#define LOG_STR_INT(S1,S2)  (std::cout<<S1<<(int)S2<<std::endl)
formatSDK::bmpCtl::bmpCtl(bmpMode setMode)
{
    switch(setMode)
    {
        case bmpMode_ReadBMP:LOG_STR2("setMode: ","bmpMode_ReadBMP");this->bmpModeCfg  =  setMode;break;
        case bmpMode_SaveBMP:LOG_STR2("setMode: ","bmpMode_SaveBMP");this->bmpModeCfg  =  setMode;break;
        default:LOG_STR2("setMode failed: ","invalid mode id");break;
    }
}
formatSDK::bmpCtl::~bmpCtl()
{
    this->free_bmp_data_mem();
}
void formatSDK::bmpCtl::init_bmp_data_mem(unsigned int raw_data_bytes)
{
    this->rawDataPtr = (unsigned char *)malloc(raw_data_bytes);
}
void formatSDK::bmpCtl::free_bmp_data_mem()
{
    if(!this->rawDataPtr)
    {
        free(this->rawDataPtr);
    }
}
void formatSDK::bmpCtl::log_bmp_header_info()
{
    LOG_STR_INT("bmpFile_type : ",((this->global_bmp_data.bmpFile_type)));
    LOG_STR_INT("bmpFile_type low  bit: ",((this->global_bmp_data.bmpFile_type) & (unsigned short)0x00ff));
    LOG_STR_INT("bmpFile_type high bit: ",(((this->global_bmp_data.bmpFile_type) & (unsigned short)0xff00)>>8));
    LOG_STR_INT("bmpFile_offbits:",this->global_bmp_data.bmp_header_struct.bmp_file_struct.bmpFile_offbytes);
    LOG_STR_INT("bmpFile_reversed[0]: ",this->global_bmp_data.bmp_header_struct.bmp_file_struct.bmpFile_reversed[0]);
    LOG_STR_INT("bmpFile_reversed[1]: ",this->global_bmp_data.bmp_header_struct.bmp_file_struct.bmpFile_reversed[1]);
    LOG_STR_INT("bmpFile_size: ",this->global_bmp_data.bmp_header_struct.bmp_file_struct.bmpFile_size);
    LOG_STR_INT("bmpInfo_bitcount: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_bitcount);
    LOG_STR_INT("bmpInfo_compression: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_compression);
    LOG_STR_INT("bmpInfo_height: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_height);
    LOG_STR_INT("bmpInfo_width: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_width);
    LOG_STR_INT("bmpInfo_important_clr: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_important_clr);
    LOG_STR_INT("bmpInfo_planes: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_planes);
    LOG_STR_INT("bmpInfo_size: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_size);
    LOG_STR_INT("bmpInfo_used_clr: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_used_clr);
    LOG_STR_INT("bmpInfo_xpels_per_meter: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_xpels_per_meter);
    LOG_STR_INT("bmpInfo_ypels_per_meter: ",this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_ypels_per_meter);
}
void formatSDK::bmpCtl::read_bmp_file(const char * readBmpPath,BMP_DATA & loadBmpData)
{
    FILE * fp =  fopen(readBmpPath,"rb");
    fread(&this->global_bmp_data.bmpFile_type,sizeof(unsigned char),sizeof(unsigned short),fp);
    fread(&this->global_bmp_data.bmp_header_struct,sizeof(unsigned char),sizeof(BMP_HEADER),fp);
    uint32_t data_bytes  = this->global_bmp_data.bmp_header_struct.bmp_file_struct.bmpFile_size - BITMAP_HEADER_LENGTH;
    init_bmp_data_mem(data_bytes);
    uint32_t pixel_height_nb = this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_height;
    uint32_t pixel_bytes_width = this->global_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_image_size/pixel_height_nb;
    for(uint32_t i = 0 ; i < pixel_height_nb ; i++)
    {
        unsigned char * load_addr = this->rawDataPtr + (pixel_height_nb-1-i)* pixel_bytes_width;
        fread(load_addr,sizeof(unsigned char),pixel_bytes_width,fp);
    }
    // fread(this->rawDataPtr,sizeof(unsigned char),data_bytes,fp);
    fclose(fp);
    loadBmpData.bmp_header_struct=this->global_bmp_data.bmp_header_struct;
    loadBmpData.bmpFile_type=this->global_bmp_data.bmpFile_type;
    loadBmpData.pixel_raw_array=this->rawDataPtr;
}
void formatSDK::bmpCtl::save_bmp_file(const char * saveBmpPath,BMP_DATA * saveBmpData)
{
    //force to set the const bits
    saveBmpData->bmpFile_type=(unsigned short)(19778);
    saveBmpData->bmp_header_struct.bmp_file_struct.bmpFile_offbytes=BITMAP_HEADER_LENGTH;
    saveBmpData->bmp_header_struct.bmp_file_struct.bmpFile_reversed[0]=0;
    saveBmpData->bmp_header_struct.bmp_file_struct.bmpFile_reversed[1]=0;
    saveBmpData->bmp_header_struct.bmp_file_struct.bmpFile_size = saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_image_size+BITMAP_HEADER_LENGTH;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_size= BITMAP_INFO_HEADER_LENGTH;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_important_clr=0;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_compression=0;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_planes=1;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_used_clr=0;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_xpels_per_meter=DEFAULT_PIXELS_X_METER;
    saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_ypels_per_meter=DEFAULT_PIXELS_Y_METER;
    //save to target path
    FILE * fp = fopen(saveBmpPath,"wb");
    fwrite(&saveBmpData->bmpFile_type,sizeof(unsigned short),1,fp);
    fwrite(&saveBmpData->bmp_header_struct,sizeof(unsigned char),sizeof(BMP_HEADER),fp);
    unsigned int bytes_perPixel =(unsigned int) (saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_bitcount >>3);
    unsigned int pixel_width    = saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_width;
    unsigned int pixel_height   = saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_height;
    for(int i=pixel_height-1;i>=0;i--)
    {
        unsigned char* save_array_addr =  saveBmpData->pixel_raw_array + i * pixel_width * bytes_perPixel;
        fwrite(save_array_addr,sizeof(unsigned char),pixel_width * bytes_perPixel,fp);
    }
    // fwrite(saveBmpData->pixel_raw_array,sizeof(unsigned char),saveBmpData->bmp_header_struct.bmp_info_struct.bmpInfo_image_size,fp);
    fclose(fp);
}
void formatSDK::bmpCtl::save_bgr_bmp(const char * saveBmpPath,unsigned char * bgr,unsigned int pixelW,unsigned int pixelH)
{
    BMP_DATA save_bmp_data;
    save_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_height=pixelH;
    save_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_width=pixelW;
    save_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_bitcount=24;
    save_bmp_data.bmp_header_struct.bmp_info_struct.bmpInfo_image_size=pixelW*pixelH*3;
    save_bmp_data.pixel_raw_array=bgr;
    this->save_bmp_file(saveBmpPath,&save_bmp_data);
}