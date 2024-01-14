/*!
 * プレイヤー入力
 * @author teco
 */

#include "fwd.h"
#include "side_action_player_input.h"
#include "root.h"
#include "side_action/side_action_types.h"
#include <base/math/tb_random.h>

///////////////////////////////////
//  SideActionPlayer::RandomInput
///////////////////////////////////

/*!
 * 自動テスト用ボタン
 * @author teco
 */
void SideActionPlayerRandomInput::OnUpdate( TbFloat32 deltatime )
{
    static const TbFloat32 JUMPING_TIME  = 1.0f;
    static const TbFloat32 JUMP_INTERVAL = 1.0f;
    static const TbFloat32 JUMP_RATE = 0.1f;
    static const TbFloat32 DIR_INTERVAL = 1.0f;

    if( IsOn(SysInput::BUTTON_BIT_JUMP) ) {
        if( GetPressTime(SysInput::BUTTON_JUMP) > JUMPING_TIME ) {
            Release(SysInput::BUTTON_JUMP);
        }
    }else{
        if( GetReleaseTime(SysInput::BUTTON_JUMP) > JUMP_INTERVAL ){
            if( TbRandomFloat() < JUMP_RATE ){
                Press(SysInput::BUTTON_JUMP);
            }
        }
    }

    if( m_DirectionTimer > DIR_INTERVAL ) {
        TbFloat32 val = TbRandomFloat();
        if( val < 0.5f ) {
            Release(SysInput::BUTTON_LEFT);
            Press(SysInput::BUTTON_RIGHT);
        }else{
            Release(SysInput::BUTTON_RIGHT);
            Press(SysInput::BUTTON_LEFT);
        }
        m_DirectionTimer = 0.0f;
    }
    m_DirectionTimer += deltatime;
}


///////////////////////////////////
//  SideActionPlayerTitleInput
///////////////////////////////////

void SideActionPlayerTitleInput::OnUpdate( TbFloat32 deltatime )
{
    static const TbFloat32 SWITCH_INTERVAL = 7.0f;
    m_Time += deltatime;
    if( SWITCH_INTERVAL <= m_Time )
    {
        if( IsOn(TB_BIT(SysInput::BUTTON_JUMP)) )
        {
            Release(SysInput::BUTTON_JUMP);
        }
        else
        {
            Press(SysInput::BUTTON_JUMP);
        }
        m_Time = 0.0f;
    }
}

///////////////////////////////////
//  SideActionPlayer::SwitchInput
///////////////////////////////////

/*!
 * スイッチ入力
 * @author teco
 */
void SideActionPlayerSwitchInput::OnEvent( TbUint32 event , void* param )
{
    SysInput::Button button = static_cast<SysInput::Button>(*reinterpret_cast<TbSint32*>(param));
    switch( event )
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        {
            Press(button);
        }
        break;
    case SIDE_ACTION_EVENT_SWITCH_OFF:
        {
            Release(button);
        }
        break;
    }
}

/*!
 * スイッチ入力定期処理
 * @author teco
 */
void SideActionPlayerSwitchInput::OnUpdate( TbFloat32 deltatime )
{
}

///////////////////////////////////
//  SideActionPlayer::MultiPlayerInput
///////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerMultiPlayerInput::SideActionPlayerMultiPlayerInput()
    : m_ActivePlayer(-1)
    , m_PlayerCount(0)
    , m_WalkVelo(0.0f)
{
}

/*!
 * 初期化
 * @author teco
 */
void SideActionPlayerMultiPlayerInput::Initialize( TbFloat32 jumpVelo , TbFloat32 walkVelo )
{
    m_WalkVelo = walkVelo;
    m_PlayerCount = Root::GetInstance().GetPlayerCount();
    m_PlayerCount = TbClamp<TbUint32>(m_PlayerCount,0,TB_ARRAY_COUNT_OF(m_PlayerInfo));

    m_PlayerInfo[0].isEnableLeft = TB_TRUE;
    m_PlayerInfo[0].isEnableRight = TB_TRUE;

    m_JumpCountLimit = CMN_PLAYER_COUNT / m_PlayerCount;

    TbFloat32 velo = 0.0f;
    TbUint32 jumpPlayerStartIndex = 0;
    TbUint32 jumpPlayerCount = m_PlayerCount;
    for( TbUint32 i = 0; i < jumpPlayerCount; ++i ){
        PlayerInfo& info = m_PlayerInfo[jumpPlayerStartIndex+i];
        info.jumpVelo = jumpVelo;
    }
}

/*!
 * ジャンプ力取得
 * @author teco
 */
TbFloat32 SideActionPlayerMultiPlayerInput::GetJumpVelo() const
{
    TbFloat32 result = 0.0f;
    if( 0 <= m_ActivePlayer && TB_VERIFY(m_ActivePlayer < m_PlayerCount ) ){
        result = m_PlayerInfo[m_ActivePlayer].jumpVelo;
    }
    return result;
}

/*!
 * イベント受信
 * @author teco
 */
void SideActionPlayerMultiPlayerInput::OnEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_RESET_JUMP:
        {
            for( TbUint32 i = 0; i < m_PlayerCount; ++i ){
                m_PlayerInfo[i].jumpCount = 0;
            }
            m_ActivePlayer = -1;
            Release(SysInput::BUTTON_JUMP);
        }
        break;
    }
}

/*!
 * 定期処理
 * @author teco
 */
void SideActionPlayerMultiPlayerInput::OnUpdate( TbFloat32 deltatime )
{
    if( 0 == m_PlayerCount ){
        return;
    }
    TbBool isOnLeft = TB_FALSE;
    TbBool isOnRight = TB_FALSE;
    TbBool isOnUp = TB_FALSE;
    TbUint32 min = TB_UINT32_MAX;
    for( TbUint32 i = 0; i < m_PlayerCount; ++i ){
        min = TbMin(min,m_PlayerInfo[i].jumpCount);
    }
    for( TbUint32 i = 0; i < m_PlayerCount; ++i ){
        if( SysInput::IsPress(SysInput::BUTTON_JUMP,i) ){
            if(min == m_PlayerInfo[i].jumpCount && m_PlayerInfo[i].jumpCount < m_JumpCountLimit )
            {
                ++m_PlayerInfo[i].jumpCount;
                m_ActivePlayer = i;
                Reset(SysInput::BUTTON_JUMP);
                Press(SysInput::BUTTON_JUMP);
                break;
            }
        }
        if( m_PlayerInfo[i].isEnableUp && SysInput::IsOn(SysInput::BUTTON_UP,i) ){
            isOnUp = TB_TRUE;
        }
        if( m_PlayerInfo[i].isEnableLeft && SysInput::IsOn(SysInput::BUTTON_LEFT,i) ){
            isOnLeft = TB_TRUE;
        }
        if( m_PlayerInfo[i].isEnableRight && SysInput::IsOn(SysInput::BUTTON_RIGHT,i) ){
            isOnRight = TB_TRUE;
        }
    }
    // 
    if( IsOn(SysInput::BUTTON_BIT_UP) ){
        if( !isOnUp ){
            Release(SysInput::BUTTON_UP);
        }
    }else{
        if( isOnUp ){
            Press(SysInput::BUTTON_UP);
        }
    }
    // 
    if( IsOn(SysInput::BUTTON_BIT_LEFT) ){
        if( !isOnLeft ){
            Release(SysInput::BUTTON_LEFT);
        }
    }else{
        if( isOnLeft ){
            Press(SysInput::BUTTON_LEFT);
        }
    }
    //
    if( IsOn(SysInput::BUTTON_BIT_RIGHT) ){
        if( !isOnRight ){
            Release(SysInput::BUTTON_RIGHT);
        }
    }else{
        if( isOnRight ){
            Press(SysInput::BUTTON_RIGHT);
        }
    }
    if( 0 <= m_ActivePlayer ){
        if( !SysInput::IsOn(SysInput::BUTTON_JUMP,m_ActivePlayer) ){
            Release(SysInput::BUTTON_JUMP);
            m_ActivePlayer = -1;
        }
    }
}
