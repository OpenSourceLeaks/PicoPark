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
#include <framework/debug/viewer/tb_allocator_viewer.h>
#include <framework/debug/viewer/tb_heap_allocator_view.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <base/memory/tb_allocator_holder.h>
#include <base/time/tb_tick.h>
#include <base/container/tb_pool.h>
#include <base/util/tb_type_traits.h>

namespace
{

using namespace toybox;

TbFont*                 s_FontAscii = NULL;   // ASCIIフォント
TbFontBuffer2d*         s_FontBufferAscii = NULL;
TbResourceHandleTexture s_ResTextureAscii;

TbFont*                 s_FontSjis = NULL;    // SJISフォント
TbFontBuffer2d*         s_FontBufferSjis = NULL;
TbResourceHandleTexture s_ResTextureSjis;

TbAllocatorViewer*      s_AllocatorViewer;
TbHeapAllocatorView*    s_HeapAllocatorView;

const TbFontCharInfoSjis FONT_SJIS_CUSTOM[] =
{
    #include "font_info_custom.h"
};

/*!
 * フルスクリーン
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void draw()
{
    static TbFloat64 begin = 0.0;
    static TbFloat64 end = 0.0;
    static TbFloat64 interval = 0.0;
    begin = TbTick::GetMilliSeconds();
    // フォント書き込み
    s_FontBufferAscii->Printf( 10 , 10 , "Frame Time: %f" , static_cast<TbFloat32>(interval) );
    
    s_FontBufferSjis->Printf( 10 , 130  , "┌──────────┐" );
    s_FontBufferSjis->Printf( 10 , 194  , "│　　あああ　　　　　┃" );
    s_FontBufferSjis->Printf( 10 , 258 , "└━━━━━━━━━━┘" );
    s_AllocatorViewer->Draw();

    // フォント描画
    s_FontBufferSjis->Flush();
    s_FontBufferAscii->Flush();

    end = TbTick::GetMilliSeconds();
    interval = end - begin;
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
    TbFrameworkSimple::SetClearColor( TbClearColor::FromRgbaU8( 0xFF , 0xFF , 0 , 0xFF ) );
 
    // 設定
    {
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        // 半透明を有効にしておく
        device.SetEnableBlend(TB_TRUE);
        device.SetCullMode(TB_CULL_MODE_NONE);
    }

    // フォント設定(ASCII)
    {
        s_ResTextureAscii.Create("system/font/font_ascii.tga",0,TB_FILE_SYNC);
        s_ResTextureAscii.Activate();
        s_FontAscii = new TbFontAscii();
        s_FontAscii->SetTexture(s_ResTextureAscii.GetTexture());
        s_FontBufferAscii = new TbFontBuffer2d(1000,TbFrameworkSimple::RENDER_PASS_2D);
        s_FontBufferAscii->SetFont(s_FontAscii);
        s_FontBufferAscii->SetFontSize(16);
        s_FontBufferAscii->SetColor(TbColor::Cyan());
    }

    // フォント設定(SJIS)
    {
        TbFontCreateParamSjis param;
        param.common.charCount = TB_ARRAY_COUNT_OF(FONT_SJIS_CUSTOM);
        param.common.defaultFontSize = 64;
        param.common.uvBaseSize = TbDimensionF32(0.062500,0.062500);
        param.charInfoList = FONT_SJIS_CUSTOM;
        s_ResTextureSjis.Create("system/font/font_dq.tga",0,TB_FILE_SYNC);
        s_ResTextureSjis.Activate();
        s_FontSjis = new TbFontSjis(param);
        s_FontSjis->SetTexture(s_ResTextureSjis.GetTexture());
        s_FontBufferSjis= new TbFontBuffer2d(1000,TbFrameworkSimple::RENDER_PASS_2D);
        s_FontBufferSjis->SetFont(s_FontSjis);
        s_FontBufferSjis->SetFontSize(64);
        s_FontBufferSjis->SetColor(TbColor::Blue());
    }

    // アロケータ情報表示
    {
        s_AllocatorViewer = new TbAllocatorViewer();
        s_HeapAllocatorView = new TbHeapAllocatorView();

        s_HeapAllocatorView->SetAllocator(&TbAllocatorHolderGlobal::GetInstance().GetValue());
        s_HeapAllocatorView->SetName("Global");
        s_AllocatorViewer->AddView(s_HeapAllocatorView);
        s_AllocatorViewer->SetPos(30,30);
        s_AllocatorViewer->SetFontBuffer(s_FontBufferAscii);
        s_AllocatorViewer->SetVisible(TB_TRUE);
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
    TB_SAFE_DELETE(s_FontAscii);
    TB_SAFE_DELETE(s_FontBufferAscii);
    s_ResTextureAscii.Deactivate();
    TB_SAFE_DELETE(s_FontSjis);
    TB_SAFE_DELETE(s_FontBufferSjis);
    s_ResTextureSjis.Deactivate();
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
