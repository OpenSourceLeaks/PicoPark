/*!
 * プレイヤー選択ウィンドウ
 * @author teco
 */

#ifndef _INCLUDED_UI_PLAYER_SELECT_WINDOW_H_
#define _INCLUDED_UI_PLAYER_SELECT_WINDOW_H_

#include "ui/ui_player_window.h"
#include "ui/ui_select_box.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_line.h>
#include <ui/tb_ui_image_box.h>

class UiPlayerSelectWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiPlayerSelectWindow Self;

public:

    enum Result
    {
        RESULT_NONE   ,
        RESULT_OK     , 
        RESULT_CANCEL
    };
    typedef TbStaticFunction32<void (Result)> DecidedCallback;

public:
    
    // コンストラクタ
    UiPlayerSelectWindow();
    
    // デストラクタ
    ~UiPlayerSelectWindow();

public:

    // リザルト取得
    Result GetResult() const {
        return m_Result;
    }

    // プレイヤー数取得
    TbUint32 GetPlayerCount() const {
        return m_PlayerSelectBox.GetSelectedIndex()+CMN_PLAYER_MIN;
    }

    // 選択文字列取得
    const char* GetSelectedString() const {
        return m_PlayerSelectBox.GetSelectedString();
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

    // フォーカス情報更新
    void updateFocusInfo();

private:

    UiSelectBox         m_PlayerSelectBox;
    UiButton            m_Button[BUTTON_MAX];
    TbUint32            m_FocusButtonIndex;
    MainFocus           m_MainFocus;
    Result              m_Result;
    DecidedCallback     m_DecidedCallback;

};

#endif
