LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE  := arm
LOCAL_CFLAGS    := -fPIE -fPIC -mfloat-abi=softfp -mfpu=neon -DNDK -O0
LOCAL_LDLIBS    := -llog -landroid
LOCAL_CFLAGS    += -DPLATFORM_ANDROID_ARM64 -D_USE_ARM_NEON_OPT_
LOCAL_SRC_FILES:= ../../Codes/NeonRgb2Gray.c
LOCAL_SRC_FILES+=$(LIBFORMAT_DRIVER_PATH)/bmp/bmp.c #add bmp driver
LOCAL_C_INCLUDES:= ../../Codes/
LOCAL_C_INCLUDES+=$(LIBFORMAT_DRIVER_PATH)/bmp
LOCAL_MODULE:= dspFactoryT2_NeonLDTest
LOCAL_MULTILIB := 64
include $(BUILD_EXECUTABLE)
