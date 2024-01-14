/*!
 * 協力しないと押せない箱
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_BIG_BOX_H_
#define _INCLUDED_SIDE_ACTION_BIG_BOX_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "side_action/side_action_types.h"
#include <base/util/tb_dynamic_cast.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/stage/cr_action_stage.h>
#include "side_action/actor/side_action_warp_object.h"

class SideActionBigBox : public SideActionWarpObject
{
public:

    typedef SideActionWarpObject Super;
    typedef SideActionBigBox Self;

public:

    enum Type
    {
        TYPE_TALL , 
        TYPE_BIG , 
        TYPE_NORMAL , 
        TYPE_SMALL ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    SideActionBigBox( Type type = TYPE_NORMAL );

    // コンストラクタ
    SideActionBigBox( const TbRectF32Pod& rect );

    // デストラクタ
    virtual ~SideActionBigBox();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // 最大押し数
    void SetPushCountMaxParcent( TbUint32 parcent );

public:

    // シーンに入った
    virtual void OnEnter(void* userData) TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param) TB_OVERRIDE;
    
private:

    // 初期化
    void initialize(const TbRectF32Pod& rect);

    // 任意の方向から押されているか
    TbBool isPushed( CrContactFace type );

    // 任意の方向に押すことができるか
    TbBool isEnablePush(CrActionActor* actor, CrContactFace type );

    // 押している数
    TbUint32 getPushCount( CrActionActor* actor , CrContactFace type );

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 押すのに必要な数
    TbUint32 getPushCountBorder() const;

    // 接触開始コールバック
    void onContactAdded(const TbVector2& normal, CrActionCollision::ContactTarget target, const CrActionCollisionInfo& info);

private:

    enum
    {
        FLAG_FORCE_JUMP ,
        FLAG_CATCHED
    };

public:
    
    TbBitArray32                m_BitArray;
    TbUint32                    m_PushCountBorderParcent;
    TbUint32                    m_PushCountBorderOffset;
    TbUint32                    m_PushCount;
    CrActionActorElasticSprite  m_Sprite;
    CrActionActorSpriteHooker   m_SpriteHooker;
    TbRectF32Pod                m_Rect;
    TbVector2                   m_ForceJumpVelo;

};

#endif
