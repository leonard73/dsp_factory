#include "uvc_cam_sdk.h"
camera_t* camera_open(const char *device_path, uint32_t width, uint32_t height);
void camera_init(camera_t* p_camera,unsigned int v4l_format);
void camera_start(camera_t* p_camera);
int camera_capture(camera_t* p_camera);
int camera_frame(camera_t* p_camera, struct timeval timeout);
void camera_stop(camera_t* p_camera);
void camera_release(camera_t* p_camera);
void camera_close(camera_t* p_camera);
camera_t* camera_open(const char *device_path, uint32_t width, uint32_t height)
{
	int fd = open(device_path, O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		return NULL;
	}
	
	camera_t* p_camera = (camera_t *)malloc(sizeof(camera_t));
	if (NULL == p_camera)
	{
		close(fd);
		return NULL;
	}
	
	memset(p_camera, 0, sizeof(camera_t));
	p_camera->fd = fd;
	p_camera->width = width;
	p_camera->height = height;
	p_camera->buffer_count = 0;
	p_camera->buffers = NULL;
	p_camera->head.start = NULL;
	p_camera->head.length = 0;
	
	return p_camera;	
}
void camera_init(camera_t* p_camera,unsigned int v4l_format)
{
	struct v4l2_capability cap;
	
	memset(&cap, 0, sizeof(cap));
	if (ioctl(p_camera->fd, VIDIOC_QUERYCAP, &cap) >= 0)
	{
		printf("version:%d, cap:%d\n", cap.version, cap.capabilities);
		if (! (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
		{
			printf("[ERROR] camera_init: no campture\n");
			return ;
		}
		
		if (! (cap.capabilities & V4L2_CAP_STREAMING))
		{
			printf("[ERROR] camera_init: no streaming\n");
			return ;
		}		
	}
	
	struct v4l2_cropcap cropcap;
	memset(&cropcap, 0, sizeof(cropcap));
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(p_camera->fd, VIDIOC_CROPCAP, &cropcap) >= 0)
	{
		struct v4l2_crop crop;
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;
		
		ioctl(p_camera->fd, VIDIOC_S_CROP, &crop);
	}
	
	struct v4l2_format format;
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = p_camera->width;
	format.fmt.pix.height = p_camera->height;
	format.fmt.pix.pixelformat = v4l_format;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	format.fmt.pix.width = p_camera->width;
	ioctl(p_camera->fd, VIDIOC_S_FMT, &format);
	
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(p_camera->fd, VIDIOC_REQBUFS, &req);
	p_camera->buffer_count = req.count;
	p_camera->buffers = calloc(req.count, sizeof(buffer_t));
	
	size_t i = 0;
	size_t buf_max = 0;
	struct v4l2_buffer buffer;
	for (i = 0; i < p_camera->buffer_count; ++i)
	{
		memset(&buffer, 0, sizeof(buffer));
		
		buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;
		
		ioctl(p_camera->fd, VIDIOC_QUERYBUF, &buffer);
		if (buffer.length > buf_max)
			buf_max = buffer.length;
		
		printf("count:%ld, length:%d\n", i, buffer.length);
		p_camera->buffers[i].length = buffer.length;
		p_camera->buffers[i].start = mmap(NULL, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, p_camera->fd, buffer.m.offset);
		if (NULL == p_camera->buffers[i].start)
		{
			perror("mmap error");
			return ;
		}
	}
	
	p_camera->head.start = (uint8_t *)malloc(buf_max);
	
	return ;
}
void camera_start(camera_t* p_camera)
{
	size_t i = 0;
	struct v4l2_buffer buffer;
	for (i = 0; i < p_camera->buffer_count;  ++i)
	{
		memset(&buffer, 0, sizeof(buffer));
		
		buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;
		
		ioctl(p_camera->fd, VIDIOC_QBUF, &buffer);
	}
	
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(p_camera->fd, VIDIOC_STREAMON, &type);
	
	return ;
}
int camera_capture(camera_t* p_camera)
{
	struct v4l2_buffer buffer;
	
	memset(&buffer, 0, sizeof(buffer));
		
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	
	ioctl(p_camera->fd, VIDIOC_DQBUF, &buffer);
	
	memcpy(p_camera->head.start, p_camera->buffers[buffer.index].start, buffer.bytesused);
	p_camera->head.length = buffer.bytesused;
	
	ioctl(p_camera->fd, VIDIOC_QBUF, &buffer);
	
	return 1;
}
int camera_frame(camera_t* p_camera, struct timeval timeout)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(p_camera->fd, &fds);
	
	int nready = select(p_camera->fd + 1, &fds, NULL, NULL, &timeout);
	if (nready < 0)
	{
		perror("[ERROR] camera_frame");
		return nready;
	}
	
	if (nready == 0)
	{
		return nready;
	}
	
	return camera_capture(p_camera);
}
void camera_stop(camera_t* p_camera)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(p_camera->fd, VIDIOC_STREAMOFF, &type);
	
	return ;
}
void camera_release(camera_t* p_camera)
{
	size_t i = 0;
	for (i = 0; i < p_camera->buffer_count;  ++i)
	{
		munmap(p_camera->buffers[i].start, p_camera->buffers[i].length);
	}
	
	free(p_camera->buffers);
	p_camera->buffer_count = 0;
	p_camera->buffers = NULL;
	
	free(p_camera->head.start);
	p_camera->head.length = 0;
	p_camera->head.start = NULL;
	
	return ;
}
void camera_close(camera_t* p_camera)
{
	close(p_camera->fd);
	free(p_camera);
}
camera_t* p_camera_global = NULL;
struct timeval timeout;
int uvc_camera_sdk_init(const char * device_path,uint32_t pixel_width,uint32_t pixel_height,int format)
{
	// try to access device_path
	printf("try to access device_path\n");
	if (access(device_path, F_OK) != 0)
	{
		perror("[ERROR] can not find the usb camera device!");
		return -1;
	}
	printf("access device_path successfully!\n");
	p_camera_global = camera_open(device_path, pixel_width, pixel_height);
	if (NULL == p_camera_global)
	{
		perror("[ERROR] can not open the usb camera device!");
		return -1;
	}
	switch(format)
	{
		case uvc_camera_sdk_stream_yuyv:camera_init(p_camera_global,V4L2_PIX_FMT_YUYV);break;
		case uvc_camera_sdk_stream_mpeg:camera_init(p_camera_global,V4L2_PIX_FMT_MPEG);break;
		default:camera_init(p_camera_global,V4L2_PIX_FMT_YUYV);break;
	}
	printf("uvc_camera_sdk_init successfully!\n");
	return 0;
}
void uvc_camera_sdk_stream_start(uint64_t timeout_microSeconds)
{
   timeout.tv_sec  = timeout_microSeconds/1000000;
   timeout.tv_usec = timeout_microSeconds%1000000;
   printf("try to capture 10 frames at beginning timeout.tv_sec=%ld, timeout.tv_usec=%ld\n",timeout.tv_sec,timeout.tv_usec);
   camera_start(p_camera_global);
   for(int i = 0; i < 10; ++i)
   {
	camera_frame(p_camera_global, timeout);
   }
   printf("capture 10 frames ok!\n");
}
camera_t * uvc_camera_sdk_stream_captured_once()
{
	camera_frame(p_camera_global, timeout);
	return p_camera_global;
}
void uvc_camera_sdk_stream_stop()
{
	camera_stop(p_camera_global);
	camera_release(p_camera_global);
	camera_close(p_camera_global);
}