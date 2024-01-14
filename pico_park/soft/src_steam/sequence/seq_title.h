/*!
 * タイトルシーケンス
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SEQ_TITLE_H_
#define _INCLUDED_SEQ_TITLE_H_

#include "seq_base.h"
#include <graphics/sprite/tb_sprite_all.h>
#include <base/container/tb_static_array.h>
#include "side_action/stage/side_action_stage.h"

#include "ui/ui_message_window.h"
#include "title/ui/ui_title_select_window.h"
#include "common/ui/ui_gamepad_config_window.h"
#include "common/ui/ui_keyboard_config_window.h"
#include "common/ui/ui_option_window.h"

#include <sound/tb_sound_source.h>


#include "common/cmn_def.h"

class SeqTitle : public SeqBase
{
public:
    typedef SeqTitle Self;
    typedef SeqBase Super;

public:
    
    // コンストラクタ
    SeqTitle( TbBool isEnableSkipAdvertise , TbBool isEnableSelectPlayer );
    
    // デストラクタ
    virtual ~SeqTitle();
    
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

    enum SpriteType
    {
        SPRITE_TYPE_ADVERTISE , // アドバタイズ
        SPRITE_TYPE_MAX 
    };

private:

    enum Flag
    {
        FLAG_SKIP_SELECT_CHARA , 
    };

    enum Phase
    {
        PHASE_BEGIN ,
        PHASE_ADVERTISE = PHASE_BEGIN ,
        PHASE_TITLE = 10 ,
        PHASE_TITLE_CHARA_SELECT = 20 , 
        PHASE_TITLE_CONFIRM      = 30 , 
        PHASE_TITLE_GAMEPAD_CONFIG = 40 ,
        PHASE_TITLE_KEYBOARD_CONFIG = 50 ,
        PHASE_TITLE_ENTER_DOOR   = 60 , 
        PHASE_TITLE_NEXT_STAGE = 70 ,
        PHASE_TITLE_RESET = 80 ,
        PHASE_TITLE_OPTION = 90
    };

    struct Sprite
    {
        TbSprite           sprite;
        TbSpriteNodeTree   node;
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
    Sprite                          m_Sprite[SPRITE_TYPE_MAX];
    toybox::TbSpriteFixedMaterial   m_SpriteMaterial;   // スプライト用マテリアル
    SideActionStage*                m_Stage;
    TbRectF32                       m_PressTextRect;

    UiMessageWindow                 m_ConfirmWindow;
    UiTitleSelectWindow             m_SelectWindow;
    UiGamePadConfigWindow           m_GamePadConfigWindow;
    UiKeyboardConfigWindow          m_KeyboardConfigWindow;
    UiOptionWindow                  m_OptionWindow;

    TbUint32                                          m_PlayerCount;
    toybox::TbStaticArray<TbSint32,CMN_PLAYER_MAX>    m_PadPlayerIndex;

};

#endif
