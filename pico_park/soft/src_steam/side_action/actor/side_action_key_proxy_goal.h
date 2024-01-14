/*!
 * キーの代理ゴール
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_KEY_PROXY_GOAL_H_
#define _INCLUDED_SIDE_ACTION_KEY_PROXY_GOAL_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionKeyProxyGoal : public ActionActor
{
public:

    typedef SideActionKeyProxyGoal  Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionKeyProxyGoal();

    // デストラクタ
    virtual ~SideActionKeyProxyGoal();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

private:

    TbBitArray32        m_BitArray;
    ActionActorSprite   m_ArrowSprite;          // 吹き出し


};

#endif
