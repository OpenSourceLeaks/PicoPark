/*!
 * @file
 * @brief テクスチャの機種依存コード
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_TEXTURE_DEPEND_H_
#define _INCLUDED_TB_TEXTURE_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_texture_dx9.h"
#elif TB_3DAPI_IS_GL
    #include "./gl/tb_texture_gl.h"
#elif TB_3DAPI_IS_S3D
    #include "./s3d/tb_texture_s3d.h"
#elif TB_3DAPI_IS_GX2
    #include "./s3d/tb_texture_gx2.h"
#elif TB_3DAPI_IS_GFX
    #include "./gfx/tb_texture_gfx.h"
#endif

#endif