LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := actrec
LOCAL_SRC_FILES :=  actrec_export.c wrapper.c

include $(BUILD_SHARED_LIBRARY)
