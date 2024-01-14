/*!
 * ワープスイッチ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_WARP_SWITCH_H_
#define _INCLUDED_SIDE_WARP_SWITCH_H_

#include "side_action_switch.h"

// スイッチ
class SideActionWarpSwitch : public SideActionSwitch
{
public:
    
    typedef SideActionWarpSwitch Self;
    typedef SideActionSwitch Super;

public:

    // コンストラクタ
    SideActionWarpSwitch();

    // デストラクタ
    virtual ~SideActionWarpSwitch();

protected:

    // サブターゲット設定
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        enum Param
        {
            PARAM_TARGET ,
            PARAM_COUNT
        };
        if (PARAM_TARGET < userData.GetCount()) {
            m_TargetName = userData.AsString(PARAM_TARGET);
        }
        if (PARAM_COUNT < userData.GetCount()) {
            m_TargetCount = userData.AsSint32(PARAM_COUNT);
        }
    }

    // スイッチ状態変化
    virtual void OnChangeSwitchState( TbBool isOn ) TB_OVERRIDE;

private:

    TbStaticString512   m_TargetName;
    TbSint32            m_TargetCount;

};

#endif
