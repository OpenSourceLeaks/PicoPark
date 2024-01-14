/*!
 * @file
 * @brief 深度ステンシルバッファの機種依存コード
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_DEPEND_H_
#define _INCLUDED_TB_DEPTH_STENCIL_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_depth_stencil_dx9.h"
#elif TB_3DAPI_IS_GL
    #include "./gl/tb_depth_stencil_gl.h"
#elif TB_3DAPI_IS_S3D
    #include "./s3d/tb_depth_stencil_s3d.h"
#elif TB_3DAPI_IS_GX2
    #include "./s3d/tb_depth_stencil_gx2.h"
#elif TB_3DAPI_IS_GFX
#include "./gfx/tb_depth_stencil_gfx.h"
#endif

#endif