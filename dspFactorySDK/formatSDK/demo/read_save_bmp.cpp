#include <iostream>
#include "formatSDK.h"
#define LOG_STR(S) std::cout<<S<<std::endl
#define RED___CHANNEL (2)
#define BLUE__CHANNEL (1)
#define GREEN_CHANNEL (0)
void set_const_channels_bgr(uint8_t * bgr,uint32_t width,uint32_t height,uint8_t channel_sel,uint8_t set_val);
int main()
{
    LOG_STR("THIS IS READ AND SAVE BMP DEMO BY FORMAT SDK");
    formatSDK::bmpCtl   bmpCTL_obj =  formatSDK::bmpCtl(formatSDK::bmpCtl::bmpMode_ReadBMP);
    formatSDK::BMP_DATA bmp_load_data;
    bmpCTL_obj.read_bmp_file("splash.bmp",bmp_load_data);
    bmpCTL_obj.log_bmp_header_info();
    set_const_channels_bgr( bmp_load_data.pixel_raw_array,
                            bmp_load_data.bmp_header_struct.bmp_info_struct.bmpInfo_width,
                            bmp_load_data.bmp_header_struct.bmp_info_struct.bmpInfo_height,
                            RED___CHANNEL,
                           150);
    bmpCTL_obj.save_bgr_bmp( "splash_red100.bmp",
                             bmp_load_data.pixel_raw_array,
                             bmp_load_data.bmp_header_struct.bmp_info_struct.bmpInfo_width,
                             bmp_load_data.bmp_header_struct.bmp_info_struct.bmpInfo_height);
}
void set_const_channels_bgr(uint8_t * bgr,uint32_t width,uint32_t height,uint8_t channel_sel,uint8_t set_val)
{
    for(uint32_t i=0;i<height*width;i++)
    {
        bgr[i*3+channel_sel] = set_val;
    }
}
