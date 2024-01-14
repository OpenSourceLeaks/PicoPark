/*!
 * UI入力系
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_INPUT_H_
#define _INCLUDED_TB_UI_INPUT_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbUiInput
{
public:

    enum Button
    {
        BUTTON_DECIDE ,
        BUTTON_UP     ,
        BUTTON_DOWN   ,
        BUTTON_LEFT   ,
        BUTTON_RIGHT  ,
        BUTTON_MAX
    };

public:
    
    // コンストラクタ
    TbUiInput() {}
    
    // デストラクタ
    virtual ~TbUiInput() {}

public:

    // 押した
    virtual TbBool IsPress( Button button , TbUint32 playerBit ) const = 0;

    // 押した
    virtual TbBool IsPress(Button button, TbUint32 playerBit , TB_OUT TbUint32* inputPlayer ) const = 0;

    // 押した
    virtual TbBool IsOnRepeat( Button button , TbUint32 playerBit ) const = 0;

    // 押している
    virtual TbBool IsOn( Button button , TbUint32 playerBit ) const = 0;

    // カーソル数
    virtual TbUint32 GetCursorCount() const = 0;

    // カーソル座標
    virtual TbBool GetCursorPos( TbVector2* dst , TbUint32 index ) const = 0;

    // カーソル押した
    virtual TbBool IsCursorPress( TbUint32 index ) const = 0;

    // カーソル押している
    virtual TbBool IsCursorOn( TbUint32 index ) const = 0;

    // カーソルオーバーしている
    virtual TbBool IsCursorOver( TbUint32 index , const TbRectF32& rect , TbBool isEnableCursorRadius) const = 0;

    // カーソル動いている
    virtual TbBool IsCursorMoving( TbUint32 index) const = 0;

    // カーソルオーバーしている
    TbBool IsAnyoneCursorOver( const TbRectF32& rect , TbBool isEnableCursorRadius , TbUint32* dstIndex = nullptr ) const {
        TbUint32 count = GetCursorCount();
        for( TbUint32 i = 0; i < count; ++i ) {
            if( IsCursorOver(i,rect,isEnableCursorRadius) )
            {
                if( dstIndex ){
                    *dstIndex = i;
                }
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

    // カーソル押した
    TbBool IsAnyoneCursorPress( const TbRectF32& rect , TbBool isEnableCursorRadius , TbUint32* dstIndex = nullptr ) {
        TbUint32 count = GetCursorCount();
        for( TbUint32 i = 0; i < count; ++i ) {
            if( IsCursorOver(i,rect,isEnableCursorRadius) && IsCursorPress(i) )
            {
                if( dstIndex ){
                    *dstIndex = i;
                }
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }
};

}

#endif
