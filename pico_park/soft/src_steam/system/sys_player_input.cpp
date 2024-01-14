/*!
 * 巨大用入力
 * @author teco
 * @since 2015.09.14
 */

#include "fwd.h"
#include "sys_player_input.h"
#include "sys_giant_input.h"

/*!
 * コンストラクタ
 * @author teco
 */
SysPlayerInput::SysPlayerInput()
    : m_Mode(SysPlayerInput::MODE_DEFAULT)
    , m_Custom(nullptr)
    , m_IsPause(TB_FALSE)
{
#if CMN_ENABLE_BUILD_GIANT_INPUT
    m_Mode = SysPlayerInput::MODE_GIANT;
#else
#if 1
    m_Mode = SysPlayerInput::MODE_DEFAULT;
#else
    m_Mode = SysPlayerInput::MODE_HYBRID;
#endif
#endif
}

/*!
 * デストラクタ
 * @author teco
 */
SysPlayerInput::~SysPlayerInput()
{
}

/*!
 * 定期処理
 * @author teco
 */
void SysPlayerInput::Update( float deltasec ) 
{
    if( m_Custom ){
        m_Custom->Update(deltasec);
    }
}

/*!
 * 初期化
 * @author teco
 */
void SysPlayerInput::SetMode( Mode mode )
{
    m_Mode = mode;
}

/*!
 * ボタンを押しているか
 * @author teco
 */
TbBool SysPlayerInput::IsOn( SysInput::Button button , TbUint32 playerIndex ) const
{
    if( m_IsPause ){
        return TB_FALSE;
    }
    if( m_Custom ){
        return m_Custom->IsOn(button,playerIndex);
    }
    TbBool result = TB_FALSE;
    switch( m_Mode )
    {
    case MODE_DEFAULT:
        result = SysInput::IsOn(button,playerIndex);
        break;
    case MODE_GIANT:
        result = SysGiantInput::GetInstance().IsOn(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = SysInput::IsOn(button,playerIndex);
        result |= SysGiantInput::GetInstance().IsOn(button,playerIndex);
        break;
    }
    return result;
}

/*!
 * ボタンを押しているか
 * @author teco
 */
TbBool SysPlayerInput::IsOnAny( TbUint32 playerIndex ) const
{
    for( TbUint32 i = 0; i < SysInput::BUTTON_MAX; ++i )
    {
        if( IsOn(static_cast<SysInput::Button>(i),playerIndex) ){
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * ボタンを押しているか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysPlayerInput::IsOnRepeat( SysInput::Button button , TbUint32 playerIndex ) const
{
    if( m_IsPause ){
        return TB_FALSE;
    }
    if( m_Custom ){
        return m_Custom->IsOnRepeat(button,playerIndex);
    }
    TbBool result = TB_FALSE;
    switch( m_Mode )
    {
    case MODE_DEFAULT:
        result = SysInput::IsOnRepeat(button,playerIndex);
        break;
    case MODE_GIANT:
        result = SysGiantInput::GetInstance().IsOnRepeat(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = SysInput::IsOnRepeat(button,playerIndex);
        result |= SysGiantInput::GetInstance().IsOnRepeat(button,playerIndex);
        break;
    }
    return result;
}

/*! 
 * ボタンを押したか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysPlayerInput::IsPress( SysInput::Button button , TbUint32 playerIndex ) const
{
    if( m_IsPause ){
        return TB_FALSE;
    }
    if( m_Custom ){
        return m_Custom->IsPress(button,playerIndex);
    }
    TbBool result = TB_FALSE;
    switch( m_Mode )
    {
    case MODE_DEFAULT:
        result = SysInput::IsPress(button,playerIndex);
        break;
    case MODE_GIANT:
        result = SysGiantInput::GetInstance().IsPress(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = SysInput::IsPress(button,playerIndex);
        result |= SysGiantInput::GetInstance().IsPress(button,playerIndex);
        break;
    }
    return result;
}

/*!
 * ボタンを話したか
 * @author teco
 * @since 2013.10.26
 */
TbBool SysPlayerInput::IsRelease( SysInput::Button button , TbUint32 playerIndex ) const
{
    if( m_IsPause ){
        return TB_FALSE;
    }
    if( m_Custom ){
        return m_Custom->IsRelease(button,playerIndex);
    }
    TbBool result = TB_FALSE;
    switch( m_Mode )
    {
    case MODE_DEFAULT:
        result = SysInput::IsRelease(button,playerIndex);
        break;
    case MODE_GIANT:
        result = SysGiantInput::GetInstance().IsRelease(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = SysInput::IsRelease(button,playerIndex);
        result |= SysGiantInput::GetInstance().IsRelease(button,playerIndex);
        break;
    }
    return result;

}



