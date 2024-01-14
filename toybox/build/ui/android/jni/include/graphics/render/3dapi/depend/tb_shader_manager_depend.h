/*!
 * @file
 * @brief シェーダー管理クラスの依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_SHADER_MANAGER_DEPEND_H_
#define _INCLUDED_TB_SHADER_MANAGER_DEPEND_H_

#if TB_SHADER_API_IS_CG
    #include "./cg/tb_shader_manager_cg.h"
#elif TB_SHADER_API_IS_HLSL
    #include "./dx9/hlsl/tb_shader_manager_hlsl.h"
#elif TB_SHADER_API_IS_GLSL
    #include "./gl/glsl/tb_shader_manager_glsl.h"
#elif TB_SHADER_API_IS_AGAL
    #include "./s3d/tb_shader_manager_agal.h"
#elif TB_SHADER_API_IS_GX2
    #include "./s3d/tb_shader_manager_gx2.h"
#elif TB_SHADER_API_IS_GFX
    #include "./gfx/tb_shader_manager_gfx.h"
#endif

#endif