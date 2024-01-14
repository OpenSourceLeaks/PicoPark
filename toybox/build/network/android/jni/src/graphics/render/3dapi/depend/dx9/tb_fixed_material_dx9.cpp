/*!
 * @file
 * @brief DirectX9の固定機能マテリアル依存コード
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_material.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"

namespace toybox
{

namespace{
    static const TbUint32 MAIN_COLOR_INDEX = 0;
    static const D3DCOLORVALUE COLORVALUE_BLACK = {0.0f,0.0f,0.0f,0.0f};
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbFixedMaterial::TbFixedMaterial()
    : m_Color()
    , m_Depend()
{
    SetDiffuseColor( TbColor::White() );
}

/*!
 * コピーコンストラク
 * @author Miyake Shunsuke
 * @since 2013.03.13
 */
TbFixedMaterial::TbFixedMaterial(const TbFixedMaterial& mat)
    : m_Color()
    , m_Depend()
{
    SetDiffuseColor(mat.GetDiffuseColor());
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbFixedMaterial::~TbFixedMaterial()
{
}

/*!
 * コピー演算子
 * @author Miyake Shunsuke
 * @since 2013.03.13
 */
TbFixedMaterial& TbFixedMaterial::operator=( const TbFixedMaterial& mat )
{
    SetDiffuseColor(mat.GetDiffuseColor());
    return *this;
}

/*!
 * カラー設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedMaterial::SetDiffuseColor( const TbColor& color )
{
    m_Color = color;
    D3DCOLORVALUE d3dColor = TbColorToD3DCOLORVALUE( m_Color );
    m_Depend.materialDX9.Diffuse = d3dColor;
    m_Depend.materialDX9.Ambient = COLORVALUE_BLACK;
    m_Depend.materialDX9.Specular = COLORVALUE_BLACK;
    m_Depend.materialDX9.Emissive = COLORVALUE_BLACK;
    m_Depend.materialDX9.Power = 0.0f;
    return TB_S_OK;
}

}

#endif