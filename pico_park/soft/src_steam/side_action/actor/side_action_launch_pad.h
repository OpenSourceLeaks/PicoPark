/*!
 * 発射台
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LAUNCH_PAD_H_
#define _INCLUDED_SIDE_ACTION_LAUNCH_PAD_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionLaunchPad : public ActionActor
{
public:
    
    typedef SideActionLaunchPad Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionLaunchPad( TbFloat32 jumpVelo );

    // デストラクタ
    virtual ~SideActionLaunchPad();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 接触開始コールバック
    void onContactAdded(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info);

private:

    TbVector2               m_JumpVelo;
    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

#endif
