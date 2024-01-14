/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_PLAYER_WINDOW_H_
#define _INCLUDED_UI_PLAYER_WINDOW_H_

#include <ui/tb_ui_window.h>
#include <ui/tb_ui_image_box.h>

class UiPlayerWindow : public TbUiWindow
{
public:

    typedef TbUiWindow Super;
    typedef UiPlayerWindow Self;

public:
    
    // コンストラクタ
    UiPlayerWindow();
    
    // デストラクタ
    virtual ~UiPlayerWindow();
    
public:
    
    // 閉じるを時間付きでリクエスト
    void RequestClose( TbFloat32 delayTime );

    // プレイヤー画像表示設定
    void SetVisiblePlayerImage( TbBool isVisible ) {
        m_PlayerImageBox.SetVisible( isVisible );
    }

    // プレイヤー画像表示するか
    TbBool IsVisiblePlayerImage() const {
        return m_PlayerImageBox.IsVisible();
    }
    
protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec );

    // プレイヤービットが変更された
    virtual void OnPlayerBitChanged( TbUint32 playerBit );

    // キャンセル
    virtual void OnCancel() {}

protected:

    // ウィンドウ内のY座標取得
    TbFloat32 GetLocalTopY() const;

private:

    enum Flag
    {
        FLAG_REQUEST_CLOSE ,
    };

private:

    // UV更新
    void updateUv();

private:

    TbBitArray32        m_BitArray;
    TbFloat32           m_CloseTimer;
    TbUiImageBox          m_PlayerImageBox;

};

#endif
