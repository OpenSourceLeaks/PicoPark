/*!
 * ポーズウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_PAUSE_WINDOW_H_
#define _INCLUDED_UI_PAUSE_WINDOW_H_

#include "ui/ui_player_window.h"

#include "ui/ui_button.h"
#include <ui/tb_ui_text_box.h>
#include <ui/tb_ui_image_box.h>

class UiPauseWindow : public UiPlayerWindow
{
public:

    typedef UiPlayerWindow Super;
    typedef UiPauseWindow Self;

public:

    enum Menu
    {
        MENU_INVALID ,
        MENU_BEGIN , 
        MENU_RETURN_GAME = MENU_BEGIN , // 復帰
        MENU_RETRY,                // リトライ
        MENU_RETURN_STAGE_SELECT , // ステージ選択画面に戻る
        MENU_RETURN_MAIN_MENU ,
        MENU_RETURN_TITLE , // タイトル画面に戻る
        MENU_JOIN         , // JOIN
        MENU_END ,
        MENU_MAX = MENU_END - MENU_BEGIN
    };

    typedef TbStaticFunction32<void (Menu)> DecidedCallback;

public:
    
    // コンストラクタ
    UiPauseWindow();
    
    // デストラクタ
    ~UiPauseWindow();
    
public:
    
    // 決定コールバック取得
    void SetDecidedCallback( const DecidedCallback& cb ) {
        m_DecidedCallback = cb;
    }

    // 結果取得
    Menu GetResult() const {
        return m_Result;
    }

    // バトルモード設定
    void SetBattleMode();
    
protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec );

    // 生成開始
    virtual void OnCreating();

    // 生成時
    virtual void OnCreated();

    // キャンセル
    virtual void OnCancel() {
        onDecidedCallback(MENU_RETURN_GAME);
    }

    // 閉じられた時に呼ばれる
    virtual void OnClosed() {
        if( MENU_INVALID == m_Result ){
            m_Result = MENU_RETURN_GAME;
            if( m_DecidedCallback.IsValid() ){
                m_DecidedCallback(m_Result);
            }
        }
    }

private:

    // 決定時コールバック
    void onDecidedCallback( Menu index ) {
        m_Result = index;
        if( m_DecidedCallback.IsValid() ){
            m_DecidedCallback(m_Result);
        }
        Close();
    }

private:

    void fit();

private:

    UiButton            m_Button[MENU_MAX];
    TbUint32            m_MenuMap[MENU_MAX];
    TbUint32            m_ActiveMenuCount;
    TbUint32            m_FocusIndex;
    Menu                m_Result;
    DecidedCallback     m_DecidedCallback;

};

#endif
