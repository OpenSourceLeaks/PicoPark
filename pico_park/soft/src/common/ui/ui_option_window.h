/*!
 * オプションウィンドウ
 * @author teco
 */

#ifndef _INCLUDED_UI_OPTION_WINDOW_H_
#define _INCLUDED_UI_OPTION_WINDOW_H_

#include "ui/ui_player_window.h"
#include "ui/ui_select_box.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_line.h>
#include <ui/tb_ui_image_box.h>

#define UI_OPTION_ENABLE_FULLSCREEN (TB_PLATFORM_IS_WIN)

class UiOptionWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiOptionWindow Self;

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
        ITEM_BEGIN       , 
        ITEM_CHANGE_JUMP_BUTTON = ITEM_BEGIN , // 
        ITEM_BG_COLOR    , // 
        ITEM_VOLUME      , // 
        ITEM_DISP_PLAYER_NUMBER ,
        ITEM_END         ,

        ITEM_MAX = ITEM_END - ITEM_BEGIN
    };

public:
    
    // コンストラクタ
    UiOptionWindow();
    
    // デストラクタ
    ~UiOptionWindow();

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
//        SetFocus(TB_FALSE);
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

    // チェンジジャンプボタン
    void onChangedJumpButton(TbSint32 index);

    // 背景カラー変更
    void onChangedBgColor(TbSint32 index);

    // プレイヤー番号表示変更
    void onChangedVisiblePlayerNumber( TbSint32 index );

    // フォーカス情報更新
    void updateFocusInfo();

private:

    Item                m_Item[ITEM_MAX];
    UiButton            m_Button[BUTTON_MAX];
    MainFocus           m_MainFocus;
    TbUint32            m_FocusItemIndex;
    TbUint32            m_FocusButtonIndex;
    TbUint32            m_DefautBgColor;
    TbSint32            m_DefautBgmVolume;
    TbBool              m_DefaultVisiblePlayerNumber;
    Result              m_Result;
    DecidedCallback     m_DecidedCallback;

};

#endif
