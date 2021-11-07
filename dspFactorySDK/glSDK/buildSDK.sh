compileToolsDir=../../Tools
source_files=source/glHandler.cpp
source_output_path=release/linux
source_static_lib=release/linux/glHandler.a
rm -rf  ${source_output_path}/*.o
rm -rf  ${source_static_lib}
g++  -c ${source_files} -lGL -lGLU -lglut  -o ${source_output_path}/glHandler.o
ar   -r ${source_static_lib} ${source_output_path}/glHandler.o
cp include/glSDK/*.h ${source_output_path}/
# echo ****************************************************************************************
# echo ****************************************************************************************
# echo                      BUILD ALL FORMATSDK LIBS INTO RELEASE FOLDER
# echo ****************************************************************************************
# echo ****************************************************************************************