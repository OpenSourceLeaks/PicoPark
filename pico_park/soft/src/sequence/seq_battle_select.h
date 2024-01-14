/*!
 * ワールド選択シーケンス
 * @author teco
 */

#ifndef _INCLUDED_SEQ_BATTLE_SELECT_H_
#define _INCLUDED_SEQ_BATTLE_SELECT_H_

#include "seq_base.h"
#include "common/cmn_simple_sprite.h"
#include "world_select/battle_stage_select_slide.h"

class SeqBattleSelect : public SeqBase
{
public:
    typedef SeqBattleSelect Self;
    typedef SeqBase Super;
public:

    // コンストラクタ
    SeqBattleSelect(TbSint32 arg );

    // デストラクタ
    virtual ~SeqBattleSelect();

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
        PHASE_STAGE_SELECT = 10
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
    BattleStageSelectSlide    m_Slide;

};

#endif
