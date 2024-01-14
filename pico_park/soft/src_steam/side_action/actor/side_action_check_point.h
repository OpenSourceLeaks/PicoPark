/*!
 * チェックポイント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_CHECK_POINT_H_
#define _INCLUDED_SIDE_ACTION_CHECK_POINT_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionCheckPoint : public ActionActor
{
public:
    
    typedef SideActionCheckPoint Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionCheckPoint();

    // デストラクタ
    virtual ~SideActionCheckPoint();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    enum State
    {
        STATE_READY , 
        STATE_REQ_END ,
        STATE_END ,
    };
    
    enum Flag
    {
        FLAG_CHECK
    };

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

public:

    TbBitArray32        m_BitArray;

};

#endif
