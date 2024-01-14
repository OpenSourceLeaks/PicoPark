/*!
 * @file
 * @brief テクスチャサンプラ依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2011.04.10
 */

#ifndef _INCLUDED_TB_TEXTURE_SAMPLER_DEPEND_H_
#define _INCLUDED_TB_TEXTURE_SAMPLER_DEPEND_H_

#if (TB_3DAPI_IS_DX9) || (TB_3DAPI_IS_GL) || (TB_3DAPI_IS_S3D)
    #include "./tb_texture_sampler_default.h"
#elif (TB_3DAPI_IS_GX2)
    #include "./gx2/tb_texture_sampler_gx2.h"
#elif (TB_3DAPI_IS_GFX)
#include "./gfx/tb_texture_sampler_gfx.h"
#else
    #error "Invalid TbTextureSamplerStd"
#endif

#endif
