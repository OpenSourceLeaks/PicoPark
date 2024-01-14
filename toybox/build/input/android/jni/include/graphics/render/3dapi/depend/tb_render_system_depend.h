/*!
 * @file
 * @brief レンダーシステム依存分岐ヘッダー
 */

#ifndef _INCLUDED_TB_RENDER_SYSTEM_DEPEND_H_
#define _INCLUDED_TB_RENDER_SYSTEM_DEPEND_H_

#if TB_3DAPI_IS_GX2
    #include "./gx2/tb_render_system_gx2.h"
#elif TB_3DAPI_IS_GFX
    #include "./gfx/tb_render_system_gfx.h"
#else
    struct TbRenderSystemDepend{};
#endif

#endif


