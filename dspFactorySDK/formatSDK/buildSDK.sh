compile_tool=g++
static_build_tool=ar
source_files=source/formatSDK.cpp
release_folder=release/linux
source_file_output=${release_folder}/formatSDK.o
target_sdk_static_lib=${release_folder}/formatSDK.a
${compile_tool} -c ${source_files} -o ${source_file_output}
${static_build_tool} -r ${target_sdk_static_lib} ${source_file_output}
cp -f  include/*.h ${release_folder}/
