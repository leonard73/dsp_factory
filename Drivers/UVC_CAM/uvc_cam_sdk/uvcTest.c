#include "uvc_cam_sdk.h"
uint8_t meanOfBuffer(uint8_t * pixels_p, uint32_t num);
int main(int argc, char *argv[])
{
	int ret=uvc_camera_sdk_init("/dev/video0",640,480,uvc_camera_sdk_stream_yuyv);
	uvc_camera_sdk_stream_start(1000212);
	static int captured_id=0;
	while(1)
	{
		camera_t * captured_p=uvc_camera_sdk_stream_captured_once();
		printf("ID[%d]: W=%d; H=%d; buf_length=%d; mean_pixel=%d\n",captured_id++,captured_p->width,captured_p->height,captured_p->head.length,meanOfBuffer(captured_p->head.start,captured_p->head.length));
	}
    uvc_camera_sdk_stream_stop();
	return 0;
}
uint8_t meanOfBuffer(uint8_t * pixels_p, uint32_t num)
{
	uint8_t mean_value=0;
	uint32_t sum_total=0;
	for(uint32_t i=0;i<num;++i)
	{
		sum_total += pixels_p[i];
	}
	mean_value = num>0 ? sum_total/num : 0;
	return mean_value;
}