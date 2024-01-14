/*!
 * ボールピッチャー
 * @author teco
 */

#include "fwd.h"
#include "side_action_ball_pitcher_manager.h"
#include "side_action_player.h"
#include "side_action/side_action_types.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBallPitcherManager::SideActionBallPitcherManager()
    : m_Phase(PHASE_TUTORIAL)
    , m_Count(0)
    , m_ActiveRefCount(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBallPitcherManager::~SideActionBallPitcherManager()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionBallPitcherManager::Setup(const CrActorFactory::UserData& userData)
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBallPitcherManager::OnUpdateAction(TbFloat32 deltatime)
{
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBallPitcherManager::NotifyEvent(TbUint32 event, void* param)
{

    return 0;
}