LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_networkd
else
LOCAL_CFLAGS := -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE    := tb_network
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/network/tb_packet_util.cpp \
    src/network/tb_packet_recv_buffer.cpp \
    src/network/tb_packet_send_buffer.cpp \
    src/network/depend/std/tb_accept_socket_std.cpp \
    src/network/depend/std/tb_client_socket_std.cpp \
    src/network/depend/std/tb_datagram_socket_std.cpp \
    src/network/depend/std/tb_network_manager_std.cpp \
    src/network/depend/std/tb_network_util_std.cpp \
    src/network/depend/std/tb_server_socket_std.cpp \
    src/network/depend/std/tb_socket_std.cpp \

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
