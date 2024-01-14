/*!
 * @file
 * @brief 固定シェーダ依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_FIXED_SHADER_DEPEND_H_
#define _INCLUDED_TB_FIXED_SHADER_DEPEND_H_

#if TB_3DAPI_IS_DX9
#   include "./dx9/tb_fixed_shader_dx9.h"
#elif TB_3DAPI_IS_GL
#   if TB_IS_ENABLE_FIXED_SHADER
#       include "./gl/tb_fixed_shader_gl.h"
#   else
#       include "./gl/tb_fixed_shader_gl_sim.h"
#   endif
#elif TB_3DAPI_IS_GX2
#   include "./gx2/tb_fixed_shader_gx2.h"
#elif TB_3DAPI_IS_GFX
#   include "./gfx/tb_fixed_shader_gfx.h"
#elif TB_3DAPI_IS_S3D
#   include "./s3d/tb_fixed_shader_s3d.h"
#endif

#endif
