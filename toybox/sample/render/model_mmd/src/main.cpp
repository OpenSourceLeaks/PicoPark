#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_draw_3d.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/io/resource/tb_resource_handle.h>
#include <base/io/stream/tb_const_memory_stream.h>
#include <base/io/stream/tb_stream_binary_reader.h>
#include <base/math/tb_matrix.h>
#include <base/math/tb_vector2.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <graphics/image/tb_image_tga.h>
#include <graphics/resource/tb_resource_model_pmx.h>
#include <graphics/resource/tb_resource_motion_vmd.h>
#include <graphics/scene/tb_render_object_mmd.h>

#include <base/io/tb_path.h>

namespace
{
using namespace toybox;

TbShaderProgram*            s_ShaderProgram = NULL;
TbResourceHandleModelPmx    s_ResourceModel;
TbResourceHandleMotionVmd   s_ResourceMotion;
TbRenderObjectMmd           s_Object;

// シェーダファイルパス
#if TB_SHADER_API_IS_GLSL
const char SHADER_ROOT_PATH[] = "/shader/glsl";
const char VERTEX_PROGRAM_FILEPATH[] =  "pmx.vert";
const char PIXEL_PROGRAM_FILEPATH[] = "pmx.frag";
#elif TB_SHADER_API_IS_HLSL
const char SHADER_ROOT_PATH[] = "/shader/hlsl";
const char VERTEX_PROGRAM_FILEPATH[] = "pmx.vsh";
const char PIXEL_PROGRAM_FILEPATH[] = "pmx.psh";
#endif

void draw()
{
    TbRenderPass* renderPass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_3D);
    if( !renderPass ){
        return;
    }
    renderPass->Clear(TB_RENDER_BUFFER_TYPE_COLOR_BIT|TB_RENDER_BUFFER_TYPE_DEPTH_BIT,TbColor::FromRgbaU8(255,140,140,255),1,0);

    // 行列計算
    TbMatrix viewMtx;
    TbMatrix projMtx;
    {
        // ビュー行列設定
        TbVector4 eye( -9.0f, 0.0f,-40.0f );
        TbVector4 at( -9.0f, 10.0f, 0.0f );
        TbVector4 up( 0.0f, 1.0f, 0.0f );
        TbMatrixSetLookAtLH( viewMtx , eye , at , up );

        // 射影行列設定
        TbMatrixSetPerspectiveLH( projMtx , TbRadToAng32(45.0f*TB_PI_RAD/180.0f) , 1.2f , 0.1f ,200.0f );
        renderPass->SetViewMatrix(viewMtx);
        renderPass->SetProjectionMatrix(projMtx);
    }

    TbFrameworkSimple::PrintfScreen( 32 , 32 , "FRAME=%0.2f" , s_Object.GetFrame() );

//    s_Object.AddRotDeg(TbVector4(0.0f,2.0f,0.0f));
#if 1 // モーション進めるか
    s_Object.Step(1/60.0f);
#endif
    s_Object.Draw(TbFrameworkSimple::RENDER_PASS_3D);
    
    // デバッグ表示
#if 0
    {
        renderPass->SetEnableDepthTest(TB_FALSE);
        TbFloat32 size = 0.5f;
        const TbMatrix* mtx = s_Object.GetMatrix(31);

        TbVector4 pos;
        mtx->GetTranslation(pos);
        TbDrawBox(pos,size,size,size,TbColor::White());

        mtx = s_Object.GetMatrix(17);
        mtx->GetTranslation(pos);
        TbDrawBox(pos,size,size,size,TbColor::White());
    }
#endif
}

}

namespace toybox
{

#if 0
#define MODEL_DIR "model/murabito/"
#define MODEL_PATH MODEL_DIR "murabito.pmx"
#define TEX_EXT ".tga"
#elif 1
#define MODEL_DIR "model/miku/"
#define MODEL_PATH MODEL_DIR "miku.pmx"
#define TEX_EXT ".tga"
#else
#define MODEL_DIR "model/stage/"
#define MODEL_PATH MODEL_DIR "stage.pmx"
#define TEX_EXT ".bmp"
#endif

/*!
 * メイン関数
 * @author Miyake Shunsuke
 * @since 2011.05.13
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    // 初期化
    TbFrameworkSimple::InitParam param;
    TbFrameworkSimple::GetDefaultInitParam(param);
    param.windowWidth *= 2;
    param.windowHeight *= 2;
    TbFrameworkSimple::Initialize(param);
    TbRenderDevice& device = TbRenderDevice::GetInstance();

    // ルートパス設定
    {
        TbStaticString512 path = TbFileManager::GetInstance().GetRootPath(TB_FILE_PATH_ID_PRESET_ROOT);
        path.Append(SHADER_ROOT_PATH);
        TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_SHADER , path.GetBuffer() );
    }

    // PMX解読
    s_ResourceModel.Create(MODEL_PATH);
    s_ResourceModel.Activate();

    s_ResourceMotion.Create("motion/tonton/miku.vmd");
    s_ResourceMotion.Activate();

    // シェーダ読みこみ
    {
        TbShaderManager& shaderManager = device.GetShaderManager();
        TbShaderManager::ProgramCreateParam param;
        param.vertexFormat = s_ResourceModel.GetVertexFormat();
        param.shaderFilePath[TB_SHADER_TYPE_VERTEX] = VERTEX_PROGRAM_FILEPATH;
        param.shaderFilePath[TB_SHADER_TYPE_PIXEL]  = PIXEL_PROGRAM_FILEPATH;
        s_ShaderProgram = shaderManager.CreateProgram(param);
    }

    s_Object.Setup( s_ResourceModel , s_ShaderProgram );
    s_Object.SetMotion(s_ResourceMotion.GetMotion());
    s_Object.SetLoop(TB_TRUE);

    // デバイス設定
    {
        device.GetFixedShader().SetEnableLighting( TB_FALSE );
        device.SetCullMode(TB_CULL_MODE_NONE);
    }

    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , NULL ,  draw , NULL );
}

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
