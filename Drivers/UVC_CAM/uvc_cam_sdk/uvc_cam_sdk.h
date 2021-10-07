#ifndef CAMERA_H_
#define CAMERA_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#define UVC_DEVICE_PATH "/dev/video0"
typedef struct {
	uint8_t *start;
	size_t length;
}buffer_t;
typedef struct {
	int fd;
	uint32_t width;
	uint32_t height;
	size_t buffer_count;
	buffer_t *buffers;
	buffer_t head;
}camera_t;
enum uvc_camera_sdk_format_t
{
	uvc_camera_sdk_stream_yuyv=0,
	uvc_camera_sdk_stream_mpeg=1
}uvc_camera_sdk_format;
int  uvc_camera_sdk_init(const char * device_path,uint32_t pixel_width,uint32_t pixel_height,int format);
void uvc_camera_sdk_stream_start(uint64_t timeout_microSeconds);
camera_t * uvc_camera_sdk_stream_captured_once();
void uvc_camera_sdk_stream_stop();
#endif // CAMERA_H_
