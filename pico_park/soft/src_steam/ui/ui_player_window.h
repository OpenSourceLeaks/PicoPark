/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_PLAYER_WINDOW_H_
#define _INCLUDED_UI_PLAYER_WINDOW_H_

#include <ui/tb_ui_window.h>
#include <ui/tb_ui_image_box.h>
#include "common/material/cmn_sprite_player_material.h"

class UiPlayerWindow : public TbUiWindow
{
public:

    typedef TbUiWindow Super;
    typedef UiPlayerWindow Self;

public:
    
    // コンストラクタ
    UiPlayerWindow( TbBool isEnableImage = TB_TRUE );
    
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

public:

    // マテリアル取得
    CmnSpritePlayerMaterial* GetMaterial() {
        return &m_Material;
    }
    
    // マテリアル取得
    const CmnSpritePlayerMaterial* GetMaterial() const {
        return &m_Material;
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
        FLAG_ENABLE_IMAGE ,
        FLAG_REQUEST_CLOSE ,
    };

private:

    TbBitArray32            m_BitArray;
    TbFloat32               m_CloseTimer;
    TbUiImageBox            m_PlayerImageBox;
    CmnSpritePlayerMaterial m_Material;

};

#endif
