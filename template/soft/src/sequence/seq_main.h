/*!
 * タイトルシーケンス
 * @author teco
 */

#ifndef _INCLUDED_SEQ_MAIN_H_
#define _INCLUDED_SEQ_MAIN_H_

#include "seq_base.h"

#include "ui/ui_message_window.h"
#include <graphics/sprite/tb_sprite_all.h>
#include <base/container/tb_static_array.h>
#include <sound/tb_sound_source.h>

#include "common/cmn_def.h"

class SeqMain : public SeqBase
{
public:
    typedef SeqMain Self;
    typedef SeqBase Super;

public:
    
    // コンストラクタ
    SeqMain( TbUint32 id );
    
    // デストラクタ
    virtual ~SeqMain();
    
public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();
    
    // 終了処理(終了完了したらRESULT_FINISHを返す)
    virtual Result OnFinish() { return RESULT_FINISH; };

private:

    void setPhase( TbSint32 phase ) {
        m_Phase = phase;
        m_PhaseTime = 0.0f;
    }

    void addPhase() {
        ++m_Phase;
        m_PhaseTime = 0.0f;
    }

private:
    
    TbBitArray32                    m_BitArray;
    TbSint32                        m_Phase;
    TbFloat32                       m_PhaseTime;

};

#endif
