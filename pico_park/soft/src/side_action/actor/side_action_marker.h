﻿/*!
 * マーカー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_MARKER_H_
#define _INCLUDED_SIDE_ACTION_MARKER_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionMarker : public CrActionActor
{
public:
    
    typedef SideActionMarker Self;
    typedef CrActionActor Super;

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