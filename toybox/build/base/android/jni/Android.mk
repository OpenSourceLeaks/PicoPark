LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_based
else
LOCAL_CFLAGS := -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE := tb_base
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/base/actor/tb_actor.cpp \
    src/base/actor/tb_actor_scene.cpp \
    src/base/crypt/tb_crc32.cpp \
    src/base/io/tb_file.cpp \
    src/base/io/tb_file_impl.cpp \
    src/base/io/tb_file_util.cpp \
    src/base/io/tb_file_manager.cpp \
    src/base/io/tb_log.cpp \
    src/base/io/tb_print.cpp \
    src/base/io/archive/tb_archive.cpp \
    src/base/io/archive/tb_archive_exporter.cpp \
    src/base/io/archive/tb_file_archive.cpp \
    src/base/io/archive/tb_file_mapper_archive.cpp \
    src/base/io/depend/std/tb_file_std.cpp \
    src/base/io/depend/std/tb_file_mapper_std.cpp \
    src/base/io/depend/android/tb_file_android.cpp \
    src/base/io/depend/android/tb_file_mapper_android.cpp \
    src/base/io/resource/tb_resource.cpp \
    src/base/io/resource/tb_resource_manager.cpp \
    src/base/io/stream/tb_const_memory_stream.cpp \
    src/base/io/stream/tb_memory_stream.cpp \
    src/base/io/stream/tb_file_stream.cpp \
    src/base/io/stream/tb_stream_binary_reader.cpp \
    src/base/io/stream/tb_stream_binary_writer.cpp \
    src/base/io/stream/tb_stream_text_reader.cpp \
    src/base/io/stream/tb_stream_text_writer.cpp \
    src/base/math/tb_geometry_util.cpp \
    src/base/math/tb_math_helper.cpp \
    src/base/math/tb_matrix.cpp \
    src/base/math/tb_quaternion.cpp \
    src/base/math/tb_random.cpp \
    src/base/math/tb_vector2.cpp \
    src/base/math/tb_vector3.cpp \
    src/base/math/tb_vector4.cpp \
    src/base/memory/tb_fixed_allocator.cpp \
    src/base/memory/tb_heap.cpp \
    src/base/memory/tb_memory.cpp \
    src/base/memory/tb_new_delete.cpp \
    src/base/string/tb_static_string.cpp \
    src/base/string/tb_string_converter.cpp \
    src/base/string/encoding/tb_string_encoding.cpp \
    src/base/string/encoding/tb_string_encoding_sjis.cpp \
    src/base/system/tb_assert.cpp \
    src/base/system/depend/android/tb_stack_trace_android.cpp \
    src/base/util/tb_async_func.cpp \
    src/base/task/tb_sequence_task.cpp \
    src/base/task/tb_task.cpp \
    src/base/task/tb_task_manager.cpp \
    src/base/time/tb_tick_common.cpp \
    src/base/time/depend/posix/tb_tick_posix.cpp \
    src/base/time/depend/std/tb_time_std.cpp \
     
LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
