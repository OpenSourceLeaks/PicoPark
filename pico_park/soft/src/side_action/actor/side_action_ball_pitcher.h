/*!
 * バウンドボールピッチャー
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_BALL_PITCHER_H_
#define _INCLUDED_SIDE_ACTION_BALL_PITCHER_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionBallPitcher : public CrActionActor
{
public:
    
    typedef SideActionBallPitcher Self;
    typedef CrActionActor Super;

    enum Type
    {
        TYPE_BOUND , 
        TYPE_LASER ,
        TYPE_ENDLESS_LASER , // 競い用
        TYPE_DEAD_LASER    , // ダメージ用
        TYPE_PHYSICS
    };

public:

    // コンストラクタ
    SideActionBallPitcher( Type type );

    // デストラクタ
    virtual ~SideActionBallPitcher();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // 速度設定
    void SetSpeed( TbFloat32 speed ){
        m_Speed = speed;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param ) TB_OVERRIDE;

private:

    // ボール生成
    CrActionActor* createBall(Type type  , TbFloat32 speed , TbVector2 offset );

private:

    Type                    m_Type;
    TbFloat32               m_CreateTimer;
    TbFloat32               m_Speed;
    TbFloat32               m_DefaultSpeed;
    TbFloat32               m_Scale;
    CrActionActorSprite     m_BaseSprite;
    CrActionActor*          m_Ball;
    TbVector2               m_Dir;
    TbStaticString32        m_LaserBallEventTarget;
    TbStaticString32        m_ManagerName;

};

#endif
