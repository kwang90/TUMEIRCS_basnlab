LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := fft
LOCAL_SRC_FILES := wrapper.c fft4android.c

include $(BUILD_SHARED_LIBRARY)
