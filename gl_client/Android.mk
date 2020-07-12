# This Android.mk was created for testing only!

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := GLESv2
LOCAL_SRC_FILES := ../common/fifo.c   \
				   ../common/server.c \
				   glclient.c         \
				   clientegl.c        \
				   clientgles.c
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../common
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_CFLAGS += -std=c99
ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -ffast-math -mtune=atom -mssse3 -mfpmath=sse
endif
include $(BUILD_SHARED_LIBRARY)

