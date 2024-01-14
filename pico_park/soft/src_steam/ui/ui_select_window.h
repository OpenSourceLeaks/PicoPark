/*!
 * 選択ウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_SELECT_WINDOW_H_
#define _INCLUDED_UI_SELECT_WINDOW_H_

#include "ui/ui_player_window.h"
#include "ui/ui_select_box.h"
#include <ui/tb_ui_button.h>
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_image_box.h>

class UiSelectWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiSelectWindow Self;

public:

    enum Result
    {
        RESULT_NONE ,
        RESULT_OK , 
        RESULT_CANCEL
    };

    typedef TbStaticFunction32<void (Result result,TbSint32 index)> DecidedCallback;

public:
    
    // コンストラクタ
    UiSelectWindow();
    
    // デストラクタ
    ~UiSelectWindow();
    
public:
    
    // メッセージ設定
    void SetMessage( const char* message );

    // 選択アイテム設定
    void AddSelectItem( const char* name );
    
    // 決定コールバック取得
    void SetDecidedCallback( const DecidedCallback& cb ) {
        m_DecidedCallback = cb;
    }

    // 結果取得
    Result GetResult() const {
        return m_Result;
    }

    // 選択したインデックス値
    TbSint32 GetSelectedIndex() const {
        return m_SelectBox.GetSelectedIndex();
    }

    // 選択文字列取得
    const char* GetSelectedString() const {
        return m_SelectBox.GetSelectedString();
    }

    // 選択
    void SetSelectedIndex( TbSint32 index ){
        m_SelectBox.SetSelectedIndex( index );
    }
    
protected:
    
    // 生成時
    virtual void OnCreated();

    // キャンセル時
    virtual void OnCancel() {
        onDecidedCallback(RESULT_CANCEL,-1);
    }

    // 閉じられた時に呼ばれる
    virtual void OnClosed() {
        if( RESULT_NONE == m_Result ){
            m_Result = RESULT_CANCEL;
            if( m_DecidedCallback.IsValid() ){
                m_DecidedCallback(m_Result,-1);
            }
        }
    }

private:

    // 決定時コールバック
    void onDecidedCallback( Result result , TbSint32 index ) {
        m_Result = result;
        if( m_DecidedCallback.IsValid() ){
            m_DecidedCallback(result,index);
        }
        Close();
    }

    // 自動リサイズ
    void fit();

private:

    UiSelectBox         m_SelectBox;
    TbUiTextBox         m_TextBox;
    Result              m_Result;
    DecidedCallback     m_DecidedCallback;

};

#endif
