/*!
 * タイマー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_JUDGE_BASE_H_
#define _INCLUDED_SIDE_ACTION_JUDGE_BASE_H_

#include "common/actor/action_actor.h"
#include "ui/ui_message_window.h"

class SideActionJudgeBase : public ActionActor
{
public:
    
    typedef SideActionJudgeBase Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionJudgeBase();

    // デストラクタ
    virtual ~SideActionJudgeBase();

public:

    // 活動状態か
    TbBool IsActive() const {
        return m_Phase != PHASE_IDLE;
    }

    // リザルト待ち中
    TbBool IsWaitingResult() const {
        return m_Phase == PHASE_WAIT_RESULT;
    }
    
    // 優勝決定
    void SetPrizePlayerIndex( TbSint32 playerIndex )
    {
        TB_RESQUE(IsWaitingResult());
        if( 0 <= playerIndex ){
            m_PrizePlayerIndex = playerIndex;
        }else{
            m_IsEven = TB_TRUE;
        }
        setPhase(PHASE_RESULT);
    }

    // 終了テキスト設定
    void SetFinishMessage( const char* text ){
        m_FinishMessage = text;
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
        PHASE_WAIT_RESULT = 20 ,
        PHASE_RESULT = 30 ,
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

    TbUint32                m_PrizePlayerIndex;
    TbStaticString64        m_FinishMessage;
    TbFloat32               m_FinishMessageX;
    TbBool                  m_IsEven;

    UiMessageWindow         m_MessageWindow;

};

#endif
