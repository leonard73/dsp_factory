@echo off
set compileToolsDir=..\..\..\Tools\x64\mingw64\bin
set Path=%~dp0%compileToolsDir%;Path
set path=%~dp0%compileToolsDir%;path
set x64_g++=g++
set x64_gcc=gcc
set x64_ar=ar
set formatSource_staticlibs=./../release/windows/formatSDK.a
set demoSource_files=read_save_bmp.cpp
set source_files=%source_files% %demoSource_files%
set demo_exe=read_save_bmp.exe
set include_path=../release/windows
@echo on
echo " [COMPILER INFO]: selected g++ is %x64_g++% "
@echo off
del /Q %demo_exe%
del splash_red100.bmp
%x64_g++% %source_files% -I%include_path% -static %formatSource_staticlibs% -o %demo_exe% 
echo ****************************************************************************************
echo ****************************************************************************************
echo                      BUILD read_save_bmp.exe ok ; now RUN the exe!
echo ****************************************************************************************
echo ****************************************************************************************
.\%demo_exe%
del /p %demo_exe%