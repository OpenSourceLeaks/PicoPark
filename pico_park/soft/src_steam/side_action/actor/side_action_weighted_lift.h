/*!
 * 重さで動く台
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_WEIGHTED_LIFT_H_
#define _INCLUDED_SIDE_ACTION_WEIGHTED_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionWeightedLift : public ActionActor
{
public:

    typedef SideActionWeightedLift Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionWeightedLift();

    // デストラクタ
    virtual ~SideActionWeightedLift();

public:

    // 高さ設定
    void SetHeight( TbFloat32 height );
    
    // 速度設定
    void SetSpeed( TbFloat32 speed ) {
        m_Speed = speed;
    }

    // 動き接触数
    void SetMoveContactCountParcent( TbUint32 parcent );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetPos = pos - GetInitPos();
    }

private:

    TbVector2   m_OffsetPos;
    TbFloat32   m_Height;
    TbFloat32   m_WaitTimer;
    TbUint32    m_MoveContactCount;
    TbUint32    m_ContactCount;
    TbFloat32   m_Speed;

private:

    // 前景描画
    void onDrawCount( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    ActionActorSprite   m_DirSPrite;
    TbSpriteSceneHooker m_SpriteHooker;

};

#endif
