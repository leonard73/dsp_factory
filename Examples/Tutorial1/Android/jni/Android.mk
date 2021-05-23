LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE  := arm
LOCAL_CFLAGS    := -fPIE -fPIC -mfloat-abi=softfp -mfpu=neon -DNDK -O0
LOCAL_LDLIBS    := -llog -landroid
LOCAL_CFLAGS    += -DPLATFORM_ANDROID_ARM64
LOCAL_SRC_FILES:= ../../Codes/NeonLoadStore.c
LOCAL_C_INCLUDES:= ../../Codes/
LOCAL_MODULE:= dspFactoryT1_NeonLDTest
LOCAL_MULTILIB := 64
include $(BUILD_EXECUTABLE)
