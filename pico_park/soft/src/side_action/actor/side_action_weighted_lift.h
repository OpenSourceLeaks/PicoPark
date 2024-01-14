/*!
 * 重さで動く台
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_WEIGHTED_LIFT_H_
#define _INCLUDED_SIDE_ACTION_WEIGHTED_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionWeightedLift : public CrActionActor
{
public:

    enum Type
    {
        TYPE_NORMAL , 
        TYPE_ONE  // 一人用
    };

public:

    typedef SideActionWeightedLift Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionWeightedLift( TbFloat32 collisionOffsetHeight = 0.0f , Type type = TYPE_NORMAL );

    // デストラクタ
    virtual ~SideActionWeightedLift();

public:

    // 高さ設定
    void SetHeight( TbFloat32 height );
    
    // 速度設定
    void SetSpeed( TbFloat32 speed ) {
        m_Speed = speed;
        m_ReturnSpeed = speed;
    }

    // 速度加算
    void AddSpeed(TbFloat32 speed) {
        m_ReturnSpeed += speed;
    }

    // 動き接触数
    void SetMoveContactCountParcent( TbUint32 parcent, TbUint32 min = 0 );

    // 戻れるか
    void SetEnableReturn(TbBool isEnable) {
        m_IsEnableReturn = isEnable;
    }

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

    Type        m_Type;
    TbVector2   m_OffsetPos;
    TbFloat32   m_Height;
    TbFloat32   m_WaitTimer;
    TbUint32    m_MoveContactCount;
    TbUint32    m_ContactCount;
    TbFloat32   m_Speed;
    TbFloat32   m_ReturnSpeed;
    TbUint32    m_IsEnableReturn;

private:

    // 前景描画
    void onDrawCount( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    CrActionActorSprite   m_DirSPrite;
    TbSpriteSceneHooker m_SpriteHooker;

};

#endif
