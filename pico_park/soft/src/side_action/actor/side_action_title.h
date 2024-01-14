/*!
 * マーカー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TITLE_H_
#define _INCLUDED_SIDE_ACTION_TITLE_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionTitle : public CrActionActor
{
public:
    
    typedef SideActionTitle Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionTitle();

    // デストラクタ
    virtual ~SideActionTitle();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

private:

    CrActionActorSprite   m_TwitterSprite;

};

#endif
