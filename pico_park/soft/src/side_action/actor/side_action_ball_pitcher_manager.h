/*!
* ボールピッチャーマネージャ
* @author teco
*/

#ifndef _INCLUDED_SIDE_ACTION_BALL_PITCHER_MANAGER_H_
#define _INCLUDED_SIDE_ACTION_BALL_PITCHER_MANAGER_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionBallPitcherManager : public CrActionActor
{
public:

    typedef SideActionBallPitcherManager Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionBallPitcherManager();

    // デストラクタ
    virtual ~SideActionBallPitcherManager();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    enum
    {
        PHASE_TUTORIAL       , // チュートリアル
        PHASE_ACTIVE   = 10  , // 開始
        PHASE_SUSPEND  = 20  , // 
    };

private:

    TbUint32            m_Phase;
    TbBool              m_HasTutorial;
    TbUint32            m_Count;
    TbUint32            m_ActiveRefCount;
    TbStaticString32    m_TargetName;

};

#endif
