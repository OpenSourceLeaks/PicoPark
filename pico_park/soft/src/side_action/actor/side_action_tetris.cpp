/*!
 * マーカー
 * @author teco
 * @since 2014.05.25
 */

#include "fwd.h"
#include "side_action_tetris.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionTetris::SideActionTetris()
    : Super()
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionTetris::~SideActionTetris()
{
}

// ゲームオーバー処理
void SideActionTetris::OnGameOver()
{
//    ReinitializeByScript();
    GetActionStage()->RequestRetry();
}

// クリアー処理
void SideActionTetris::OnClear()
{
    if (!m_SwitchTarget.IsEmpty()) {
        TbSint32 param = 0;
        GetStage()->NotifyEvent(m_SwitchTarget.GetBuffer(), SIDE_ACTION_EVENT_SWITCH_ON, &param);
    }
    GetStage()->NotifyEvent(nullptr, SIDE_ACTION_EVENT_WAKEUP, nullptr);
}

