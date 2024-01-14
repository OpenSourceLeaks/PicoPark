/*!
 * マーカー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TITLE_H_
#define _INCLUDED_SIDE_ACTION_TITLE_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionTitle : public ActionActor
{
public:
    
    typedef SideActionTitle Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionTitle();

    // デストラクタ
    virtual ~SideActionTitle();

private:

    ActionActorSprite   m_TwitterSprite;

};

#endif
