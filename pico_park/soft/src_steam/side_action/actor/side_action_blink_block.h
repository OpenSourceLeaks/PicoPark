/*!
 * 点滅ブロック
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BLINK_BLOCK_H_
#define _INCLUDED_SIDE_ACTION_BLINK_BLOCK_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "common/stage/action_stage.h"
#include "side_action/side_action_types.h"

class SideActionBlinkBlock : public ActionActor
{
public:

    typedef ActionActor Super;
    typedef SideActionBlinkBlock Self;

public:

    // コンストラクタ
    SideActionBlinkBlock();

    // デストラクタ
    virtual ~SideActionBlinkBlock();

public:

    // 点滅感覚
    void SetBlinkInterval( TbFloat32 interval ){
        m_BlinkInterval = interval;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    TbFloat32   m_BlinkInterval;
    TbFloat32   m_Timer;

};

#endif
