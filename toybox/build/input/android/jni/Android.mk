LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_inputd
else
LOCAL_CFLAGS := -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE    := tb_input
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/input/tb_input_setting.cpp \
    src/input/tb_keyboard_common.cpp \
    src/input/tb_virtual_input_manager.cpp \
    src/input/depend/manual/tb_keyboard_manual.cpp \
    src/input/depend/manual/tb_mouse_manual.cpp \
    src/input/depend/manual/tb_pad_manual.cpp \
    src/input/depend/manual/tb_touch_manual.cpp \

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
