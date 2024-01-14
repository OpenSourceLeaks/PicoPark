/*!
 * UI入力
 * @author teco
 */

#ifndef _INCLUDED_UI_INPUT_H_
#define _INCLUDED_UI_INPUT_H_

#include <ui/tb_ui_input.h>

class UiInput : public toybox::TbUiInput
{
public:

    // 誰かが押した
    virtual TbBool IsPress( Button button , TbUint32 playerBit ) const;

    // 押した
    virtual TbBool IsPress(Button button, TbUint32 playerBit, TB_OUT TbUint32* inputPlayer) const;

    // 誰かが押した
    virtual TbBool IsOnRepeat( Button button , TbUint32 playerBit ) const;

    // 誰かが離した
    virtual TbBool IsOn( Button button , TbUint32 playerBit ) const;

    // カーソル数
    virtual TbUint32 GetCursorCount() const;

    // カーソル座標
    virtual TbBool GetCursorPos( TbVector2* dst , TbUint32 index ) const;

    // カーソル押した
    virtual TbBool IsCursorPress( TbUint32 index ) const;

    // カーソル押している
    virtual TbBool IsCursorOn( TbUint32 index ) const;

    // カーソルオーバーしている
    virtual TbBool IsCursorOver( TbUint32 index , const TbRectF32& rect , TbBool isEnableCursorRadius ) const;

    // カーソル動いている
    virtual TbBool IsCursorMoving( TbUint32 index) const;

};

#endif
