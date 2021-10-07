#libFormat Path
libFormat_path:=../../libFormat
libDriver_path:=../../Drivers
cc:=gcc
src:=glShowPixels.c
src+=$(libFormat_path)/bmp/bmp.c
src+=$(libDriver_path)/UVC_CAM/uvc_cam_sdk/uvc_cam_sdk.c
include_bmp:=$(libFormat_path)/bmp 
include_uvc:=$(libDriver_path)/UVC_CAM/uvc_cam_sdk	 
flag:=-lGL -lGLU -lglut -I$(include_bmp)  -I$(include_uvc)
exefile:=glShowPixels
build:
		$(cc) -o $(exefile) $(src) $(flag)
run:
		./$(exefile)
clean:
		rm -rf $(exefile)
all:
		make clean
		make build
		make run