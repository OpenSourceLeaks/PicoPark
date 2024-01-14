/*!
 * スクロールを制御するレンジ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_SCROLL_LIMIT_RANGE_H_
#define _INCLUDED_SIDE_ACTION_SCROLL_LIMIT_RANGE_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionScrollLimitRange : public ActionActor
{
public:
    
    typedef SideActionScrollLimitRange Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionScrollLimitRange( TbFloat32 width );

    // デストラクタ
    virtual ~SideActionScrollLimitRange();

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

private:

    TbFloat32 m_Width;
    TbUint32  m_Phase;

};

#endif
