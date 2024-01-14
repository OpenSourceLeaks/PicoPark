/*!
 * ステージ選択シーケンス
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_STAGE_SELECT_H_
#define _INCLUDED_SEQ_STAGE_SELECT_H_

#include "seq_base.h"
#include <graphics/sprite/tb_sprite_all.h>
#include <crayon/stage/cr_stage.h>
#include "stage_select/stage_select_menu.h"
#include "ui/ui_message_window.h"

#include <sound/tb_sound_source.h>

class SeqStageSelect : public SeqBase
{
public:
    typedef SeqStageSelect Self;
    typedef SeqBase Super;

public:
    
    // コンストラクタ
    SeqStageSelect( TbSint32 focusIndex , TbSint32 count );
    
    // デストラクタ
    virtual ~SeqStageSelect();
    
public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();
    
    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) {}

private:

    void setPhase( TbSint32 phase ) {
        m_Phase = phase;
        m_PhaseTime = 0.0f;
    }

private:
    
    TbSint32            m_Phase;
    TbFloat32           m_PhaseTime;
    TbFloat32           m_IdleSec;
    StageSelectMenu     m_Menu;
    UiMessageWindow     m_ReturnWindow;
    TbSint32            m_InitIndex;

};

#endif
