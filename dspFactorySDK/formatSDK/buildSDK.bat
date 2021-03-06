@echo off
set compileToolsDir=..\..\Tools\x64\mingw64\bin
set Path=%~dp0%compileToolsDir%;Path
set path=%~dp0%compileToolsDir%;path
set x64_g++=g++
set x64_gcc=gcc
set x64_ar=ar
set formatSource_files=source\formatSDK.cpp
set source_files=%formatSource_files%
set source_files=%source_files% %demoSource_files%
set source_output_path=release\windows
set source_static_lib=release\windows\formatSDK.a
@echo on
echo " [COMPILER INFO]: selected g++ is %x64_g++% "
@echo off
del /Q %source_output_path%\*.o
del /Q %source_static_lib%
%x64_g++% -c %source_files% -o %source_output_path%\formatSDK.o
%x64_ar% -r %source_static_lib% %source_output_path%\*.o
copy include\*.h %source_output_path%\
echo ****************************************************************************************
echo ****************************************************************************************
echo                      BUILD ALL FORMATSDK LIBS INTO RELEASE FOLDER
echo ****************************************************************************************
echo ****************************************************************************************