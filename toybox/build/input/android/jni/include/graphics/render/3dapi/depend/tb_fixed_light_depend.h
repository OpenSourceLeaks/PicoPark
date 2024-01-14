/*!
 * @file
 * @brief 固定機能用ライト依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_FIXED_LIGHT_DEPEND_H_
#define _INCLUDED_TB_FIXED_LIGHT_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_fixed_light_dx9.h"
#elif TB_3DAPI_IS_GL || TB_3DAPI_IS_S3D || TB_3DAPI_IS_GX2 || TB_3DAPI_IS_GFX
    typedef TbUint8 TbFixedLightDepend;
#endif

#endif
