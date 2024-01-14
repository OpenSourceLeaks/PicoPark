/*!
 * マップ矩形
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_MAP_RECT_H_
#define _INCLUDED_SIDE_ACTION_MAP_RECT_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionMapRect : public CrActionActor
{
public:
    
    typedef SideActionMapRect Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionMapRect();

    // デストラクタ
    virtual ~SideActionMapRect();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

    // シーンに入った
    virtual void OnEnter(void* userData);

private:

    TbRectS32   m_Rect;

};

#endif
