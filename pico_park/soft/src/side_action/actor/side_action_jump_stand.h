/*!
 * ジャンプ台
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_JUMP_STAND_H_
#define _INCLUDED_SIDE_ACTION_JUMP_STAND_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionJumpStand : public CrActionActor
{
public:
    
    typedef SideActionJumpStand Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionJumpStand( const TbVector2& jumpVelo );

    // デストラクタ
    virtual ~SideActionJumpStand();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 接触開始コールバック
    void onContactAdded(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info);

private:

    TbVector2               m_JumpVelo;
    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

#endif
