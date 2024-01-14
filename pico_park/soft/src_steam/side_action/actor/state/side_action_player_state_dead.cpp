/*!
 * タイトル画面用プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_dead.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "common/map/action_map.h"
#include "system/sys_input.h"

static TB_NDEBUG_CONST TbFloat32 SPEED = 3.0f;

static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 1.0f;
static TB_NDEBUG_CONST TbFloat32 JUMP_VELO = -9.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.14
 */
SideActionPlayerStateDead::SideActionPlayerStateDead()
    : m_State(STATE_INIT)
    , m_WaitTimer(0.0f)
{
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerStateDead::~SideActionPlayerStateDead()
{
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayerStateDead::Update( SideActionPlayer& player , TbFloat32 deltatime )
{
    switch( m_State )
    {
    case STATE_INIT:
        {
            player.SetVelo(TbVector2());
            player.GetCollision()->SetActive(TB_FALSE);
            player.SetAnim(SideActionPlayer::ANIM_DEAD);
            // コリジョン設定を外す
            m_State = STATE_WAIT;
        }
        break;
    case STATE_WAIT:
        {
            // 数秒間待機
            m_WaitTimer += deltatime;
            if( m_WaitTimer >= WAIT_TIME ){
                m_State = STATE_DEAD;
                player.SetVeloY(JUMP_VELO);
            }
        }
        break;
    case STATE_DEAD:
        {
            // 画面外から消えたらNULL状態にする
            player.AddVelo(0.0f,SIDE_ACTION_GRAVITY);
            if( player.GetPosY() > CMN_WINDOW_HEIGHT*1.5f ){
                player.GetStage()->RequestRetry();
                player.RequestState(SideActionPlayer::STATE_NULL);
            }
        }
        break;
    };
}
