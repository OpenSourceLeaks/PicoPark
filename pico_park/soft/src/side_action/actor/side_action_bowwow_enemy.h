/*!
 * バウワウ(早く動いているやつを襲う)
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BOWWOW_ENEMY_H_
#define _INCLUDED_SIDE_ACTION_BOWWOW_ENEMY_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionBowwowEnemy : public CrActionActor
{
public:

    typedef SideActionBowwowEnemy Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionBowwowEnemy();

    // デストラクタ
    virtual ~SideActionBowwowEnemy();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum State
    {
        STATE_SLEEP , 
        STATE_SLEEP_WARN ,
        STATE_WAKE ,
        STATE_ATTACK ,
        STATE_ATTACK_AFTER,
        STATE_MAX
    };

    enum Anim
    {
        ANIM_SLEEP ,
        ANIM_SLEEP_WARN ,
        ANIM_ANGRY ,
        ANIM_MAX
    };

private:

    // 接触開始コールバック
    void onCollided(CrActionCollision*);

    // 
    void updateOffset( TbFloat32 deltatime );

    // 状態設定
    void setState( State state );

private:

    State                   m_State;
    TbFloat32               m_StateSec;
    TbFloat32               m_Sec;
    TbVector2               m_AttackVelo;
    TbFloat32               m_AttackSpeed;
    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim[ANIM_MAX];

    TbSpriteNodeTree        m_SleepSpriteNode;
    TbSpriteNodeAnimTree    m_SleepSpriteAnim;
    CrActionActorSprite     m_SleepSprite;

};

#endif
