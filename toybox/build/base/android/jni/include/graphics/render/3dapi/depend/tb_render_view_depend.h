/*!
 * @file
 * @brief ビュークラス依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_DEPEND_H_
#define _INCLUDED_TB_RENDER_VIEW_DEPEND_H_

#if TB_3DAPI_IS_DX9
#   include "./dx9/tb_render_view_dx9.h"
#elif TB_3DAPI_IS_GL
#   if TB_PLATFORM_IS_WIN
#       if TB_GLAPI_IS_GLES
#           include "./gl/egl/tb_render_view_gl_egl.h"
#       else
#           include "./gl/win32/tb_render_view_gl_win32.h"
#       endif
#   elif TB_PLATFORM_IS_ANDROID
#       include "./gl/android/tb_render_view_gl_android.h"
#   elif TB_PLATFORM_IS_IOS
#       include "./gl/ios/tb_render_view_gl_ios.h"
#   elif TB_GLAPI_IS_GLES
#       include "./gl/egl/tb_render_view_gl_egl.h"
#   else
#       error "Invalid Platform"
#   endif
#elif TB_3DAPI_IS_S3D
#   include "./s3d/tb_render_view_s3d.h"
#elif TB_3DAPI_IS_GX2
#   include "./s3d/tb_render_view_gx2.h"
#elif TB_3DAPI_IS_GFX
#   include "./gfx/tb_render_view_gfx.h"
#endif

#endif
