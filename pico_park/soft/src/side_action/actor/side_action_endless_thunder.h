/*!
* 無限サンダー
* @author teco
*/

#ifndef _INCLUDED_SIDE_ACTION_ENDLESS_THUNDER_H_
#define _INCLUDED_SIDE_ACTION_ENDLESS_THUNDER_H_

#include "side_action_thunder.h"

class SideActionEndlessThunder : public CrActionActor
{
public:

    typedef SideActionEndlessThunder Self;
    typedef CrActionActor Super;
    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // コンストラクタ
    SideActionEndlessThunder();

    // デストラクタ
    virtual ~SideActionEndlessThunder();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

protected:

    // 開始時
    virtual void OnEnter( void* userData ) TB_OVERRIDE;

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

private:

    CrActionActor*      m_Thunder[2];
    TbUint32            m_CurrentThunder;
    CmnDir              m_Dir;

};

#endif
