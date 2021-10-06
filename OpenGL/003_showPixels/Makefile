#libFormat Path
libFormat_path:=../../libFormat
cc:=gcc
src:=glShowPixels.c
src+=$(libFormat_path)/bmp/bmp.c
inc:=$(libFormat_path)/bmp
flag:=-lGL -lGLU -lglut
exefile:=glShowPixels
build:
		$(cc) -o $(exefile) $(src) $(flag) -I ${inc}
run:
		./$(exefile)
clean:
		rm -rf $(exefile)