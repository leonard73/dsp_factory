#!/bin/bash
#set DSP_FACTORy_ROOT_PATH
export DSP_FACTORY_ROOT_PATH=$(pwd)
#set LIBFORMAT DRIVER PATH
export LIBFORMAT_DRIVER_PATH=$(pwd)/libFormat
#set EIGEN_PATH
export EIGEN_PATH=$(pwd)/thirdParty
#Set your NDK Install Path
export NDK_ROOT_PATH=$ANDROID_NDK
#Set TARGET_COMPILE_PLATFORM
#Supported "linux_amd64" , "linux_arm64" , "linux_arm" ,"win_amd64" , "android_arm64v8a"
export TARGET_COMPILE_PLATFORM="win_amd64"
export LABEL_android_arm64v8a="android_arm64v8a"
export LABEL_linux_amd64="linux_amd64"
export LABEL_linux_arm64="linux_arm64"
export LABEL_win_amd64="win_amd64"