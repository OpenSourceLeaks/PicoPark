/*!
 * 信号
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TRAFFIC_LIGHT_H_
#define _INCLUDED_SIDE_ACTION_TRAFFIC_LIGHT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_scene_hooker.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/actor/cr_action_actor_sprite.h>

class SideActionPlayer;

class SideActionTrafficLight : public CrActionActor
{
public:
    
    typedef SideActionTrafficLight Self;
    typedef CrActionActor Super;

public:

    enum State
    {
        STATE_BEGIN , 
        STATE_MOVE = STATE_BEGIN , 
        STATE_STOP , 
        STATE_END , 
        STATE_MAX = STATE_END - STATE_BEGIN
    };


public:

    // コンストラクタ
    SideActionTrafficLight();

    // デストラクタ
    virtual ~SideActionTrafficLight();

public:

    // 切り替え時間設定
    void SetSwitchSec( TbFloat32 moveSec , TbFloat32 stopSec ) {
        m_ActiveSec[STATE_MOVE] = moveSec;
        m_ActiveSec[STATE_STOP] = stopSec;
    }

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 前景描画
    void OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    enum Flag
    {
        FLAG_BLINK_OFF ,
    };

private:

    // 
    void setState( State state ){
        TB_RESQUE(STATE_BEGIN<=state&&state<STATE_END);
        m_State = state;
        m_Timer = m_ActiveSec[state];
        m_BlinkTimer = 0.0f;
        m_BitArray.ResetBit(FLAG_BLINK_OFF);
    }

public:

    TbBitArray32            m_BitArray;
    State                   m_State;
    CrActionActorSpriteHooker m_SpriteHooker;
    TbFloat32               m_ActiveSec[STATE_MAX];     // 移動可能時間

    TbFloat32               m_Timer;                    // タイマー
    TbFloat32               m_BlinkTimer;               // 点滅

};

#endif
