/*!
 * 多数決コントローラ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_MAJORITY_CONTROLLER_H_
#define _INCLUDED_SIDE_ACTION_MAJORITY_CONTROLLER_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include "common/actor/action_actor.h"
#include "common/actor/action_actor_sprite.h"

class SideActionPlayer;

class SideActionMajorityController : public ActionActor
{
public:
    
    typedef SideActionMajorityController Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionMajorityController( SideActionPlayer* player );

    // デストラクタ
    virtual ~SideActionMajorityController();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // 前景描画
    void OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

public:

    TbSpriteSceneHooker m_SpriteHooker;
    SideActionPlayer*   m_Player;

};

#endif
