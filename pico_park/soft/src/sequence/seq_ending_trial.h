/*!
 * エンディングシーケンス(体験版用)
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_ENDING_TRIAL_H_
#define _INCLUDED_SEQ_ENDING_TRIAL_H_

#include "seq_base.h"
#include "common/cmn_simple_sprite.h"
#include "side_action/stage/side_action_stage.h"

class SeqEndingTrial : public SeqBase
{
public:
    typedef SeqEndingTrial Self;
    typedef SeqBase Super;
public:

    // コンストラクタ
    SeqEndingTrial();

    // デストラクタ
    virtual ~SeqEndingTrial();

public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };

    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();
    
private:

    TbUint32            m_Phase;
    TbFloat32           m_PhaseSec;
    CmnSimpleSprite     m_Sprite;

};

#endif
