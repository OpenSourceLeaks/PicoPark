/*!
 * UI管理クラス
 * @author teco
 */

#ifndef _INCLUDED_UI_MANAGER_H_
#define _INCLUDED_UI_MANAGER_H_

#include "ui/ui_input.h"
#include <ui/tb_ui_manager.h>

class UiManager
{
public:

    // コンストラクタ
    UiManager();

public:

    // 定期処理
    void Update() {
        m_Manager.Update();
    }

    // 描画処理
    void Draw() {
        m_Manager.Draw();
    }

public:

    // フォントバッファ取得
    void SetFontBuffer( toybox::TbFontBuffer2d*  font ) {
        m_Manager.SetFontBuffer(font);
    }

    // UVパラメータ取得
    void SetUv( toybox::TbUiUvType type , const toybox::TbElasticSprite::UvParam& param ){
        m_Manager.SetUV(type,param);
    }

    // UVパラメータ取得
    void SetUvRect( toybox::TbUiUvRectType type , const toybox::TbRectF32& param ){
        m_Manager.SetUVRect(type,param);
    }

    // テクスチャ設定
    void SetTexture( TbTexture* texture ) {
        m_Manager.SetTexture(texture);
    }

private:
    toybox::TbUiManager m_Manager;
    UiInput             m_Input;
};

#endif
