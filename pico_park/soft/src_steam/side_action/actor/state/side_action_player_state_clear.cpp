/*!
 * クリア時用プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_clear.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "common/map/action_map.h"
#include "system/sys_input.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.14
 */
SideActionPlayerStateClear::SideActionPlayerStateClear()
    : m_State(STATE_INIT)
    , m_WaitTimer(1.0f)
{
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerStateClear::~SideActionPlayerStateClear()
{
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayerStateClear::Update( SideActionPlayer& player , TbFloat32 deltatime )
{
    switch( m_State )
    {
    case STATE_INIT:
        {
            player.ResetVelo();
            player.GetMainSprite().SetVisible(TB_FALSE);
            player.GetCollision()->SetActive(TB_FALSE);
            m_State = STATE_WAIT;
        }
        break;
    case STATE_WAIT:
        {
            m_WaitTimer -= deltatime;
            if( m_WaitTimer <= 0.0f ){
                m_State = STATE_IDLE;
            }
        }
        break;
    case STATE_IDLE:
        break;
    };
}

/*!
 * 後処理
 * @author teco
 * @since 2014.06.25
 */
void SideActionPlayerStateClear::Post( SideActionPlayer& player , TbFloat32 deltatime )
{
    switch( m_State )
    {
    case STATE_IDLE:
        {
            if( player.IsPressInput(SysInput::BUTTON_UP) ) {
                ActionCollision* collision = player.GetCollision();
                if( !collision->GetScene()->IsCollidedByPushBackTarget( *collision ) ){
                    player.GetMainSprite().SetVisible(TB_TRUE);
                    player.GetCollision()->SetActive(TB_TRUE);
                    player.RequestState(SideActionPlayer::STATE_DEFAULT);
                }
            }
        }
        break;
    };
}
