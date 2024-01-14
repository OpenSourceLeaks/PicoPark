LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef DEBUG
LOCAL_CFLAGS := -D_DEBUG -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11 -g -O0 -ggdb
LOCAL_MODULE := tb_graphicsd
else
LOCAL_CFLAGS := -DTB_USE_GLES2 -Wno-invalid-offsetof -std=c++11
LOCAL_MODULE    := tb_graphics
endif

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    src/graphics/display/depend/fixed/tb_display_manager_fixed.cpp \
    src/graphics/font/tb_font.cpp \
    src/graphics/font/tb_font_buffer_2d.cpp \
    src/graphics/image/tb_image.cpp \
    src/graphics/image/tb_image_bmp.cpp \
    src/graphics/image/tb_image_tga.cpp \
    src/graphics/light/tb_light.cpp \
    src/graphics/material/tb_material.cpp \
    src/graphics/material/tb_material_pmx.cpp \
    src/graphics/math/tb_gr_matrix_util.cpp \
    src/graphics/model/tb_mesh.cpp \
    src/graphics/model/tb_model_data_pmx.cpp \
    src/graphics/motion/tb_motion_data_vmd.cpp \
    src/graphics/motion/tb_motion_vmd.cpp \
    src/graphics/render/tb_draw_2d.cpp \
    src/graphics/render/tb_draw_3d.cpp \
    src/graphics/render/tb_render_command_buffer.cpp \
    src/graphics/render/tb_render_command_buffer_flush.cpp \
    src/graphics/render/tb_render_pass.cpp \
    src/graphics/render/tb_render_pass_manager.cpp \
    src/graphics/render/3dapi/tb_color.cpp \
    src/graphics/render/3dapi/tb_render_device_common.cpp \
    src/graphics/render/3dapi/tb_shader_manager_common.cpp \
    src/graphics/render/3dapi/tb_texture_common.cpp \
    src/graphics/render/3dapi/depend/tb_fixed_light_default.cpp \
    src/graphics/render/3dapi/depend/tb_fixed_material_default.cpp \
    src/graphics/render/3dapi/depend/tb_texture_sampler_default.cpp \
    src/graphics/render/3dapi/depend/gl/tb_depth_stencil_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_fixed_shader_gl_sim.cpp \
    src/graphics/render/3dapi/depend/gl/tb_index_buffer_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_render_device_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_render_target_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_render_view_gl_common.cpp \
    src/graphics/render/3dapi/depend/gl/tb_texture_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_vertex_buffer_gl.cpp \
    src/graphics/render/3dapi/depend/gl/tb_vertex_format_gl.cpp \
    src/graphics/render/3dapi/depend/gl/android/tb_render_view_gl_android.cpp \
    src/graphics/render/3dapi/depend/gl/glsl/tb_shader_manager_glsl.cpp \
    src/graphics/render/3dapi/depend/gl/glsl/tb_shader_program_glsl.cpp \
    src/graphics/resource/tb_resource_model_pmx.cpp \
    src/graphics/resource/tb_resource_texture.cpp \
    src/graphics/scene/tb_render_object_mmd.cpp \
    src/graphics/scene/tb_render_scene.cpp \
    src/graphics/scene/tb_render_scene_node.cpp \
    src/graphics/sprite/tb_elastic_sprite.cpp \
    src/graphics/sprite/tb_sprite.cpp \
    src/graphics/sprite/tb_sprite_material.cpp \
    src/graphics/sprite/tb_sprite_node_anim_tree.cpp \
    src/graphics/sprite/tb_sprite_scene.cpp \
    src/graphics/sprite/tb_sprite_scene_hooker.cpp \
    src/graphics/window/depend/android/tb_window_android.cpp \
    src/graphics/window/depend/android/tb_window_manager_android.cpp \
    src/graphics/window/tb_window_manager_common.cpp \
    
LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
