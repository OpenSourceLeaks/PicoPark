/*!
 * シェーダでの2D描画サンプル
 * @author Miyake Shunsuke
 * @since 2011.05.12
 */

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/window/tb_window.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/io/tb_print.h>
#include <base/util/tb_scope_finalizer.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>

namespace 
{

using namespace toybox;

TbResourceHandleTexture  s_ResourceTexture;

TbVertexBuffer* s_VertexBuffer = NULL;                   // 頂点バッファ
TbVertexFormat* s_VertexFormat = NULL;                   // 頂点フォーマット
TbShaderId      s_VertexShaderId = TB_SHADER_ID_INVALID; // 頂点シェーダ
TbShaderId      s_PixelShaderId = TB_SHADER_ID_INVALID;  // ピクセルシェーダ
TbShaderProgram* s_ShaderProgram = NULL;

// 頂点構造体
struct SimpleVertex{
    float 	x, y, z; 	// 頂点座標
    TbColor diffuse; 	// 頂点の色
    float   u , v;
};

// 頂点バッファ
float width  = 1.0f;
float height = 1.0f;
static const SimpleVertex SIMPLE_VERTEX[4] = 
{
    { width/2,  height/2, 0.1f, 0xffffffff , 1.0f , 0.0f }, 
    { width/2, -height/2, 0.1f, 0xffffffff , 1.0f , 1.0f },
    {-width/2,  height/2, 0.1f, 0xffffffff , 0.0f , 0.0f },
    {-width/2, -height/2, 0.1f, 0xffffffff , 0.0f , 1.0f },
};

// 頂点フォーマット要素
static const TbVertexElement SIMPLE_VERTEX_ELEMENT[] = 
{
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3  , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR  , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 12+sizeof(TbColor) , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 } ,
};

// シェーダファイルパス
const char SHADER_ROOT_PATH[] = "/shader/" TB_SHADER_API_STR;
#if 1 // 実験
    const char VERTEX_PROGRAM_FILEPATH[] = "player.vs";
    const char PIXEL_PROGRAM_FILEPATH[] = "player.ps";
#elif TB_SHADER_API_IS_GLSL
    const char VERTEX_PROGRAM_FILEPATH[] =  "basic_tex.vert";
    const char PIXEL_PROGRAM_FILEPATH[] = "basic_tex.frag";
#elif TB_SHADER_API_IS_HLSL
    const char VERTEX_PROGRAM_FILEPATH[] = "basic_tex.vsh";
    const char PIXEL_PROGRAM_FILEPATH[] = "basic_tex.psh";
#endif
    
/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void draw()
{
    TbRenderPass* renderPass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_3D);
    if( !renderPass ){
        return;
    }

    // シェーダ設定
    renderPass->SetShaderProgram( s_ShaderProgram );

    // テクスチャ描画
    renderPass->SetTexture( 0 , s_ResourceTexture.GetTexture() );  // テクスチャ設定
    renderPass->SetVertexBuffer( 0 , s_VertexBuffer , 0 , sizeof(SimpleVertex) ); // 頂点バッファ設定
    renderPass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 0 , 2 ); // 描画コマンド発行
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
    
    // シェーダルートパス設定
    {
        TbStaticString512 path = TbFileManager::GetInstance().GetRootPath(TB_FILE_PATH_ID_PRESET_ROOT);
        path.Append(SHADER_ROOT_PATH);
        TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_SHADER , path.GetBuffer() );
    }

    // テクスチャ生成
    s_ResourceTexture.Create( "tex/sample.bmp" , 0 , TB_FILE_SYNC );
    s_ResourceTexture.Activate();

    // デバイス設定
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.SetRasterizerState(TbRenderStatePreset::GetInstance().GetRasterizerState(TbRenderStatePreset::RASTERIZER_CULL_NONE));

    // シェーダ作成
    TbSint32 vertexShaderMax = 1;
    TbSint32 pixelShaderMax = 1;
    TbShaderManager& shaderManager = device.GetShaderManager();

    s_VertexShaderId = shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_VERTEX ,
                                                           TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                           VERTEX_PROGRAM_FILEPATH,
                                                           "main",
                                                           NULL );
    s_PixelShaderId =  shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_PIXEL ,
                                                           TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                           PIXEL_PROGRAM_FILEPATH,
                                                           "main",
                                                            NULL );
    s_ShaderProgram = shaderManager.CreateProgram();
    s_ShaderProgram->SetShader(TB_SHADER_TYPE_VERTEX,s_VertexShaderId);
    s_ShaderProgram->SetShader(TB_SHADER_TYPE_PIXEL,s_PixelShaderId);
    s_ShaderProgram->Update();

    // 頂点フォーマット、頂点バッファ作成
    s_VertexFormat = new TbVertexFormat( SIMPLE_VERTEX_ELEMENT , TB_ARRAY_COUNT_OF(SIMPLE_VERTEX_ELEMENT) );
    s_VertexBuffer = new TbVertexBuffer( s_VertexFormat ,  sizeof(SIMPLE_VERTEX) );
    s_VertexBuffer->Write(sizeof(SIMPLE_VERTEX),SIMPLE_VERTEX);
    shaderManager.BindVertexFormat( s_VertexFormat , s_VertexShaderId );

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
    TB_SAFE_DELETE( s_VertexFormat );
    TB_SAFE_DELETE( s_VertexBuffer );
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
