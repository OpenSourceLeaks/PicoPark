/*!
 * 縄跳び
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTOR_SKIPPING_ROPE_H_
#define _INCLUDED_ACTOR_SKIPPING_ROPE_H_

#include <crayon/actor/cr_actor.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>

class SideActionSkippingRope : public CrActor
{
public:

    typedef SideActionSkippingRope Self;
    typedef CrActor Super;

public:

    // コンストラクタ
    SideActionSkippingRope();

    // デストラクタ
    virtual ~SideActionSkippingRope();

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime );
    
    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

private:

    // 描画
    void draw( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbSpriteSceneHooker m_SpriteHooker;         // スプライト処理のフックオブジェクト
    TbPointS32          m_Start;
    TbPointS32          m_End;

};

#endif
