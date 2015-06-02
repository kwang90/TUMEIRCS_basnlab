LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hallomodule
LOCAL_SRC_FILES := halloWrapper.c untitled.c

include $(BUILD_SHARED_LIBRARY)
