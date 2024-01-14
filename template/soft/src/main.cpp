#include "fwd.h"
#include "root.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <sound/tb_sound_manager.h>
#include <sound/tb_sound_source.h>
#include <base/time/tb_tick.h>

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
        initParam.windowTitle = "TEMPLATE";
        initParam.projectName = "template";
        initParam.taskLineMax = CMN_TASK_LINE_MAX;
        initParam.bitArray.SetBit(initParam.FLAG_USE_PAD);
        initParam.bitArray.SetBit(initParam.FLAG_USE_KEYBOARD);
        initParam.bitArray.SetBit(initParam.FLAG_USE_MOUSE);
        initParam.bitArray.SetBit(initParam.FLAG_USE_CONSOLE);
        TbFrameworkSimple::Initialize(initParam);
        TbFrameworkSimple::SetClearColor(TbClearColor::Black());

        TbRenderDevice& device = TbRenderDevice::GetInstance();
        device.SetPresentType( TB_RENDER_PRESENT_WINDOW_KEEP_ASPECT );
        TbRenderStatePreset& renderStatePreset = TbRenderStatePreset::GetInstance();
        device.SetRasterizerState(renderStatePreset.GetRasterizerState(TbRenderStatePreset::RASTERIZER_CULL_NONE) );
    }

    CrInput::Initialize();
    s_Root = new Root();

    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , update ,  draw , NULL );
}

void TbFramework::OnExitPoint()
{
    TB_SAFE_DELETE(s_Root);
    CrInput::Finalize();
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
