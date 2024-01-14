/*!
 * 特定のプレイヤーが押さないと動かない箱
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_COLOR_BOX_H_
#define _INCLUDED_SIDE_ACTION_COLOR_BOX_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"
#include "common/stage/action_stage.h"
#include <base/util/tb_dynamic_cast.h>

class SideActionColorBox : public ActionActor
{
public:

    typedef ActionActor         Super;
    typedef SideActionColorBox  Self;

public:

    // コンストラクタ
    SideActionColorBox( const TbRectF32& rect , TbSint32 playerIndex , TbSint32 exceptPlayerIndex);

    // コンストラクタ
    SideActionColorBox(const TbRectF32& rect, TbSint32 playerIndex);

    // デストラクタ
    virtual ~SideActionColorBox();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // 後処理
    virtual void OnPostAction(TbFloat32 deltatime) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);
    
private:

    // 初期化
    void initialize(const TbRectF32& rect);

    // 任意の方向から押されているか
    TbBool isPushed( CmnContactFace type );

    // 押している数
    TbUint32 isPushsdByColor( ActionActor* actor , CmnContactFace type );

private:
    
    ActionActorElasticSprite m_Sprite;
    TbRectF32                m_Rect;
    TbSint32                 m_PlayerIndex;
    TbVector2                m_RequestedPos;

};

#endif
