/*!
 * メインメニューシーケンス
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_MAIN_MENU_H_
#define _INCLUDED_SEQ_MAIN_MENU_H_

#include "seq_base.h"
#include "side_action/stage/side_action_stage.h"
#include "main_menu/main_menu_slide.h"
#include "common/cmn_def.h"

#include <graphics/sprite/tb_sprite_all.h>
#include <base/container/tb_static_array.h>
#include <sound/tb_sound_source.h>

class SeqMainMenu : public SeqBase
{
public:
    typedef SeqMainMenu Self;
    typedef SeqBase Super;

public:
    
    // コンストラクタ
    SeqMainMenu();
    
    // デストラクタ
    virtual ~SeqMainMenu();
    
public:

    // 準備処理(準備完了したらRESULT_FINISHを返す)
    virtual Result OnPrepare() { return RESULT_FINISH; };
    
    // 更新関数
    virtual void OnUpdate();

    // 描画関数
    virtual void OnDraw();

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) {
        if( m_Stage ) {
            m_Stage->OnCreateDebugMenu(parent);
        }
    }

private:

    enum Phase
    {
        PHASE_BEGIN ,
        PHASE_WAIT_PAGE = 10
    };

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
    SideActionStage*                m_Stage;
    TbUint32                        m_PlayerCount;

private:
    MainMenuSlide                   m_Slide;
};

#endif
