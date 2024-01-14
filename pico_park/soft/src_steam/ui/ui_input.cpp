/*!
 * UI管理クラス
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_manager.h"
#include "root.h"
#include "system/sys_input.h"

#include <input/tb_mouse.h>
#include <framework/sub/tb_framework_simple.h>
#include <graphics/render/3dapi/tb_render_device.h>

static const SysInput::Button BUTTON_TABLE[] =
{
    SysInput::BUTTON_DECIDE , 
    SysInput::BUTTON_UP , 
    SysInput::BUTTON_DOWN , 
    SysInput::BUTTON_LEFT , 
    SysInput::BUTTON_RIGHT , 
};

/*!
 * 押した
 * @author teco
 */
TbBool UiInput::IsPress( Button button , TbUint32 playerBit ) const
{
    return SysInput::IsAnyonePress( BUTTON_TABLE[button] , playerBit );
}

/*!
 * 押した
 * @author teco
 */
TbBool UiInput::IsOnRepeat( Button button , TbUint32 playerBit ) const
{
    return SysInput::IsAnyoneOnRepeat( BUTTON_TABLE[button] , playerBit );
}

/*!
 * 押している
 * @author teco
 */
TbBool UiInput::IsOn( Button button , TbUint32 playerBit ) const
{
    return SysInput::IsAnyoneOn( BUTTON_TABLE[button] , playerBit );
}


/*!
 * カーソル数
 * @author teco
 */
TbUint32 UiInput::GetCursorCount() const
{
    return 1;
}

/*!
 * カーソル座標
 * @author teco
 */
TbBool UiInput::GetCursorPos( TbVector2* dst , TbUint32 index ) const
{
    if( 0 == index )
    {
        TbVector2 pos;
        TbMouse::GetPos(&pos,TB_MOUSE_POS_TYPE_WINDOW);

        // 表示領域にマッピング
        TbRectS32 rect = TbRenderDevice::GetInstance().GetView(0)->GetPresentRegion();
        TbFloat32 x = static_cast<TbFloat32>(pos.GetX()-rect.x) / static_cast<TbFloat32>(rect.width);
        TbFloat32 y = static_cast<TbFloat32>(pos.GetY()-rect.y) / static_cast<TbFloat32>(rect.height);
        x *= static_cast<TbFloat32>(CMN_WINDOW_WIDTH);
        y *= static_cast<TbFloat32>(CMN_WINDOW_HEIGHT);
        dst->Set( x , y );

        return true;
    }
    return false;
}

/*!
 * カーソル押した
 * @author teco
 */
TbBool UiInput::IsCursorPress( TbUint32 index ) const
{
    if( 0 == index )
    {
        return TbMouse::IsPress(TB_MOUSE_LEFT_BIT);
    }
    return TB_FALSE;
}

/*!
 * カーソル押している
 * @author teco
 */
TbBool UiInput::IsCursorOn( TbUint32 index ) const
{
    if( 0 == index )
    {
        return TbMouse::IsOn(TB_MOUSE_LEFT_BIT);
    }
    return TB_FALSE;
}

/*!
 * カーソルオーバーしている
 * @author teco
 */
TbBool UiInput::IsCursorOver( TbUint32 index , const TbRectF32& rect , TbBool isEnableCursorRadius ) const
{
    if( 0 == index )
    {
        TbRectF32 dstRect;
        if( !Root::GetInstance().ConvertGameRectToScreenRect(&dstRect,rect) ){
            return TB_FALSE;
        }
        return TbMouse::IsOver(dstRect,TB_MOUSE_POS_TYPE_WINDOW);
    }
    return TB_FALSE;
}

/*!
 * カーソル動いている
 * @author teco
 */
TbBool UiInput::IsCursorMoving( TbUint32 index) const
{
    if( 0 == index )
    {
        return TbMouse::IsMoving();
    }
    return TB_FALSE;
}
