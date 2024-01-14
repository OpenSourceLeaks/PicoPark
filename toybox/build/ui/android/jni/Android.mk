LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_uid
else
LOCAL_CFLAGS := -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE    := tb_ui
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/ui/tb_ui_button.cpp \
    src/ui/tb_ui_button_skin.cpp \
    src/ui/tb_ui_frame_sprite.cpp \
    src/ui/tb_ui_image_box.cpp \
    src/ui/tb_ui_manager.cpp \
    src/ui/tb_ui_text_box.cpp \
    src/ui/tb_ui_widget.cpp \
    src/ui/tb_ui_window.cpp \

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
