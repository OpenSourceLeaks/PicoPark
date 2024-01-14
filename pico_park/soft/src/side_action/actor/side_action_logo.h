/*!
 * マーカー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LOGO_H_
#define _INCLUDED_SIDE_ACTION_LOGO_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionLogo : public CrActionActor
{
public:
    
    typedef SideActionLogo Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionLogo();

    // デストラクタ
    virtual ~SideActionLogo();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

};

#endif
