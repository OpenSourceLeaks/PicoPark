#include "fwd.h"
#include "root.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/window/tb_window.h>
#include <graphics/window/tb_window_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <sound/tb_sound_manager.h>
#include <base/system/tb_console.h>

namespace
{

Root* s_Root;

// 毎フレーム更新
void update()
{
    if( s_Root ){
        s_Root->Update();
    }
}

// 描画
void draw()
{
    if( s_Root ){
        s_Root->Draw();
    }
}

}

namespace toybox
{

/*!
 * メイン関数
 * @author teco
 * @since 2011.05.13
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{    
    // 初期化
    {
        TbFrameworkSimple::InitParam initParam;
        TbFrameworkSimple::GetDefaultInitParam(initParam);
        initParam.windowWidth = CMN_WINDOW_WIDTH; // 1280;
        initParam.windowHeight = CMN_WINDOW_HEIGHT; // 800;
        initParam.windowTitle = "PICO PARK";
        initParam.projectName = "pico_park";
#if CMN_ENABLE_BUILD_ALPHA
        initParam.rootPath = "resource_alpha";
#else
        initParam.rootPath = "resource";
#endif
        initParam.savePath = "save";
        initParam.taskLineMax = CMN_TASK_LINE_MAX;
        initParam.bitArray.SetBit(initParam.FLAG_USE_PAD);
        initParam.bitArray.SetBit(initParam.FLAG_USE_KEYBOARD);
        initParam.bitArray.SetBit(initParam.FLAG_USE_MOUSE);
#if TB_IS_DEBUG || CMN_ENABLE_BUILD_ALPHA
        initParam.bitArray.SetBit(initParam.FLAG_USE_LOG);
#endif
//        initParam.pa
//        initParam.bitArray.SetBit(initParam.FLAG_FULLSCREEN);
        TbFrameworkSimple::Initialize(initParam);

//        TbRenderDevice::GetInstance().SetPresentType( TB_RENDER_PRESENT_BUFFER );
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        device.SetPresentType( TB_RENDER_PRESENT_WINDOW_KEEP_ASPECT );
        TbRenderStatePreset& renderStatePreset = TbRenderStatePreset::GetInstance();
        device.SetRasterizerState(renderStatePreset.GetRasterizerState(TbRenderStatePreset::RASTERIZER_CULL_NONE) );
#if !TB_PLATFORM_IS_NX
        device.SetBlendState(renderStatePreset.GetBlendState(TbRenderStatePreset::BLEND_NORMAL));
#endif

#if 0
        // フルスクリーンの場合レンダーターゲットがディスプレイ依存になる
        // ビューポートで描画範囲を決定する
        if( initParam.bitArray.Test(initParam.FLAG_FULLSCREEN) ){
            TbRenderTarget* rt = device.GetView(0)->GetRenderTarget(0);
            TbPrintf("RT=%d,%d",rt->GetWidth(),rt->GetHeight());
            TbUint32 deltaW = (rt->GetWidth() - initParam.windowWidth);
            TbUint32 deltaH = (rt->GetHeight() - initParam.windowHeight);
            TbUint32 halfW = initParam.windowWidth/2;
            TbUint32 halfH = initParam.windowHeight/2;
            TbUint32 halfScale = TbMin(deltaW / halfW,deltaH / halfH);
            TbFloat32 scale = 1.0f + static_cast<TbFloat32>(halfScale) * 0.5f;
            TbUint32 width = static_cast<TbUint32>(static_cast<TbFloat32>(initParam.windowWidth)*scale);
            TbUint32 height = static_cast<TbUint32>(static_cast<TbFloat32>(initParam.windowHeight)*scale);
            deltaW = (rt->GetWidth() - width);
            deltaH = (rt->GetHeight() - height);
            device.SetViewport( deltaW/2 , deltaH/2 , width , height );
        }
#endif
    }

//    TbCreateConsole();
    CrInput::Initialize();
    s_Root = new Root();

    TbFrameworkSimple::SetClearColor(TbColor::White());

    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , update ,  draw , NULL );
}

void TbFramework::OnExitPoint()
{
    TB_SAFE_DELETE(s_Root);
    CrInput::Finalize();
    //    TbDeleteConsole();
    TbFrameworkSimple::Finalize();
}

/*!
 * 画面の縦、横サイズが変わった
 * @author teco
 * @since 2012.07.14
 */
void TbFramework::OnResizeDisplay( TbSint32 w , TbSint32 h )
{
    // 特に何もしない
}

}
