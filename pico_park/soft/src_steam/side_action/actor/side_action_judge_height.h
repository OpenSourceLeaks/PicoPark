/*!
 * タイマー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_HEIGHT_JUDGEMENT_H_
#define _INCLUDED_SIDE_ACTION_HEIGHT_JUDGEMENT_H_

#include "side_action_judge_base.h"
#include "common/actor/action_actor.h"
#include "ui/ui_message_window.h"

class SideActionJudgeHeight : public SideActionJudgeBase
{
public:
    
    typedef SideActionJudgeHeight Self;
    typedef SideActionJudgeBase Super;

public:

    // コンストラクタ
    SideActionJudgeHeight();

    // デストラクタ
    virtual ~SideActionJudgeHeight();

public:

    // 活動状態か
    TbBool IsActive() const {
        return m_Phase != PHASE_IDLE;
    }

public:

    // ロジック更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Phase
    {
        PHASE_IDLE ,
        PHASE_TIMEUP = 10 ,
        PHASE_JUDGE = 20 ,
    };

private:

    // ライン描画
    void onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // フェーズ設定
    void setPhase( TbSint32 phase ){
        m_PhaseSec = 0.0f;
        m_Phase = phase;
    }

private:

    TbSint32                m_Phase;
    TbFloat32               m_PhaseSec;
    ActionActorSpriteHooker m_SpriteHooker;
    TbFloat32               m_PrizeTargetY;

};

#endif
