// main.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <input/tb_keyboard.h>
#include <input/tb_mouse.h>
#include <input/tb_pad.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <base/time/tb_tick.h>

#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>

namespace
{

using namespace toybox;

void draw()
{
    static TbFloat64 pre;
    static TbFloat64 end;
    static TbFloat64 interval;

    pre = TbTick::GetMilliSeconds();

    TbVector2 mousePos;
    TbMouse::GetPos(&mousePos,TB_MOUSE_POS_TYPE_WINDOW);
    TbFrameworkSimple::PrintfScreen( 400 , 200 , "mouseX=%0.4f" , mousePos.GetX() );
    TbFrameworkSimple::PrintfScreen( 400 , 230 , "mouseY=%0.4f" , mousePos.GetY() );

    if( TbKeyboard::IsOn(TB_KEY_A) ){
        TbFrameworkSimple::PrintfScreen( 100 , 300 , "Keyboard A is On!!" );
    }
    if( TbMouse::IsOn(TB_MOUSE_LEFT_BIT) ){
        TbFrameworkSimple::PrintfScreen( 100 , 330 , "Mouse Left is On!!" );
    }
    if( TbPad::IsOn(TB_PAD_A_BIT) ){
        TbFrameworkSimple::PrintfScreen( 100 , 350 , "Pad A is On!!" );
    }

    end = TbTick::GetMilliSeconds();
    interval = end - pre;
}

}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    // 初期化
    TbFrameworkSimple::Initialize();
     
    // 設定
    {
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        // 半透明を有効にしておく
        device.SetEnableBlend(TB_TRUE);
        device.SetCullMode(TB_CULL_MODE_NONE);
    }
     
    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , NULL , draw , NULL );
}

/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    TbFrameworkSimple::Finalize();
}

/*!
 * 画面の縦、横サイズが変わった
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
void TbFramework::OnResizeDisplay( TbSint32 w , TbSint32 h )
{
    // 特に何もしない
}

}
