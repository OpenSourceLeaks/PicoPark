/*!
 * マーカー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_MARKER_H_
#define _INCLUDED_SIDE_ACTION_MARKER_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionMarker : public ActionActor
{
public:
    
    typedef SideActionMarker Self;
    typedef ActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_LEFT , 
        TYPE_RIGHT ,
        TYPE_MAX
    };

public:

    // コンストラクタ
    SideActionMarker( Type type );

    // デストラクタ
    virtual ~SideActionMarker();

public:

    TbBitArray32        m_BitArray;

};

#endif
