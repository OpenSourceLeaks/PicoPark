#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/window/tb_window.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/time/tb_tick.h>
#include <base/math/tb_matrix.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework.h>

namespace
{

using namespace toybox;

TbResourceHandleTexture  s_ResourceTexture;
const TbSint32      WINDOW_WIDTH = 640;
const TbSint32      WINDOW_HEIGHT = 480;

/*!
 * メインループ
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void draw()
{
    TbRenderPass* renderPass = TbGetRenderPass(0);
    TbRenderDevice& device = TbRenderDevice::GetInstance();

    TbUint32 viewCount = device.GetViewCount();
    for( TbUint32 i = 0; i < viewCount; ++i )
    {        
        renderPass->SetRenderTarget( 0 , device.GetView(i)->GetRenderTarget(0) );
        renderPass->Clear( TB_RENDER_BUFFER_TYPE_COLOR_BIT , TB_COLOR_A8R8G8B8( 0xFF , 0xFF , 0xFF , 0xFF ) , 1.0f , 0 ); // 背景色

        // テクスチャ描画
        TbDraw2dSetRenderPass(0);

        {
            TbDraw2dTextureBegin();
            TbDrawRectTextureParam drawParam(s_ResourceTexture.GetTexture());
            drawParam.SetRect(0.0f , 0.0f , 320.0f , 240.0f);
            TbDrawRectTexture( drawParam );
            TbDraw2dTextureEnd();
        }

        {
            renderPass->SetFixedShaderEnableLighting(TB_FALSE);
            TbDraw2dBegin();
            TbDrawRectParam drawParam;
            drawParam.SetRect(320.0f , 240.0f , 320.0f , 240.0f);
            drawParam.SetColor(TbColor::Red());
            TbDrawRect(drawParam);
            TbDraw2dEnd();
        }
    }
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
    {
        TbFrameworkInitParam initParam;
        TbFramework::GetDefaultInitParam(initParam);
        initParam.renderViewMax = 2; // 1つのデバイスで複数ウィンドウに描画する場合はここも要設定
        TbFramework::Initialize( initParam );
    }

    // ルートパス設定
    TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_SYSTEM , "resource/system" );
    TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_ROOT , "resource" );
    TbResourceManager::GetInstance().SetFilePath( 0 , TB_FILE_PATH_ID_PRESET_ROOT );

    // ウィンドウ生成
    TbWindow* window = NULL;
    {
        TbWindowParameter windowParam;
        windowParam.rect.width = WINDOW_WIDTH;
        windowParam.rect.height = WINDOW_HEIGHT;
        windowParam.name = "window1";
        windowParam.isVisible = TB_TRUE;
        window = TbWindowManager::GetInstance().CreateWindowWithParam( windowParam );
    }

    TbWindow* window2 = NULL;
    {
        TbWindowParameter windowParam;
        windowParam.rect.width = WINDOW_WIDTH;
        windowParam.rect.height = WINDOW_HEIGHT;
        windowParam.name = "window2";
        windowParam.isVisible = TB_TRUE;
        window2 = TbWindowManager::GetInstance().CreateWindowWithParam( windowParam );
    }
 
    // デバイス作成
    TbRenderViewParameter param;
    TbMemClear( &param , sizeof( param ) );
    param.backBufferHeight = 0;
    param.backBufferWidth = 0;
    param.backBufferFormat = TB_COLOR_FORMAT_UNKNOWN;
    param.backBufferNum = 0;
    param.enableCreateDepthStencil = TB_FALSE;
    param.autoDepthStencilsFormat = TB_COLOR_FORMAT_UNKNOWN;
    param.isFullscreen = TB_FALSE;
    param.multiSampleType = TB_MULTISAMPLE_NONE;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.Initialize( 0 , window , 0 , param );

    // ビュー作成(二つ目のウィンドウ用)
    param.window = window2;
    TbRenderView* view = device.CreateView( param );

    // デフォルト設定
    {
        device.SetCullMode( TB_CULL_MODE_NONE );                  // カリングオフ
        device.GetFixedShader().SetEnableLighting( TB_FALSE );   // ライトオフ
        TbMatrix orthoMatrix;
        TbMatrixSetOrthoOffCenterLH(orthoMatrix,
                                    0.0f,
                                    static_cast<TbFloat32>(WINDOW_WIDTH),
                                    static_cast<TbFloat32>(-WINDOW_HEIGHT),
                                    0.0f,0.1f,10.0f);
        device.SetWorldViewProjectionMatrix(TbMatrix::IDENTITY,TbMatrix::IDENTITY,orthoMatrix);
    }

    // テクスチャ生成
    s_ResourceTexture.Create( "tex/sample.tga" , 0 , TB_FILE_SYNC );
    s_ResourceTexture.Activate();

    // メインループ
    TbFramework::StartMainLoop( NULL , NULL , draw , NULL );
}

/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    // 後始末
    TbFramework::Finalize();
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
