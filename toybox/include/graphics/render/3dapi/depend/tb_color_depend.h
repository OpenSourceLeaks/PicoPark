/*!
 * @file
 * @brief TbColorの機種依存コード
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_COLOR_DEPEND_H_
#define _INCLUDED_TB_COLOR_DEPEND_H_

namespace toybox
{

#define TB_COLOR_MAP_ARGB 1
#define TB_COLOR_MAP_RGBA 2
#define TB_COLOR_MAP_ABGR 3
#define TB_COLOR_TYPE_U8 1
#define TB_COLOR_TYPE_F32 2

#if TB_3DAPI_IS_DX9
    #define TB_COLOR_MAP TB_COLOR_MAP_ARGB  // ARGB
    #define TB_COLOR_TYPE TB_COLOR_TYPE_U8  // u8
#elif TB_3DAPI_IS_GL || TB_3DAPI_IS_S3D || TB_3DAPI_IS_GX2 || TB_3DAPI_IS_GFX
    #define TB_COLOR_MAP TB_COLOR_MAP_ABGR // RGBA
    #define TB_COLOR_TYPE TB_COLOR_TYPE_F32 // Float32
#endif

}

#endif