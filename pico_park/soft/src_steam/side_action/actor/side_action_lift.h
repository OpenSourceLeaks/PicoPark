/*!
 * リフト
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LIFT_H_
#define _INCLUDED_SIDE_ACTION_LIFT_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionLift : public ActionActor
{
public:

    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionLift();

    // デストラクタ
    virtual ~SideActionLift();

};

#endif
