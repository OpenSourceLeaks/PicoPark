/*!
 * 踏むと倒れるアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_STEP_ENEMY_H_
#define _INCLUDED_SIDE_ACTION_STEP_ENEMY_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionStepEnemy : public ActionActor
{
public:

    typedef SideActionStepEnemy Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionStepEnemy();

    // デストラクタ
    virtual ~SideActionStepEnemy();

public:

    // 左へ向かう
    void GoToLeft() {
        m_Dir.SetX(-1);
        GetMainSprite().SetScale(-1.0f,1.0f);
    }

    // 右へ向かう
    void GoToRight() {
        m_Dir.SetX(1);
        GetMainSprite().SetScale(1.0f,1.0f);
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum State
    {
        STATE_DEFAULT , 
        STATE_DEAD
    };

private:

    // 接触開始コールバック
    void onContactAdded(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info);

private:
    
    TbSint32         m_State;
    TbVector2        m_Dir;

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

#endif
