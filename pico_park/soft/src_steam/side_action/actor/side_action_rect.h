/*!
 * シンプルな矩形
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_RECT_H_
#define _INCLUDED_SIDE_ACTION_RECT_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionRect : public ActionActor
{
public:
    
    typedef SideActionRect Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionRect( const TbRectF32& rect );

    // デストラクタ
    virtual ~SideActionRect();

private:

    ActionActorElasticSprite m_Sprite;
    TbRectF32                m_Rect;

};

#endif
