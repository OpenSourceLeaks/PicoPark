/*!
 * 特定のプレイヤーが押さないと動かない箱
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_COLOR_BOX_H_
#define _INCLUDED_SIDE_ACTION_COLOR_BOX_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include <crayon/stage/cr_action_stage.h>
#include <base/util/tb_dynamic_cast.h>

class SideActionColorBox : public CrActionActor
{
public:

    typedef CrActionActor         Super;
    typedef SideActionColorBox  Self;

public:

    TB_DEFINE_SUB_CLASS(Self,Super)

public:

    // コンストラクタ
    SideActionColorBox( const TbRectF32& rect , TbSint32 playerIndex , TbSint32 exceptPlayerIndex);

    // コンストラクタ
    SideActionColorBox(const TbRectF32& rect, TbSint32 playerIndex);

    // デストラクタ
    virtual ~SideActionColorBox();

public:

    // ジャンプ可能か
    void SetEnableJump(TbBool isEnable ) {
        m_IsEnableJump = isEnable;
        GetCollision()->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // 後処理
    virtual void OnPostAction(TbFloat32 deltatime) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    enum
    {
        FLAG_FORCE_JUMP,
        FLAG_CATCHED
    };
    
private:

    // 初期化
    void initialize(const TbRectF32& rect);

    // 任意の方向から押されているか
    TbBool isPushed( CrContactFace type );

    // 押している数
    TbUint32 isPushsdByColor( CrActionActor* actor , CrContactFace type );

    // ジャンプ可能か
    TbBool isEnableJump() const;

    // 表示設定
    void setVisible(TbBool isVisible);

private:
    
    TbBitArray32                m_BitArray;
    CrActionActorElasticSprite  m_Sprite;
    TbBool                      m_IsEnableJump;
    TbRectF32                   m_Rect;
    TbSint32                    m_PlayerIndex;
    TbUint32                    m_JumpCount;
    TbVector2                   m_RequestedPos;
    TbVector2                   m_ForceJumpVelo;

};

#endif
