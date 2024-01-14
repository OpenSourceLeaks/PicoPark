/*!
 * 踏むと倒れるアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_STEP_ENEMY_H_
#define _INCLUDED_SIDE_ACTION_STEP_ENEMY_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionStepEnemy : public CrActionActor
{
public:

    typedef SideActionStepEnemy Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionStepEnemy();

    // デストラクタ
    virtual ~SideActionStepEnemy();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        if (userData.AsSint32(0) < 0) {
            GoToLeft();
        }
    }

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
    void onContactAdded(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info);

private:
    
    TbSint32         m_State;
    TbVector2        m_Dir;

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

#endif
