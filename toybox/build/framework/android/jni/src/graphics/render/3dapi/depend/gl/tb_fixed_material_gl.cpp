/*!
 * @file
 * @brief OpenGLの固定機能マテリアル依存コード
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_material.h"

#if (TB_3DAPI_IS_GL)

namespace toybox
{

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
    return TB_S_OK;
}

}

#endif