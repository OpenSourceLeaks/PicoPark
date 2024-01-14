#include "fwd.h"
#include <base/time/tb_tick.h>
#include <base/math/tb_matrix.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/window/tb_window.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <framework/tb_framework.h>

namespace
{

using namespace toybox;

static TbDimensionF32 s_WindowSize;

TbFloat32 s_Time = 0.0f;

/*!
 * フルスクリーン
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void draw()
{
    TbRenderPass* pass = TbGetRenderPass(0);
    pass->Clear( TB_RENDER_BUFFER_TYPE_COLOR_BIT , TB_COLOR_A8R8G8B8( 0xFF , 0xFF , 0xFF , 0xFF ) , 1.0f , 0 );
    
    pass->SetFixedShaderEnableLighting( TB_FALSE );   // ライトオフ
    TbMatrix orthoMatrix;
    TbMatrixSetOrthoOffCenterLH(orthoMatrix,
                                0.0f,
                                s_WindowSize.width,
                                -s_WindowSize.height,
                                0.0f,0.1f,10.0f);
    pass->SetWorldViewProjectionMatrix(TbMatrix::IDENTITY,TbMatrix::IDENTITY,orthoMatrix);

    TbDraw2dSetRenderPass(0);
    TbDraw2dBegin();
    TbDrawRectParam param;
    param.SetRect(s_Time, s_Time, 320.0f , 240.0f );
    param.SetColor(TbColor::Cyan());
    TbDrawRect( param );
    TbDraw2dEnd();
    s_Time += 0.3f;
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
    TbFramework::Initialize();

    // ウィンドウ生成
    TbWindow* window = NULL;
    {
        TbWindowParameter windowParam;
        windowParam.rect.width = 640;
        windowParam.rect.height = 480;
        windowParam.isVisible = TB_TRUE;
        window = TbWindowManager::GetInstance().CreateWindowWithParam( windowParam );
    }
 
    // デバイス作成
    TbRenderViewParameter param;
    TbMemClear( &param , sizeof( param ) );
    param.backBufferHeight = 0;
    param.backBufferWidth = 0;
    param.backBufferFormat = TB_COLOR_FORMAT_UNKNOWN;
    param.backBufferNum = 0;
    param.enableCreateDepthStencil = TB_FALSE;
    param.autoDepthStencilsFormat = TB_COLOR_FORMAT_D24S8;
    param.isFullscreen = TB_TRUE;
    param.multiSampleType = TB_MULTISAMPLE_NONE;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.Initialize( 0 , window , 0 , param );

    // デフォルト設定
    {
        s_WindowSize.width = static_cast<TbFloat32>(window->GetWidth());
        s_WindowSize.height = static_cast<TbFloat32>(window->GetHeight());

        device.GetFixedShader().SetEnableLighting( TB_FALSE );   // ライトオフ
        TbMatrix orthoMatrix;
        TbMatrixSetOrthoOffCenterLH(orthoMatrix,
                                    0.0f,
                                    s_WindowSize.width,
                                    -s_WindowSize.height,
                                    0.0f,0.1f,10.0f);
        device.SetWorldViewProjectionMatrix(TbMatrix::IDENTITY,TbMatrix::IDENTITY,orthoMatrix);
    }

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
    TbWindowManager::GetInstance().DeleteWindowAll();
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
