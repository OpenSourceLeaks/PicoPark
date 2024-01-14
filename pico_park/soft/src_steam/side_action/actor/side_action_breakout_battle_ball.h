/*!
 * バトルブロック崩し用ボール
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BREAKOUT_BATTLE_BALL_H_
#define _INCLUDED_SIDE_ACTION_BREAKOUT_BATTLE_BALL_H_

#include "breakout/actor/breakout_ball.h"

class SideActionBreakoutBattleBall : public BreakoutBall
{
public:
    
    typedef SideActionBreakoutBattleBall Self;
    typedef BreakoutBall Super;

    enum Type
    {
        TYPE_BREAKOUT ,
        TYPE_TERRITORY
    };

public:

    // コンストラクタ
    SideActionBreakoutBattleBall();

    // デストラクタ
    virtual ~SideActionBreakoutBattleBall();

public:

    // 対象プレイヤー設定
    void SetPlayerIndex( TbUint32 playerIndex );

    // タイプ設定
    void SetType( Type type ) {
        m_Type = type;
    }

public:

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

protected:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // マップチップヒット
    virtual void OnHitMapChip( TbSint32 x , TbSint32 y );

    // コリジョンヒット
    virtual void OnHitCollision( ActionCollision* collision );

private:

    TbSint32 m_PlayerIndex;
    Type     m_Type;

};

#endif
