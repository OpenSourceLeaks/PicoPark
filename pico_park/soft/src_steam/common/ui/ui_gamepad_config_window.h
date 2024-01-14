/*!
 * ポーズウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_GAMEPAD_CONFIG_WINDOW_H_
#define _INCLUDED_UI_GAMEPAD_CONFIG_WINDOW_H_

#include "ui/ui_player_window.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_line.h>
#include <ui/tb_ui_image_box.h>

class UiGamePadConfigWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiGamePadConfigWindow Self;

public:

    enum Result
    {
        RESULT_NONE ,
        RESULT_OK , 
        RESULT_CANCEL
    };

    enum ItemType
    {
        ITEM_BEGIN , 

        ITEM_DEVICE = ITEM_BEGIN , // 

        ITEM_BUTTON_BEGIN ,
        ITEM_A      = ITEM_BUTTON_BEGIN ,
        ITEM_B      ,
        ITEM_START  ,
//        ITEM_OK     ,
//        ITEM_CANCEL ,
        ITEM_BUTTON_END , 

        ITEM_END   = ITEM_BUTTON_END ,

        ITEM_MAX = ITEM_END - ITEM_BEGIN
    };

public:
    
    // コンストラクタ
    UiGamePadConfigWindow();
    
    // デストラクタ
    ~UiGamePadConfigWindow();

public:

    // リザルト取得
    Result GetResult() const {
        return m_Result;
    }

protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec ) TB_OVERRIDE;

    // 生成時
    virtual void OnCreated() TB_OVERRIDE;

    // 閉じた
    virtual void OnClosed() TB_OVERRIDE;

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
            TYPE_TEXT , 
            TYPE_BUTTON ,
        };
        Item()
            : type(TYPE_TEXT)
            , input(-1)
            , backup(0)
        {}
        Type        type;
        TbUiTextBox   caption;
        TbUiTextBox   text;
        UiButton     button;
        TbSint32    input;
        TbUint32    backup;
    };

private:

    void fit();

private:

    TbUiTextBox         m_Title;
    TbUiLine            m_Line[2];
    Item                m_Item[ITEM_MAX];
    TbUint32            m_FocusIndex;
    Result              m_Result;

};

#endif
