/*!
 * 風
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_WIND_H_
#define _INCLUDED_SIDE_ACTION_WIND_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/actor/cr_action_actor_sprite.h>
#include <crayon/stage/cr_action_stage.h>
#include "side_action/side_action_types.h"
#include "root.h"

class SideActionWind : public CrActionActor
{
public:

    typedef CrActionActor Super;
    typedef SideActionWind Self;

public:

    // コンストラクタ
    SideActionWind();

    // デストラクタ
    virtual ~SideActionWind();

public:

    // セットアップ
    void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        TbSint32 index = TbMax<TbSint32>(1, Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN + 1);
        TbFloat32 power = userData.AsFloat32(index);
        SetPower(power);
    }

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
    void onCollided(CrActionCollision* target);

private:

    TbFloat32               m_Power;
    CrActionActorSprite       m_SubSprite;
    TbSpriteNodeTree        m_SubSpriteNode;
    TbSpriteNodeAnimTree    m_SubSpriteAnim;
};

#endif
