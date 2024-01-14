/*!
 * 重さで動く台
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_VARIABLE_WEIGHTED_LIFT_H_
#define _INCLUDED_SIDE_ACTION_VARIABLE_WEIGHTED_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionVariableWeightedLift : public CrActionActor
{
public:

    typedef SideActionVariableWeightedLift Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionVariableWeightedLift();

    // デストラクタ
    virtual ~SideActionVariableWeightedLift();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

protected:

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetPos = pos - GetInitPos();
    }

private:

    // 移動に必要なパーセント設定
    TbUint32 getMoveContactCount();

private:

    TbVector2   m_OffsetPos;
    TbFloat32   m_WaitTimer;
    TbFloat32   m_Height;
    TbUint32    m_MoveContactCountParcent;
    TbUint32    m_ContactCount;
    TbFloat32   m_Speed;
    TbFloat32   m_ReturnSpeed;
    TbUint32    m_IsEnableReturn;
    CrActionActorElasticSprite  m_Sprite;

};

#endif
