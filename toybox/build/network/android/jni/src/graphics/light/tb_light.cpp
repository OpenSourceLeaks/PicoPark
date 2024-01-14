/*!
 * ライト
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */

#include "tb_fwd.h"
#include "graphics/light/tb_light.h"
#include "base/system/tb_assert.h"

namespace toybox
{

namespace
{

/*!
 * 点光源初期化
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
void initPointLight( TbLight& light )
{
    static const TbVector4 INIT_POS( 0.0f , 0.0f , 0.0f , 1.0f );
    light.SetPosition( INIT_POS );
}

/*!
 * 指向性ライト初期化
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
void initDirectionLight( TbLight& light )
{
    static const TbVector4 INIT_DIR( 0.0f , -1.0f , 0.0f , 0.0f );
    light.SetDirection( INIT_DIR );
}


/*!
 * 種類別初期設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult init( TbLight& light , TbLightType type )
{
    typedef void (*InitFuncType)( TbLight& light );
    static const InitFuncType FUNC[] =
    {
        initPointLight , 
        initDirectionLight ,
        nullptr , 
        nullptr 
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(FUNC) == TB_LIGHT_TYPE_MAX );
    if( FUNC[type] ){
        FUNC[type]( light );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbLight::TbLight()
    : m_Type( TB_LIGHT_TYPE_DIRECTION )
    , m_Color( TbColor::White() )
    , m_Pos()
    , m_Radius(0.0f)
    , m_Direction( 0.0f , -1.0f , 0.0f , 0.0f )
{
    SetType( TB_LIGHT_TYPE_DIRECTION );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbLight::~TbLight()
{
}
    
/*!
 * 種類設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbLight::SetType( TbLightType type )
{
    if( 0 <= type && type < TB_LIGHT_TYPE_MAX && TB_S_OK == init( *this , type ) ){
        m_Type = type;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 位置設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbLight::SetPosition( const TbVector4& pos )
{
    static const TbUint32 BIT_MASK = TB_BIT( TB_LIGHT_TYPE_POINT );
    if( BIT_MASK & TB_BIT(m_Type) ){
        m_Pos = pos;
        m_Pos.SetW( 1.0f );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 半径設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbLight::SetRadius( TbFloat32 radius )
{
    static const TbUint32 BIT_MASK = TB_BIT( TB_LIGHT_TYPE_POINT );
    if( BIT_MASK & TB_BIT(m_Type) ){
        m_Radius = radius;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}
        
/*!
 * 方向設定
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
TbResult TbLight::SetDirection( const TbVector4& dir )
{
    static const TbUint32 BIT_MASK = TB_BIT( TB_LIGHT_TYPE_DIRECTION );
    if( BIT_MASK & TB_BIT(m_Type) ){
        m_Direction = dir;
        m_Direction.SetW( 0.0f );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

}
