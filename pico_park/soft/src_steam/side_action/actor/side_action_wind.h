/*!
 * 風
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_WIND_H_
#define _INCLUDED_SIDE_ACTION_WIND_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "common/actor/action_actor_sprite.h"
#include "common/stage/action_stage.h"
#include "side_action/side_action_types.h"

class SideActionWind : public ActionActor
{
public:

    typedef ActionActor Super;
    typedef SideActionWind Self;

public:

    // コンストラクタ
    SideActionWind();

    // デストラクタ
    virtual ~SideActionWind();

public:

    // パワー設定
    void SetPower( TbFloat32 power ){
        m_Power = power;
    }

public:

    // シーンに入った
    void OnEnter( void* userData );
    
    // シーンから出た
    void OnExit();

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

private:

    TbFloat32               m_Power;
    ActionActorSprite       m_SubSprite;
    TbSpriteNodeTree        m_SubSpriteNode;
    TbSpriteNodeAnimTree    m_SubSpriteAnim;
};

#endif
