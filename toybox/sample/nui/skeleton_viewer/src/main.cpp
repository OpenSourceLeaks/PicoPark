// main.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <base/io/tb_file_manager.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/window/tb_window.h>
#include <base/time/tb_tick.h>
#include <input/tb_keyboard.h>
#include <input/tb_mouse.h>
#include <input/tb_pad.h>
#include <nui/tb_nui.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <graphics/font/tb_font.h>
#include <graphics/resource/tb_resource_texture.h>
#include <base/io/resource/tb_resource_manager.h>

#include "render_filter_nui_outline.h"

namespace
{

using namespace toybox;

TbResourceHandleTexture s_FontTexture;
TbFont*         s_Font = NULL;           // フォント
TbFontBuffer2d* s_FontBuffer = NULL;
TbNuiSimpleManager* s_SkeletonViewer = NULL;
RenderFilterNuiOutline* s_OutlineFilter;
TbFloat64 s_Begin = 0.0;
TbFloat64 s_End = 0.0;
TbFloat64 s_Interval = 0.0;
TbBool    s_DrawColor = TB_FALSE;

// シェーダ用ルートパス
#if TB_SHADER_API_IS_GLSL
    #define PATH_ROOT_SHADER "shader/glsl"
#elif TB_SHADER_API_IS_HLSL
    #define PATH_ROOT_SHADER "shader/hlsl"
#endif

#if TB_NUIAPI_IS_KINECT
static const TbUint32 WINDOW_WIDTH = 640;
static const TbUint32 WINDOW_HEIGHT = 480;
#else
static const TbUint32 WINDOW_WIDTH = 960;
static const TbUint32 WINDOW_HEIGHT = 540;
#endif

void update()
{
    s_Begin = TbTick::GetMilliSeconds();
    s_SkeletonViewer->Update();
}

void draw()
{
    // テクスチャ描画
    TbDraw2dTextureBegin();
    if( s_DrawColor )
    {
        TbTexture* tex = s_SkeletonViewer->GetColorTexture();
        if( tex )
        {
            if( !s_OutlineFilter ) {
                s_OutlineFilter = new RenderFilterNuiOutline(s_SkeletonViewer->GetColorUV());
            }
#if 1
            if( s_SkeletonViewer->IsEnableColorPlayerMask() ){
                s_OutlineFilter->Render(tex,s_SkeletonViewer->GetColorUV());
            }else
#endif
            {
                TbDrawRectTextureParam drawParam(tex);
                drawParam.SetRect(0.0f , 0.0f , WINDOW_WIDTH , WINDOW_HEIGHT);
                drawParam.SetRectUV(s_SkeletonViewer->GetColorUV());
                TbDrawRectTexture( drawParam );
            }
        }
    }
    else
    {
        TbTexture* tex = s_SkeletonViewer->GetDepthTexture();
        if( tex )
        {
            TbDrawRectTextureParam drawParam(tex);
            drawParam.SetRect(0.0f , 0.0f , 512.0f , 424.0f);
            drawParam.SetRectUV(s_SkeletonViewer->GetDepthUV());
            TbDrawRectTexture( drawParam );
        }
    }
    TbDraw2dTextureEnd();

    if( TbKeyboard::IsPress(TB_KEY_A) ){
        s_SkeletonViewer->SetEnableDepthPlayerMask( !s_SkeletonViewer->IsEnableDepthPlayerMask() );
    }
    if( TbKeyboard::IsPress(TB_KEY_B) ){
        s_SkeletonViewer->SetEnableColorPlayerMask( !s_SkeletonViewer->IsEnableColorPlayerMask() );
    }
    if( TbKeyboard::IsPress(TB_KEY_C) ){
        s_DrawColor = !s_DrawColor;
    }
    if( TbKeyboard::IsPress(TB_KEY_SPACE) ){
        s_SkeletonViewer->ExportColorImage( TbNuiSimpleManager::EXPORT_FILE_FORMAT_TGA );
    }

    if( s_SkeletonViewer->GetPlayerSkeletonIndex(0) >= 0 ){
        s_FontBuffer->Printf( 10 , 400 , "Skeleton Tracked : %d" , s_SkeletonViewer->GetPlayerSkeletonIndex(0) );
    }
    s_FontBuffer->Printf( 10 , 450 , "msec=%0.4f" , static_cast<TbFloat32>(s_Interval) );
    s_FontBuffer->Flush();


    // スケルトン描画
    {
        TbRenderPass* pass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_2D);
        TbUint32 stride = 0;
        TbVertexBuffer* vertexBuffer = s_SkeletonViewer->GetSkeletonVertexBuffer(0 , &stride );
        pass->SetTexture( 0 , NULL );
        pass->SetWorldViewProjectionMatrix( TbMatrix::IDENTITY , TbMatrix::IDENTITY , TbMatrix::IDENTITY );
        pass->SetFixedShaderVertexFormat( s_SkeletonViewer->GetSkeletonVertexFormat() );
        pass->SetVertexBuffer( 0 , vertexBuffer , 0 , stride );
        pass->DrawPrimitive( TB_PT_LINELIST , 0 , 18 );
    }

    s_End = TbTick::GetMilliSeconds();
    s_Interval = s_End - s_Begin;
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
    TbFrameworkSimple::InitParam initParam;
    TbFrameworkSimple::GetDefaultInitParam(initParam);
    initParam.windowWidth  = WINDOW_WIDTH;
    initParam.windowHeight = WINDOW_HEIGHT;
    TbFrameworkSimple::Initialize(initParam);

    // ルートパス設定
    TbResourceManager::GetInstance().SetFilePath(0,TB_FILE_PATH_ID_PRESET_ROOT);
    TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_SHADER , PATH_ROOT_SHADER );

    TbDraw2dSetRenderPass( TbFrameworkSimple::RENDER_PASS_2D );

    {
        TbRenderPass* pass = TbGetRenderPass( TbFrameworkSimple::RENDER_PASS_ONSCREEN_BEGIN );
        TbFrameworkSimple::SetClearColor( TB_COLOR_A8R8G8B8( 0xFF , 0xFF , 0 , 0 ) );
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        device.SetCullMode( TB_CULL_MODE_NONE );
    }

    TbFileManager::PathList list;
    TbFileManager::GetInstance().GetDirectories(list,"..",0,TB_FILE_PATH_ID_PRESET_ROOT);
     
    // フォント設定(ASCII)
    {
        s_FontTexture.Create("system/font/font_ascii.tga",0,TB_FILE_SYNC);
        s_Font = new TbFontAscii();
        s_Font->SetTexture(s_FontTexture.GetTexture());
        s_FontBuffer = new TbFontBuffer2d(1000,TbFrameworkSimple::RENDER_PASS_2D);
        s_FontBuffer->SetFont(s_Font);
        s_FontBuffer->SetFontSize(16);
        s_FontBuffer->SetColor(TbColor::Cyan());
    }
    // スケルトンビューワ
    s_SkeletonViewer = new TbNuiSimpleManager(1);

    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , update , draw , NULL );
}
    
/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    // 後始末
    TB_SAFE_DELETE( s_Font );
    TB_SAFE_DELETE( s_SkeletonViewer );
    TbWindowManager::GetInstance().DeleteWindowAll();
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
