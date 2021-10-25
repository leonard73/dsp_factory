compile_tool=g++
source_files=read_save_bmp.cpp
release_folder=../release/linux
source_file_output=${release_folder}/formatSDK.o
target_sdk_static_lib=${release_folder}/formatSDK.a
demo_exe=read_save_bmp.linux
rm -rf splash_red100.bmp
rm -rf ${demo_exe}
${compile_tool} ${source_files} -I${release_folder} -static ${target_sdk_static_lib} -o ${demo_exe}