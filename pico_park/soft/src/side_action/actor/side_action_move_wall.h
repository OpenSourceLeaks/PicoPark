/*!
 * 動く壁箱
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_MOVE_WALL_H_
#define _INCLUDED_SIDE_ACTION_MOVE_WALL_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionMoveWall : public CrActionActor
{
public:

    typedef SideActionMoveWall Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionMoveWall( TbFloat32 movWidth , TbFloat32 triggerWidth , TbFloat32 height = -1.0f );

    // デストラクタ
    virtual ~SideActionMoveWall();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetPos = pos - GetInitPos();
        if( m_PushBackCount == 0 ) {
            m_PushBackCount = 1;
        }
    }

private:

    enum State
    {
        STATE_IDLE    ,
        STATE_WAIT    ,
        STATE_MOVE    ,
        STATE_RETURN  ,
    };

private:

    // コリジョン衝突コールバック
    void onCollidedCollision(CrActionCollision* collision);

    // 状態設定
    void setState( State state ){
        m_State = state;
        m_StateTimer = 0.0f;
    }

private:

    State               m_State;
    TbFloat32           m_StateTimer;
    TbVector2           m_OffsetPos;
    TbFloat32           m_MoveWidth;      // 移動幅
    TbUint32            m_PushBackCount;

};

#endif
