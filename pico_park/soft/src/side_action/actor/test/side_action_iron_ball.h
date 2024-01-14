/*!
 * 鉄球
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTOR_IRON_ENEMY_H_
#define _INCLUDED_SIDE_ACTOR_IRON_ENEMY_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_actor.h>

class SideActionIronBall : public CrActor
{
public:

    typedef CrActor Super;

public:

    // コンストラクタ
    SideActionIronBall();

    // デストラクタ
    virtual ~SideActionIronBall();

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime );
    
    // 描画更新
    virtual void OnDraw( TbFloat32 deltatime );

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

public:

    TbSprite         m_Sprite;
    TbSpriteNodeTree m_SpriteNode;

};

#endif
