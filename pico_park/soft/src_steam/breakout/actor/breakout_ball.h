/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_BREAKOUT_BALL_H_
#define _INCLUDED_BREAKOUT_BALL_H_

#include "common/actor/action_actor.h"

class BreakoutBall : public ActionActor
{
public:
    
    typedef BreakoutBall Self;
    typedef ActionActor Super;

    // 消滅時コールバック
    typedef TbStaticFunction32<void ()> DeadCallback;

public:

    // コンストラクタ
    BreakoutBall();

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

protected:

    // マップチップヒット
    virtual void OnHitMapChip( TbSint32 x , TbSint32 y ) {}

    // コリジョンヒット
    virtual void OnHitCollision( ActionCollision* collision ) {}

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& collision);

private:

    TbFloat32           m_Friction;
    TbBool              m_IsEnableDead;
    TbSpriteNodeTree    m_SpriteNode;
    TbUint32            m_DeadCount;
    DeadCallback        m_DeadCallback;

};

#endif
