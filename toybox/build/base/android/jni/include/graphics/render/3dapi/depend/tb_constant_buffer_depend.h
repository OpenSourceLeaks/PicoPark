/*!
 * @file
 * @brief constantバッファ依存分岐ヘッダー
 * @author teco
 */

#ifndef _INCLUDED_TB_CONSTANT_BUFFER_DEPEND_H_
#define _INCLUDED_TB_CONSTANT_BUFFER_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./tb_constant_buffer_default.h"
#elif TB_3DAPI_IS_GL
    #include "./gl/tb_constant_buffer_gl.h"
#elif TB_3DAPI_IS_GFX
    #include "./gfx/tb_constant_buffer_gfx.h"
#endif

#endif
