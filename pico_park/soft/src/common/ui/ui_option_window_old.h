/*!
 * オプションウィンドウ
 * @author teco
 */

#ifndef _INCLUDED_UI_OPTION_WINDOW_OLD_H_
#define _INCLUDED_UI_OPTION_WINDOW_OLD_H_

#include "ui/ui_player_window.h"
#include "ui/ui_select_box.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_line.h>
#include <ui/tb_ui_image_box.h>

#define UI_OPTION_ENABLE_FULLSCREEN (TB_PLATFORM_IS_WIN)

class UiOptionWindowOld : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiOptionWindowOld Self;

public:

    enum Result
    {
        RESULT_NONE ,
        RESULT_OK , 
        RESULT_CANCEL
    };
    typedef TbStaticFunction32<void (Result)> DecidedCallback;

    enum ItemType
    {
        ITEM_BEGIN , 
        ITEM_VOLUME = ITEM_BEGIN , // 
#if UI_OPTION_ENABLE_FULLSCREEN
        ITEM_WINDOW_MODE ,
#endif
        ITEM_END         ,

        ITEM_MAX = ITEM_END - ITEM_BEGIN
    };

public:
    
    // コンストラクタ
    UiOptionWindowOld();
    
    // デストラクタ
    ~UiOptionWindowOld();

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

    // キャンセル
    virtual void OnCancel() TB_OVERRIDE {
        m_Result = RESULT_CANCEL;
        Close();
    }

    // 閉じられた時に呼ばれる
    virtual void OnClosed() {
        if( RESULT_NONE == m_Result ){
            m_Result = RESULT_CANCEL;
        }
    }

private:

    struct Item
    {
        enum Type
        {
            TYPE_SELECT , 
        };
        Item()
            : type(TYPE_SELECT)
        {}
        Type        type;
        TbUiTextBox caption;
        UiSelectBox selectBox;
    };

    enum
    {
        BUTTON_OK ,
        BUTTON_CANCEL ,
        BUTTON_MAX ,
    };

    enum MainFocus
    {
        MAIN_FOCUS_ITEM ,
        MAIN_FOCUS_BUTTON 
    };

private:

    void fit();

    // 決定時コールバック
    void onDecidedCallback( Result result );

    // 音量変更通知
    void onChangedVolume( TbSint32 index );

    // フォーカス情報更新
    void updateFocusInfo();

private:

    TbUiTextBox         m_Title;
    Item                m_Item[ITEM_MAX];
    UiButton            m_Button[BUTTON_MAX];
    MainFocus           m_MainFocus;
    TbUint32            m_FocusItemIndex;
    TbUint32            m_FocusButtonIndex;
    Result              m_Result;
    DecidedCallback     m_DecidedCallback;

};

#endif
