﻿/*!
 * @file
 * @brief 頂点バッファ依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_BUFFER_DEPEND_H_
#define _INCLUDED_TB_VERTEX_BUFFER_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_vertex_buffer_dx9.h"
#elif TB_3DAPI_IS_GL
    #include "./gl/tb_vertex_buffer_gl.h"
#elif TB_3DAPI_IS_S3D
    #include "./s3d/tb_vertex_buffer_s3d.h"
#elif TB_3DAPI_IS_GX2
    #include "./s3d/tb_vertex_buffer_gx2.h"
#elif TB_3DAPI_IS_GFX
    #include "./gfx/tb_vertex_buffer_gfx.h"
#endif

#endif