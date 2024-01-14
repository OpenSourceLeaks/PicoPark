/*!
 * キーボード設定ウィンドウ
 * @author teco
 */

#ifndef _INCLUDED_UI_KEYBOARD_CONFIG_WINDOW_H_
#define _INCLUDED_UI_KEYBOARD_CONFIG_WINDOW_H_

#include "ui/ui_player_window.h"

#include "ui/ui_button.h"
#include "ui/ui_select_box.h"

#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_line.h>
#include <ui/tb_ui_image_box.h>
#include <input/tb_keyboard.h>

class UiKeyboardConfigWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiKeyboardConfigWindow Self;

public:

    enum Result
    {
        RESULT_NONE ,
        RESULT_OK , 
        RESULT_CANCEL
    };

    enum ItemButton
    {
        ITEM_BUTTON_UP    , 
        ITEM_BUTTON_DOWN  , 
        ITEM_BUTTON_LEFT  , 
        ITEM_BUTTON_RIGHT , 
        ITEM_BUTTON_A     , 
        ITEM_BUTTON_B     , 
        ITEM_BUTTON_START , 
        ITEM_BUTTON_MAX
    };

    enum ItemType
    {
        ITEM_BEGIN , 

        ITEM_PLAYER_INDEX = ITEM_BEGIN , // 

        ITEM_KEY_BEGIN ,
        ITEM_KEY_END = ITEM_KEY_BEGIN + ITEM_BUTTON_MAX ,
        ITEM_CHARA_CHANGE = ITEM_KEY_END , 
        ITEM_END ,

        ITEM_MAX = ITEM_END - ITEM_BEGIN
    };

public:
    
    // コンストラクタ
    UiKeyboardConfigWindow();
    
    // デストラクタ
    ~UiKeyboardConfigWindow();

public:

    // リザルト取得
    Result GetResult() const {
        return m_Result;
    }

protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec );

    // 生成時
    virtual void OnCreated();

    // キャンセル
    virtual void OnCancel() TB_OVERRIDE {
        m_Result = RESULT_CANCEL;
        Close();
    }

private:

    struct Item
    {
        enum Type
        {
            TYPE_BUTTON , 
            TYPE_SELECT ,
        };
        Item()
            : type(TYPE_BUTTON)
        {}
        TbBool IsEnable() {
            switch( type )
            {
            case TYPE_BUTTON:
                return button.IsEnable();
            case TYPE_SELECT:
                return TB_TRUE;
            }
            return TB_FALSE;
        }
        void SetEnable( TbBool isEnable ) {
            switch( type )
            {
            case TYPE_BUTTON:
                button.SetEnable(isEnable);
                break;
            case TYPE_SELECT:
                break;
            }
        }
        // フォーカスか
        TbBool IsFocused() {
            switch( type )
            {
            case Item::TYPE_BUTTON:
                return button.IsFocused();
            case Item::TYPE_SELECT:
                return selectBox.IsFocused();
            }
            return TB_FALSE;
        }

        // フォーカスか
        void SetFocus() {
            switch( type )
            {
            case Item::TYPE_BUTTON:
                button.SetFocus();
            case Item::TYPE_SELECT:
                selectBox.SetFocus();
            }
        }
        Type         type;
        TbUiTextBox  caption;
        UiButton     button;
        UiSelectBox  selectBox;
    };

    struct ItemButtonInfo
    {
        ItemButtonInfo() {
            TbFill(buttonKey,TB_KEY_INVALID);
        }
        TbKeyType   buttonKey[ITEM_BUTTON_MAX];
    };

    enum
    {
        BUTTON_OK ,
        BUTTON_CANCEL ,
        BUTTON_DEFAULT ,
        BUTTON_MAX ,
    };

    enum MainFocus
    {
        MAIN_FOCUS_ITEM   ,
        MAIN_FOCUS_BUTTON 
    };
    
    enum Phase
    {
        PHASE_IDLE ,
        PHASE_TRIGGER_INPUT ,
        PHASE_WAIT_INPUT ,
    };

private:

    void fit();

    // 決定時コールバック
    void onDecidedCallback( Result result );

    // リセットボタン押した
    void onPressedResetButton();

    // キーアサイン用ボタン押した
    void onPressedKeyButton();

    // 対象プレイヤー決まった
    void onChangedPlayer( TbSint32 index );

    // フォーカス情報更新
    void updateFocusInfo();

    // ITEM_KEYの名前を更新
    void updateItemKeyName( TbSint32 playerIndex );

private:

    TbSint32            m_Phase;
    TbUiTextBox         m_Title;
    TbUiLine            m_Line[3];
    Item                m_Item[ITEM_MAX];
    UiButton            m_Button[BUTTON_MAX];
    MainFocus           m_MainFocus;
    TbUint32            m_FocusItemIndex;
    TbUint32            m_FocusButtonIndex;
    Result              m_Result;
    ItemButtonInfo      m_ItemButtonInfo[CMN_PLAYER_MAX];

};

#endif
