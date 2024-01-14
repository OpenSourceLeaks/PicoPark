/*!
 * エンディングシーケンス
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_ENDING_H_
#define _INCLUDED_SEQ_ENDING_H_

#include "seq_base.h"
#include "common/cmn_simple_sprite.h"

class SeqEnding : public SeqBase
{
public:
    typedef SeqEnding Self;
    typedef SeqBase Super;
public:

    // コンストラクタ
    SeqEnding();

    // デストラクタ
    virtual ~SeqEnding();

public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();
    
private:

    TbUint32        m_Phase;
    TbFloat32       m_PhaseSec;
    CmnSimpleSprite m_Sprite;

};

#endif
