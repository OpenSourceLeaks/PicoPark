/*!
 * @file
 * @brief シェーダー管理クラスの依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_SHADER_CONTEXT_DEPEND_H_
#define _INCLUDED_TB_SHADER_CONTEXT_DEPEND_H_

#if TB_SHADER_API_IS_CG
    #include "./cg/tb_shader_program_cg.h"
#elif TB_SHADER_API_IS_HLSL
    class TbShaderProgramDepend {};
#elif TB_SHADER_API_IS_GLSL
    #include "./gl/glsl/tb_shader_program_glsl.h"
#elif TB_SHADER_API_IS_AGAL
    #include "./s3d/tb_shader_program_agal.h"
#elif TB_SHADER_API_IS_GX2
    #include "./s3d/tb_shader_program_gx2.h"
#elif TB_SHADER_API_IS_GFX
#include "./gfx/tb_shader_program_gfx.h"
#endif

#endif
