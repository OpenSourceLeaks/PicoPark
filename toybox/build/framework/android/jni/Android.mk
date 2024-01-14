LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_frameworkd
else
LOCAL_CFLAGS := -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE := tb_framework
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include
LOCAL_SRC_FILES := \
    src/framework/tb_framework.cpp \
    src/framework/impl/tb_framework_impl.cpp \
    src/framework/sub/tb_framework_simple.cpp \
    src/framework/debug/menu/tb_debug_menu.cpp \
    src/framework/debug/menu/tb_debug_menu_item.cpp \
    src/framework/debug/menu/tb_debug_menu_item_alias.cpp \
    src/framework/debug/menu/tb_debug_menu_item_bool.cpp \
    src/framework/debug/menu/tb_debug_menu_item_button.cpp \
    src/framework/debug/menu/tb_debug_menu_item_color.cpp \
    src/framework/debug/menu/tb_debug_menu_item_float.cpp \
    src/framework/debug/menu/tb_debug_menu_item_vec.cpp \
    src/framework/debug/menu/tb_debug_menu_manager.cpp \
    src/framework/debug/viewer/tb_allocator_viewer.cpp \
    src/framework/debug/viewer/tb_fixed_allocator_view.cpp \
    src/framework/debug/viewer/tb_heap_allocator_view.cpp \

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
