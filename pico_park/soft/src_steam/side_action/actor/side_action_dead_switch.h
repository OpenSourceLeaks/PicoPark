/*!
 * 死のスイッチ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_DEAD_SWITCH_H_
#define _INCLUDED_SIDE_DEAD_SWITCH_H_

#include "side_action_switch.h"

// スイッチ
class SideActionDeadSwitch : public SideActionSwitch
{
public:
    
    typedef SideActionDeadSwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionDeadSwitch();

    // デストラクタ
    virtual ~SideActionDeadSwitch();

protected:

    // スイッチ状態変化
    virtual void OnChangeSwitchState( TbBool isOn ) TB_OVERRIDE;

private:

    ActionActorSprite   m_WarnSprite;

};

#endif
