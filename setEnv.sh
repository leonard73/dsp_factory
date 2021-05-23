#!/bin/bash
#set DSP_FACTORy_ROOT_PATH
export DSP_FACTORY_ROOT_PATH=$(pwd)
#Set your NDK Install Path
export NDK_ROOT_PATH=/root/Android/Sdk/ndk/22.0.7026061
#Set TARGET_COMPILE_PLATFORM
#Supported "linux_amd64" , "linux_arm64" , "win_amd64" , "android_arm64v8a"
export TARGET_COMPILE_PLATFORM="win_amd64"
#Echo TOP LEVEL Setting:
echo '##################################################'
echo '#############DSP FACTORRY ENV SETTING#############'
echo '##################################################'
echo '##################################################'
echo '#############TOP LEVEL Setting:###################'
echo 'DSP_FACTORY_ROOT_PATH: '$DSP_FACTORY_ROOT_PATH
echo 'TARGET_COMPILE_PLATFORM: '$TARGET_COMPILE_PLATFORM
echo 'NDK_ROOT_PATH: '$NDK_ROOT_PATH
echo '##################################################'
echo '##################################################'
#SET & Echo SUB LEVEL Setting:
echo '#############SUB LEVEL Setting:###################'
export NDK_COMPILE_SCRIPT_PATH=$DSP_FACTORY_ROOT_PATH/Scripts/NDK_SCRIPTS
echo 'NDK_COMPILE_SCRIPT_PATH: '$NDK_COMPILE_SCRIPT_PATH
#Label Setting:
export LABEL_android_arm64v8a="android_arm64v8a"
export LABEL_linux_amd64="linux_amd64"
export LABEL_linux_arm64="linux_arm64"
export LABEL_win_amd64="win_amd64"
if  [ "$TARGET_COMPILE_PLATFORM" == "$LABEL_win_amd64" ] ; then
echo "LABEL_win_amd64"
else
echo 2
fi
echo '##################################################'
echo '##################################################'
echo '##################################################'