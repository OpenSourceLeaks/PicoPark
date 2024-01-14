/*!
 * 固定シェーダを利用したボックス描画
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */

#include "fwd.h"
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/window/tb_window.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/resource/tb_resource_texture.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>

namespace
{

using namespace toybox;

TbIndexBuffer*  s_IndexBuffer = NULL;                    // 頂点バッファ
TbVertexBuffer* s_VertexBuffer = NULL;                   // 頂点バッファ
TbVertexFormat* s_VertexFormat = NULL;                   // 頂点フォーマット
TbResourceHandleTexture      s_ResourceTexture;

// 頂点構造体
struct SimpleVertex{
    float 	x, y, z; 	// 頂点座標
    float   normX , normY , normZ;
    TbColor diffuse; 	// 頂点の色
    float   u , v;
};

// 頂点バッファ
float width  = 1.0f;
float height = 1.0f;
float depth = 1.0f;
    
const SimpleVertex SIMPLE_VERTEX[24] = 
{
    {-width/2.0f, -height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , 0xffffffff , 0.0f , 1.0f },
    {-width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , 0xffffffff , 0.0f , 0.0f },
    { width/2.0f, -height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 0.0f , -1.0f , 0xffffffff , 1.0f , 0.0f }, 

    {-width/2.0f,  -height/2.0f, -depth/2.0f , 0.0f , -1.0f , 0.0f , 0xffffffff , 0.0f , 1.0f },
    {-width/2.0f,  -height/2.0f,  depth/2.0f , 0.0f , -1.0f , 0.0f , 0xffffffff , 0.0f , 0.0f },
    { width/2.0f,  -height/2.0f, -depth/2.0f , 0.0f , -1.0f , 0.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { width/2.0f,  -height/2.0f,  depth/2.0f , 0.0f , -1.0f , 0.0f , 0xffffffff , 1.0f , 0.0f }, 

    { -width/2.0f,  -height/2.0f, -depth/2.0f , -1.0f , 0.0f , 0.0f , 0xffffffff , 0.0f , 1.0f },
    { -width/2.0f,  -height/2.0f,  depth/2.0f , -1.0f , 0.0f , 0.0f , 0xffffffff , 0.0f , 0.0f },
    { -width/2.0f,   height/2.0f, -depth/2.0f , -1.0f , 0.0f , 0.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { -width/2.0f,   height/2.0f,  depth/2.0f , -1.0f , 0.0f , 0.0f , 0xffffffff , 1.0f , 0.0f }, 

    {-width/2.0f, -height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , 0xffffffff , 0.0f , 1.0f },
    {-width/2.0f,  height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , 0xffffffff , 0.0f , 0.0f },
    { width/2.0f, -height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { width/2.0f,  height/2.0f, depth/2.0f , 0.0f , 0.0f , 1.0f , 0xffffffff , 1.0f , 0.0f }, 

    {-width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 1.0f , 0.0f , 0xffffffff , 0.0f , 1.0f },
    {-width/2.0f,  height/2.0f,  depth/2.0f , 0.0f , 1.0f , 0.0f , 0xffffffff , 0.0f , 0.0f },
    { width/2.0f,  height/2.0f, -depth/2.0f , 0.0f , 1.0f , 0.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { width/2.0f,  height/2.0f,  depth/2.0f , 0.0f , 1.0f , 0.0f , 0xffffffff , 1.0f , 0.0f }, 

    { width/2.0f,  -height/2.0f, -depth/2.0f , 1.0f , 0.0f , 0.0f , 0xffffffff , 0.0f , 1.0f },
    { width/2.0f,  -height/2.0f,  depth/2.0f , 1.0f , 0.0f , 0.0f , 0xffffffff , 0.0f , 0.0f },
    { width/2.0f,   height/2.0f, -depth/2.0f , 1.0f , 0.0f , 0.0f , 0xffffffff , 1.0f , 1.0f }, // 頂点、法線、カラー、UV
    { width/2.0f,   height/2.0f,  depth/2.0f , 1.0f , 0.0f , 0.0f , 0xffffffff , 1.0f , 0.0f }, 

};

const TbUint32 INDEX_BUFFER[] =
{
    0 , 1 , 2 , 
    2 , 1 , 3 , 

    4 , 5 , 6 , 
    6 , 5 , 7 , 

    8 , 9 , 10 , 
    10 , 9 , 11 , 

    12 , 13 , 14 , 
    14 , 13 , 15 , 

    16 , 17 , 18 , 
    18 , 17 , 19 , 

    20 , 21 , 22 , 
    22 , 21 , 23 , 
};

// 頂点フォーマット要素
const TbVertexElement SIMPLE_VERTEX_ELEMENT[] = 
{
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 24 + sizeof(TbColor) , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 } ,
};

void draw()
{
    TbRenderPass* renderPass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_3D);
    if( !renderPass ){
        return;
    }

    renderPass->SetTexture( 0 , s_ResourceTexture.GetTexture() );
    renderPass->SetFixedShaderEnableLighting(TB_TRUE);
    renderPass->SetFixedShaderEnableLight(0,TB_TRUE);

    // 行列計算
    {
        // ワールド行列設定
        static TbUint32 count = 0;
        count = count++ % 1000;
        TbFloat32 fAngle = static_cast<TbFloat32>(count) * ( 2.0f * TB_PI_RAD ) / 1000.0f;
        TbMatrix worldMtx;
        worldMtx.Unit();

        // ビュー行列設定
        TbVector4 eye( 0.0f, 3.0f,-5.0f );
        TbVector4 at( 0.0f, 0.0f, 0.0f );
        TbVector4 up( 0.0f, 1.0f, 0.0f );
        TbMatrix viewMtx;
        TbMatrixSetLookAtLH( viewMtx , eye , at , up );

        // 射影行列設定
        TbMatrix projMtx;
        TbMatrixSetPerspectiveLH( projMtx , TB_PI_ANG32 / 4 , 640.0f / 480.0f , 0.1f , 100.0f );
        renderPass->SetWorldViewProjectionMatrix(worldMtx,viewMtx,projMtx);

        // 固定機能用マテリアル設定
        {
            TbFixedMaterial material;
            material.SetDiffuseColor( TbColor::White() );
            renderPass->SetFixedShaderMaterial( &material );
        }
        // ライト
        {
            TbFixedLight light;
            light.SetType( TB_LIGHT_TYPE_DIRECTION );
            TbVector4 dir( 0.0f , 0.0f , 1.0f , 0.0f );
            light.SetRadius ( 100.0f );
            worldMtx.RotateX( TbRadToAng32( fAngle ) );
            dir = worldMtx * dir;
            dir.Normalize();
            light.SetDirection(dir);
            renderPass->SetFixedShaderLight( 0 , &light );
        }
    }

    renderPass->SetFixedShaderVertexFormat( s_VertexFormat );
    renderPass->SetVertexBuffer( 0 , s_VertexBuffer , 0 , sizeof(SimpleVertex) );        
    renderPass->SetIndexBuffer( s_IndexBuffer );
    renderPass->DrawIndexedPrimitive( TB_PT_TRIANGLELIST , 0 , 24 , 0 , 12 );
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
    TbFrameworkSimple::SetClearColor(TbClearColor::Red());

    // ルートパス設定
//    TbFileManager::GetInstance().SetRootPath( TB_FILE_PATH_ID_PRESET_ROOT , "./../../../../resource" );
//    TbResourceManager::GetInstance().SetFilePath( 0 , TB_FILE_PATH_ID_PRESET_ROOT );

    // テクスチャ生成
    s_ResourceTexture.Create("tex/sample.tga" , 0 , TB_FILE_SYNC );
    s_ResourceTexture.Activate();

    // 頂点フォーマット、頂点バッファ作成
    s_VertexFormat = new TbVertexFormat( SIMPLE_VERTEX_ELEMENT , TB_ARRAY_COUNT_OF(SIMPLE_VERTEX_ELEMENT) );
    s_VertexBuffer = new TbVertexBuffer( s_VertexFormat , sizeof(SIMPLE_VERTEX) );
    s_VertexBuffer->Write(sizeof(SIMPLE_VERTEX),SIMPLE_VERTEX);
    s_IndexBuffer = new TbIndexBuffer( TB_INDEX_BUFFER_FORMAT_U32 , sizeof(INDEX_BUFFER) );
    s_IndexBuffer->Write( 0 , sizeof(INDEX_BUFFER) , INDEX_BUFFER );
        
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
    TB_SAFE_DELETE( s_IndexBuffer );
    s_ResourceTexture.Destroy();
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
