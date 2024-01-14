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

    if( IsOn(CrInput::BUTTON_BIT_JUMP) ) {
        if( GetPressTime(CrInput::BUTTON_JUMP) > JUMPING_TIME ) {
            Release(CrInput::BUTTON_JUMP);
        }
    }else{
        if( GetReleaseTime(CrInput::BUTTON_JUMP) > JUMP_INTERVAL ){
            if( TbRandomFloat() < JUMP_RATE ){
                Press(CrInput::BUTTON_JUMP);
            }
        }
    }

    if( m_DirectionTimer > DIR_INTERVAL ) {
        TbFloat32 val = TbRandomFloat();
        if( val < 0.5f ) {
            Release(CrInput::BUTTON_LEFT);
            Press(CrInput::BUTTON_RIGHT);
        }else{
            Release(CrInput::BUTTON_RIGHT);
            Press(CrInput::BUTTON_LEFT);
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
        if( IsOn(TB_BIT(CrInput::BUTTON_JUMP)) )
        {
            Release(CrInput::BUTTON_JUMP);
        }
        else
        {
            Press(CrInput::BUTTON_JUMP);
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
    CrInput::Button button = static_cast<CrInput::Button>(*reinterpret_cast<TbSint32*>(param));
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
    , m_MainPlayer(0)
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
 * メインプレイヤー設定
 * @author teco
 */
void SideActionPlayerMultiPlayerInput::SetMainPlayerIndex(TbUint32 index)
{
    m_MainPlayer = index;
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
            m_ActivePlayer = -1;
            Release(CrInput::BUTTON_JUMP);
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
    if( CrInput::IsPress(CrInput::BUTTON_JUMP, m_MainPlayer) ){
        m_ActivePlayer = m_MainPlayer;
        Reset(CrInput::BUTTON_JUMP);
        Press(CrInput::BUTTON_JUMP);
    }
    if ( CrInput::IsOn(CrInput::BUTTON_UP, m_ActivePlayer) || 
         CrInput::IsOn(CrInput::BUTTON_UP, m_MainPlayer) ) {
        isOnUp = TB_TRUE;
    }
    if ( CrInput::IsOn(CrInput::BUTTON_LEFT, m_ActivePlayer) ||
        CrInput::IsOn(CrInput::BUTTON_LEFT, m_MainPlayer)) 
    {
        isOnLeft = TB_TRUE;
    }
    if ( CrInput::IsOn(CrInput::BUTTON_RIGHT, m_ActivePlayer) ||
        CrInput::IsOn(CrInput::BUTTON_RIGHT, m_MainPlayer) ) 
    {
        isOnRight = TB_TRUE;
    }

    // 
    if( IsOn(CrInput::BUTTON_BIT_UP) ){
        if( !isOnUp ){
            Release(CrInput::BUTTON_UP);
        }
    }else{
        if( isOnUp ){
            Press(CrInput::BUTTON_UP);
        }
    }
    // 
    if( IsOn(CrInput::BUTTON_BIT_LEFT) ){
        if( !isOnLeft ){
            Release(CrInput::BUTTON_LEFT);
        }
    }else{
        if( isOnLeft ){
            Press(CrInput::BUTTON_LEFT);
        }
    }
    //
    if( IsOn(CrInput::BUTTON_BIT_RIGHT) ){
        if( !isOnRight ){
            Release(CrInput::BUTTON_RIGHT);
        }
    }else{
        if( isOnRight ){
            Press(CrInput::BUTTON_RIGHT);
        }
    }
    if( 0 <= m_ActivePlayer ){
        if( !CrInput::IsOn(CrInput::BUTTON_JUMP,m_ActivePlayer) ){
            Release(CrInput::BUTTON_JUMP);
            m_ActivePlayer = -1;
        }
    }
}
