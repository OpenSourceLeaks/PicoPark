/*!
 * @file
 * @brief 固定機能用マテリアル依存分岐ヘッダー
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_FIXED_MATERIAL_DEPEND_H_
#define _INCLUDED_TB_FIXED_MATERIAL_DEPEND_H_

#if TB_3DAPI_IS_DX9
    #include "./dx9/tb_fixed_material_dx9.h"
#elif TB_3DAPI_IS_GL || TB_3DAPI_IS_S3D || TB_3DAPI_IS_GX2 || TB_3DAPI_IS_GFX
    typedef TbUint8 TbFixedMaterialDepend;
#endif

#endif
