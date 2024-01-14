/*!
 * 巨大用入力
 * @author teco
 * @since 2015.09.14
 */

#include "tb_fwd.h"
#include <crayon/input/cr_player_input.h>
#include <crayon/input/cr_giant_input.h>

namespace crayon
{

/*!
 * コンストラクタ
 * @author teco
 */
CrPlayerInput::CrPlayerInput()
    : m_Mode(CrPlayerInput::MODE_DEFAULT)
    , m_Custom(nullptr)
    , m_IsPause(TB_FALSE)
{
}

/*!
 * デストラクタ
 * @author teco
 */
CrPlayerInput::~CrPlayerInput()
{
}

/*!
 * 定期処理
 * @author teco
 */
void CrPlayerInput::Update( float deltasec ) 
{
    if( m_Custom ){
        m_Custom->Update(deltasec);
    }
}

/*!
 * 初期化
 * @author teco
 */
void CrPlayerInput::SetMode( Mode mode )
{
    m_Mode = mode;
}

/*!
 * ボタンを押しているか
 * @author teco
 */
TbBool CrPlayerInput::IsOn( CrInput::Button button , TbUint32 playerIndex ) const
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
        result = CrInput::IsOn(button,playerIndex);
        break;
    case MODE_GIANT:
        result = CrGiantInput::GetInstance().IsOn(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = CrInput::IsOn(button,playerIndex);
        if( CrGiantInput::GetInstancePtr() ){
            result |= CrGiantInput::GetInstance().IsOn(button,playerIndex);
        }
        break;
    }
    return result;
}

/*!
 * ボタンを押しているか
 * @author teco
 * @since 2013.10.26
 */
TbBool CrPlayerInput::IsOnRepeat( CrInput::Button button , TbUint32 playerIndex ) const
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
        result = CrInput::IsOnRepeat(button,playerIndex);
        break;
    case MODE_GIANT:
        result = CrGiantInput::GetInstance().IsOnRepeat(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = CrInput::IsOnRepeat(button,playerIndex);
        if( CrGiantInput::GetInstancePtr() ){
            result |= CrGiantInput::GetInstance().IsOnRepeat(button,playerIndex);
        }
        break;
    }
    return result;
}

/*! 
 * ボタンを押したか
 * @author teco
 * @since 2013.10.26
 */
TbBool CrPlayerInput::IsPress( CrInput::Button button , TbUint32 playerIndex ) const
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
        result = CrInput::IsPress(button,playerIndex);
        break;
    case MODE_GIANT:
        result = CrGiantInput::GetInstance().IsPress(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = CrInput::IsPress(button,playerIndex);
        if( CrGiantInput::GetInstancePtr() ){
            result |= CrGiantInput::GetInstance().IsPress(button,playerIndex);
        }
        break;
    }
    return result;
}

/*!
 * ボタンを話したか
 * @author teco
 * @since 2013.10.26
 */
TbBool CrPlayerInput::IsRelease( CrInput::Button button , TbUint32 playerIndex ) const
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
        result = CrInput::IsRelease(button,playerIndex);
        break;
    case MODE_GIANT:
        result = CrGiantInput::GetInstance().IsRelease(button,playerIndex);
        break;
    case MODE_HYBRID:
        result = CrInput::IsRelease(button,playerIndex);
        if( CrGiantInput::GetInstancePtr() ){
            result |= CrGiantInput::GetInstance().IsRelease(button,playerIndex);
        }
        break;
    }
    return result;

}

}

