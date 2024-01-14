/*!
 * @file
 * @brief OpenGLの固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_light.h"

#if (TB_3DAPI_IS_GL)

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbFixedLight::TbFixedLight()
    : m_Light()
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbFixedLight::~TbFixedLight()
{
}

/*!
 * 種類設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetType( TbLightType type )
{
    if( TB_LIGHT_TYPE_POINT == type &&
        TB_LIGHT_TYPE_DIRECTION == type )
    {
        m_Light.SetType( type );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}
    
/*!
 * 色
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetColor( const TbColor& color )
{
    return m_Light.SetColor( color );
}

/*!
 * 位置設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetPosition( const TbVector4& pos )
{
    return m_Light.SetPosition ( pos );
}

/*!
 * 半径設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetRadius( TbFloat32 radius )
{
    return m_Light.SetRadius( radius );
}

/*!
 * 方向設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetDirection( const TbVector4& dir )
{
    return m_Light.SetDirection( dir );
}
    
}

#endif