/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_light.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"

namespace toybox
{

namespace
{
    TbBool TbLightTypeToD3DLIGHTTYPE( D3DLIGHTTYPE &dst , const TbLightType& src )
    {
        static const D3DLIGHTTYPE CONVERT_TABLE[] = {
            D3DLIGHT_POINT , 
            D3DLIGHT_DIRECTIONAL ,
        };
        if( 0 <= src && src < TB_ARRAY_COUNT_OF(CONVERT_TABLE) ){
            dst = CONVERT_TABLE[ src ];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    static const D3DCOLORVALUE COLORVALUE_BLACK = {0.0f,0.0f,0.0f,0.0f};
}
   
/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbFixedLight::TbFixedLight()
    : m_Light()
    , m_Depend()
{
    TbLightTypeToD3DLIGHTTYPE( m_Depend.lightDX9.Type , m_Light.GetType() );
    D3DCOLORVALUE color = TbColorToD3DCOLORVALUE( m_Light.GetColor() );
    color.a = 0.0f;
    m_Depend.lightDX9.Diffuse = color;
    m_Depend.lightDX9.Specular = COLORVALUE_BLACK;
    m_Depend.lightDX9.Ambient = COLORVALUE_BLACK;
    m_Depend.lightDX9.Position = TbVector4ToD3DVECTOR( m_Light.GetPosition() );
    m_Depend.lightDX9.Direction = TbVector4ToD3DVECTOR( m_Light.GetDirection() );
    m_Depend.lightDX9.Range = m_Light.GetRadius();
    m_Depend.lightDX9.Attenuation0 = 1.0f;
    m_Depend.lightDX9.Attenuation1 = 0.0f;
    m_Depend.lightDX9.Attenuation2 = 0.0f;
    m_Depend.lightDX9.Theta = 0.0f;
    m_Depend.lightDX9.Phi = 0.0f;
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
    if( TbLightTypeToD3DLIGHTTYPE( m_Depend.lightDX9.Type , type) ){
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
    if( TB_S_OK == m_Light.SetColor( color ) ){
        D3DCOLORVALUE color = TbColorToD3DCOLORVALUE( m_Light.GetColor() );
        color.a = 0.0f;
        m_Depend.lightDX9.Diffuse = color;
        m_Depend.lightDX9.Specular = COLORVALUE_BLACK;
        m_Depend.lightDX9.Ambient = COLORVALUE_BLACK;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 位置設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetPosition( const TbVector4& pos )
{
    if( TB_S_OK == m_Light.SetPosition ( pos ) ){
        m_Depend.lightDX9.Position = TbVector4ToD3DVECTOR( m_Light.GetPosition() );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 半径設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetRadius( TbFloat32 radius )
{
    if( TB_S_OK == m_Light.SetRadius( radius ) ){
        m_Depend.lightDX9.Range = m_Light.GetRadius();
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 方向設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbFixedLight::SetDirection( const TbVector4& dir )
{
    if( TB_S_OK == m_Light.SetDirection( dir ) ){
        m_Depend.lightDX9.Direction = TbVector4ToD3DVECTOR( m_Light.GetDirection() );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}
    
}

#endif