/*!
 * スコア
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_SCORE_BASE_H_
#define _INCLUDED_SIDE_ACTION_SCORE_BASE_H_

#include <crayon/actor/cr_action_actor.h>
#include "ui/ui_message_window.h"

class SideActionScoreBase : public CrActionActor
{
public:
    
    typedef SideActionScoreBase Self;
    typedef CrActionActor Super;

public:

    enum Type
    {
        TYPE_SCORE ,
        TYPE_TIME  
    };

public:

    // コンストラクタ
    SideActionScoreBase();

    // デストラクタ
    virtual ~SideActionScoreBase();

public:

    // 活動状態か
    TbBool IsActive() const {
        return m_Phase != PHASE_IDLE;
    }
    
    // 終了テキスト設定
    void SetFinishMessage( const char* text ){
        m_FinishMessage = text;
    }

    // 結果スコア設定
    void SetResultScore(TbSint32 score)
    {
        m_Result = score;
        m_Type = TYPE_SCORE;
    }

    // 結果時間設定
    void SetResultTime(TbSint32 msec)
    {
        m_Result = msec;
        m_Type = TYPE_TIME;
    }

    // 新記録
    void SetNewRecord() {
        m_IsNewRecord = TB_TRUE;
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
        PHASE_FINISH = 10 ,
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

    Type                        m_Type;
    TbSint32                    m_Result;
    TbBool                      m_IsNewRecord;
    TbSint32                    m_Phase;
    TbFloat32                   m_PhaseSec;
    CrActionActorSpriteHooker   m_SpriteHooker;

    TbStaticString64            m_FinishMessage;
    TbFloat32                   m_FinishMessageX;

    TbUiImageBox                m_IconNew;
    UiMessageWindow             m_MessageWindow;
    UiMessageWindow             m_ConfirmWindow;

};

#endif
