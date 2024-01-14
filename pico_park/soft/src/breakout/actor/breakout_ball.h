/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_BALL_H_
#define _INCLUDED_BREAKOUT_BALL_H_

#include <crayon/actor/cr_action_actor.h>

class BreakoutBall : public CrActionActor
{
public:
    
    typedef BreakoutBall Self;
    typedef CrActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

public:

    // コンストラクタ
    BreakoutBall(TbBool isBattle);

    // デストラクタ
    virtual ~BreakoutBall();

public:

    // 死亡時コールバック
    void SetDeadCallback( const DeadCallback& callback ) {
        m_DeadCallback = callback;
    }

    // 死ぬことができるか
    void SetEnableDead( TbBool isEnable ) {
        m_IsEnableDead = isEnable;
    }

    // カラー設定
    void SetColor( const TbColor& color ) {
        GetMainSprite().SetColor(color);
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

protected:

    // マップチップヒット
    virtual void OnHitMapChip( TbSint32 x , TbSint32 y ) {}

    // コリジョンヒット
    virtual void OnHitCollision(CrActionCollision* collision ) {}

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal, CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision);

    // SE再生
    void playSE( TbVector2 velo );

private:

    TbFloat32           m_Friction;
    TbFloat32           m_Speed;
    TbFloat32           m_SpeedScale;
    TbBool              m_IsEnableDead;
    TbSpriteNodeTree    m_SpriteNode;
    TbUint32            m_DeadCount;
    DeadCallback        m_DeadCallback;
    TbUint32            m_SoundTimer;

};

#endif
