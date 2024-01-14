/*!
 * バトルリザルトシーケンス
 * @author teco
 */

#ifndef _INCLUDED_SEQ_BATTLE_RESULT_H_
#define _INCLUDED_SEQ_BATTLE_RESULT_H_

#include "seq_base.h"
#include "common/cmn_simple_sprite.h"
#include "battle/battle_result_slide.h"
#include "ui/ui_message_window.h"

class SeqBattleResult : public SeqBase
{
public:
    typedef SeqBattleResult Self;
    typedef SeqBase Super;
public:

    // コンストラクタ
    SeqBattleResult(TbSint32 arg );

    // デストラクタ
    virtual ~SeqBattleResult();

public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();

private:

    enum
    {
        PHASE_BEGIN , 
        PHASE_RESULT = 10 ,
        PHASE_TOTAL_WINNER =  20 ,
    };

private:

    void setPhase(TbSint32 phase) {
        m_Phase = phase;
        m_PhaseTime = 0.0f;
    }

    void addPhase() {
        ++m_Phase;
        m_PhaseTime = 0.0f;
    }

private:

    TbBitArray32        m_BitArray;
    TbSint32            m_Phase;
    TbFloat32           m_PhaseTime;
    BattleResultSlide   m_Slide;
    TbUint32            m_TotalWinner;
    UiMessageWindow     m_TotalWinnerWindow;

};

#endif
