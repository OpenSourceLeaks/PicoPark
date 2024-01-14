/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_MESSAGE_WINDOW_H_
#define _INCLUDED_UI_MESSAGE_WINDOW_H_

#include "ui/ui_player_window.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_image_box.h>

class UiMessageWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiMessageWindow Self;

public:

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_OK ,
        TYPE_YESNO ,
    };

    enum Result
    {
        RESULT_NONE ,
        RESULT_YES , 
        RESULT_NO , 
        RESULT_CANCEL
    };

    typedef TbStaticFunction32<void (Result)> DecidedCallback;

public:
    
    // コンストラクタ
    UiMessageWindow( Type type );
    
    // デストラクタ
    virtual ~UiMessageWindow();
    
public:

    // メッセージ設定
    void SetMessage( const char* message );
    
    // 決定コールバック取得
    void SetDecidedCallback( const DecidedCallback& cb ) {
        m_DecidedCallback = cb;
    }

    // 結果取得
    Result GetResult() const {
        return m_Result;
    }

    // テキストのアライン設定
    void SetTextAlignX( TbUiAlignX align ) {
        m_TextBox.SetTextAlignX(align);
    }

    // テキストのアライン設定
    void SetTextAlignY( TbUiAlignY align ) {
        m_TextBox.SetTextAlignY(align);
    }

    // フィット可能か
    void SetEnableFit( TbBool isEnable ) {
        m_IsEnableFit = isEnable;
        m_TextBox.SetEnableResize(isEnable);
    }

    // 指定リザルトにフォーカス
    void SetFocusResult(Result result);

    // 最後に操作したプレイヤー取得
    TbSint32 GetInputPlayer() const;
    
protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec );

    // 生成時
    virtual void OnCreated();

    // キャンセル
    virtual void OnCancel() {
        onDecidedCallback(RESULT_CANCEL);
    }

    // 閉じられた時に呼ばれる
    virtual void OnClosed() {
        if( RESULT_NONE == m_Result ){
            m_Result = RESULT_CANCEL;
            if( m_DecidedCallback.IsValid() ){
                m_DecidedCallback(m_Result);
            }
        }
    }

private:

    static const TbSizeT BUTTON_MAX = 2;
    static const TbSint32 BUTTON_INDEX_OK = 0;
    static const TbSint32 BUTTON_INDEX_YES = 0;
    static const TbSint32 BUTTON_INDEX_NO = 0;

private:

    // 決定時コールバック
    void onDecidedCallback( Result result ) {
        m_Result = result;
        if( m_DecidedCallback.IsValid() ){
            m_DecidedCallback(result);
        }
        Close();
    }

    // 自動リサイズ
    void fit();

private:

    Type                m_Type;
    TbUiTextBox         m_TextBox;
    UiButton            m_Button[BUTTON_MAX];
    Result              m_Result;
    DecidedCallback     m_DecidedCallback;
    TbBool              m_IsEnableFit;
    TbUint32            m_InitFocusIndex;

};

#endif
