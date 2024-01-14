/*!
 * @file
 * @brief TbRenderDeviceの機種依存コード
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_RENDER_DEVICE_DEPEND_H_
#define _INCLUDED_TB_RENDER_DEVICE_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_render_device_dx9.h"
#elif TB_3DAPI_IS_GL
    #include "./gl/tb_render_device_gl.h"
#elif TB_3DAPI_IS_S3D
    #include "./s3d/tb_render_device_s3d.h"
#elif TB_3DAPI_IS_GX2
    #include "./s3d/tb_render_device_gx2.h"
#elif TB_3DAPI_IS_GFX
    #include "./gfx/tb_render_device_gfx.h"
#endif

#endif
