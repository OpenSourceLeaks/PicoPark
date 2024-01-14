/*!
 * 移動エネルギー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_MOVE_ENERGY_H_
#define _INCLUDED_SIDE_ACTION_MOVE_ENERGY_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/actor/cr_action_actor_sprite.h>

class SideActionPlayer;

class SideActionMoveEnergy : public CrActionActor
{
public:

    enum
    {
        TYPE_MOVE ,
        TYPE_STOP
    };

public:
    
    typedef SideActionMoveEnergy Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionMoveEnergy();

    // デストラクタ
    virtual ~SideActionMoveEnergy();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // シーンに入った
    virtual void OnEnter( void* userData ) TB_OVERRIDE;
    
    // シーンから出た
    virtual void OnExit() TB_OVERRIDE;

    // 前景描画
    void OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbSpriteSceneHooker m_SpriteHooker;

private:

    TbFloat32           m_Energy;
    TbFloat32           m_CosumeEnergy;
    TbFloat32           m_RecoverEnergy;

};

#endif
