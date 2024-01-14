/*!
 * UIボタン
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_BUTTON_H_
#define _INCLUDED_UI_BUTTON_H_

#include <graphics/sprite/tb_elastic_sprite.h>
#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>

#include <ui/tb_ui_button.h>

class UiButtonSkin : public TbUiButtonDefaultSkin
{
public:

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus );

    // 押された状態変更
    virtual void OnPressChanged( TbBool isFocus );

};

class UiButton : public TbUiButton
{
public:

    // コンストラクタ
    UiButton()
    {
        SetSkin(&m_Skin);
        SetEnableFocusCursorOver(TB_TRUE);
    }

private:

    UiButtonSkin    m_Skin;

};

#endif
