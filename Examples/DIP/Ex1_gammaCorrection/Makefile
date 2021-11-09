#libFormat Path
common_path:=../common
cc:=g++
src:=demo.cpp
src+=$(common_path)/source/glController.cpp
src+=$(common_path)/source/uvc_cam_sdk.cpp
include_dir:=$(common_path)/include	
flag:=-lGL -lGLU -lglut -I$(include_dir)  -fpermissive -lpthread
exefile:=gammaCorrection
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