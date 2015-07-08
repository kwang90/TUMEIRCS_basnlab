LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := helloJNI
LOCAL_SRC_FILES := stringFromJNI.c

include $(BUILD_SHARED_LIBRARY)