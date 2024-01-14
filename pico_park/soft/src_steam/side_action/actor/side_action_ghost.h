/*!
 * 幽霊
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_GHOST_H_
#define _INCLUDED_SIDE_ACTION_GHOST_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "common/actor/action_actor_child.h"
#include "common/stage/action_stage.h"
#include <base/util/tb_dynamic_cast.h>

#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_key.h"
#include "side_action/actor/side_action_lift.h"
#include "side_action/actor/side_action_switch.h"

class SideActionGhost : public ActionActor
{
public:

    typedef ActionActor Super;
    typedef SideActionGhost Self;

public:

    // コンストラクタ
    SideActionGhost();

    // デストラクタ
    virtual ~SideActionGhost();

public:

    // スイッチターゲット設定
    void SetSwitchTarget( const char* targetName );

    // 移動ボーダー設定
    void SetMoveBorder( TbUint32 border ) 
    {
        m_MoveBorder = border;
    }

    // 速度設定
    void SetSpeed( TbFloat32 speed ) {
        m_Speed = speed;
    }

    // 左向き
    virtual void LookLeft() TB_OVERRIDE {
        GetMainSprite().SetScale(-1.0f,1.0f);
    }

    // 左向き
    virtual void LookRight() TB_OVERRIDE {
        GetMainSprite().SetScale(1.0f,1.0f);
    }

    // イベント通知
    TbSint32 NotifyEvent( TbUint32 event , void* param );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum Flag
    {
        FLAG_ACTIVE ,
        FLAG_SWITCH ,
        FLAG_DEAD ,
        FLAG_END
    };

    enum Direction
    {
        DIRECTION_UP    , 
        DIRECTION_RIGHT , 
        DIRECTION_DOWN  , 
        DIRECTION_LEFT  , 
        DIRECTION_MAX
    };
    
private:

    // 自分を見ているプレイヤー数取得
    TbUint32 getLookingPlayerCount() const;

    // 方向設定
    void setDirection( Direction dir );

    // 有効設定
    void setActive( TbBool isActive );

    // 透明度設定
    void setAlpha( TbFloat32 alpha );

private:

    TbBitArray32             m_BitArray;
    TbUint32                 m_MoveBorder;             // 動く境界値
    TbUint32                 m_LookingPlayerCount;
    TbFloat32                m_Sec;
    ActionActorSprite        m_BalloonSprite;          // 吹き出し
    ActionActorSprite        m_DirectionSprite;        // 方向
    TbFloat32                m_DirectionTimer;
    TbFloat32                m_Speed;
    TbFloat32                m_Alpha;                  // 透明度

private:

    Direction                m_Direction;
    

};

#endif
