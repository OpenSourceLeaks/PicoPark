/*!
 * 一時的にいなくする時用プレイヤー
 * @author teco
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_null.h"
#include "side_action/actor/side_action_player.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerStateNull::SideActionPlayerStateNull()
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerStateNull::~SideActionPlayerStateNull()
{
}

/*! 
 * 後始末
 * @author teco
 */
void SideActionPlayerStateNull::Initialize(SideActionPlayer& player)
{
    player.ResetVelo();
    player.GetMainSprite().SetVisible(TB_FALSE);
    player.GetCollision()->SetActive(TB_FALSE);
}

/*! 
 * 後始末
 * @author teco
 */
void SideActionPlayerStateNull::Finalize(SideActionPlayer& player)
{
    player.GetMainSprite().SetVisible(TB_TRUE);
    player.GetCollision()->SetActive(TB_TRUE);
}
