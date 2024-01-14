LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := simple3d
LOCAL_SRC_FILES := simple3d.cpp

LOCAL_C_INCLUDES := $(TBSDK_DIR)/include
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_LDLIBS := -llog -lGLESv2 \
	-L$(TBSDK_DIR)/lib/android/$(TARGET_ARCH_ABI) \
    -ltb_framework \
    -ltb_input \
    -ltb_graphics \
    -ltb_base \

include $(BUILD_SHARED_LIBRARY)
