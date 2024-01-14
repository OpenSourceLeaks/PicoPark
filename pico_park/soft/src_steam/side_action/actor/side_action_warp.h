/*!
 * ワープゾーン
 * @author teco
 * @since 2015.01.25
 */

#ifndef _INCLUDED_SIDE_ACTION_WARP_H_
#define _INCLUDED_SIDE_ACTION_WARP_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionWarp : public ActionActor
{
public:
    
    typedef SideActionWarp Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionWarp( TbFloat32 width , TbFloat32 height );

    // デストラクタ
    virtual ~SideActionWarp();

public:

    // 移動先
    void SetDestination( const TbVector2& pos ) {
        m_Dest = pos;
    }

    // 移動先を初期値にする
    void SetDestInitPos(TbBool val) {
        m_IsDestInitPos = val;
    }

    // 移動先オフセット
    void SetDestOffset( const TbVector2& pos ) {
        m_DestOffset = pos;
    }

    // ターゲット全員か
    void SetTargetAll(TbBool isTargetAll) {
        m_IsTargetAll = isTargetAll;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

private:

    TbBool    m_IsDestInitPos;
    TbBool    m_IsTargetAll;
    TbVector2 m_Dest;
    TbVector2 m_DestOffset;

};

#endif
