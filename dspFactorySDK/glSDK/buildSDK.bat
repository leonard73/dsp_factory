@echo off
set compileToolsDir=..\..\Tools
set x64_static_gl_lib=%compileToolsDir%\x64\freeglut\bin\x64
set x64_g++=%compileToolsDir%\x64\mingw64\bin\g++
set x64_gcc=%compileToolsDir%\x64\mingw64\bin\gcc
set x64_ar=%compileToolsDir%\x64\mingw64\bin\ar
set formatSource_files=source\glHandler.cpp
set source_files=%formatSource_files%
set source_files=%source_files% %demoSource_files%
set source_output_path=release\windows
set source_static_lib=release\windows\glHandler.a
@echo on
echo " [COMPILER INFO]: selected g++ is %x64_g++% "
@echo off
del /Q %source_output_path%\*.o
del /Q %source_static_lib%
%x64_g++%  -c %source_files% -L%x64_static_gl_lib% -lfreeglut -lopengl32 -lwinmm -lgdi32  -o %source_output_path%\glHandler.o
%x64_ar% -r %source_static_lib% %source_output_path%\glHandler.o
copy include\glSDK\*.h %source_output_path%\
echo ****************************************************************************************
echo ****************************************************************************************
echo                      BUILD ALL FORMATSDK LIBS INTO RELEASE FOLDER
echo ****************************************************************************************
echo ****************************************************************************************