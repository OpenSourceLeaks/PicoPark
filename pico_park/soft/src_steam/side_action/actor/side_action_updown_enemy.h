/*!
 * 上下運動エネミー
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_UPDOWN_ENEMY_H_
#define _INCLUDED_SIDE_ACTION_UPDOWN_ENEMY_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionUpDownEnemy : public ActionActor
{
public:

    typedef SideActionUpDownEnemy Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionUpDownEnemy();

    // デストラクタ
    virtual ~SideActionUpDownEnemy();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum {
        STATE_BEGIN ,
        STATE_WAIT_DOWN = STATE_BEGIN , // 下で待機
        STATE_UP , // 上に上る
        STATE_WAIT_UP , // 上で待機
        STATE_DOWN // 下に下がる
    };

private:

    void setState( TbUint32 state ){
        m_State = state;
        m_Timer = 0.0f;
    }

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

private:

    TbUint32    m_State;
    TbFloat32   m_Timer;
    TbFloat32   m_OffsetY;

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

#endif
