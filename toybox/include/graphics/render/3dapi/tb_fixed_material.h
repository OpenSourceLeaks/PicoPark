/*!
 * マテリアルクラス
 * @author Miyake Shunsuke
 * @since 2011.06.16
 */

#ifndef _INCLUDED_TB_FIXED_MATERIAL_H_
#define _INCLUDED_TB_FIXED_MATERIAL_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/material/tb_material.h>

#include "./depend/tb_fixed_material_depend.h"

namespace toybox
{

/*!
 * 固定機能マテリアル
 * @author Miyake Shunsuke
 * @since 2011.06.17
 */
class TbFixedMaterial
{
public:

    // コンストラクタ
    TbFixedMaterial();
    // コピーコンストラクタ
    TbFixedMaterial(const TbFixedMaterial& mat);
    // デストラクタ
    ~TbFixedMaterial();

public:

    // コピー演算子
    TbFixedMaterial& operator=( const TbFixedMaterial& mat );

public:
    
    // カラー設定
    TbResult SetDiffuseColor( const TbColor& color );
    
    // カラー取得
    const TbColor GetDiffuseColor() const {
        return m_Color;
    }

public:
    
    // 依存クラス取得
    const TbFixedMaterialDepend& GetDepend() const  { return m_Depend; }

private:
    TbColor               m_Color;
    TbFixedMaterialDepend m_Depend; // 依存クラス
};

}

#endif
