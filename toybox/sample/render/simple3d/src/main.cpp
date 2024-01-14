#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/io/resource/tb_resource_handle.h>
#include <base/math/tb_matrix.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <graphics/image/tb_image_tga.h>

namespace
{
using namespace toybox;

TbVertexBuffer*         s_VertexBuffer = NULL;   // 頂点バッファ
TbResourceHandleTexture s_ResourceTexture;

// 頂点バッファ
float width  = 2.0f;
float height = 2.0f;
static const TbVertexXYZColorTex0 SIMPLE_VERTEX[] = 
{
#if 1
    { width/2,  height/2, 0.2f, 0xffffffff , 1.0f , 0.0f }, 
    { width/2, -height/2, 0.2f, 0xffffffff , 1.0f , 1.0f },
    {-width/2,  height/2, 0.2f, 0xffffffff , 0.0f , 0.0f },
    {-width/2, -height/2, 0.2f, 0xffffffff , 0.0f , 1.0f },
#else
    { width / 2,  height / 2, 0.2f, 0xffffffff , 1.0f , 0.0f },
    { width / 2, -height / 2, 0.2f, 0xffffffff , 1.0f , 1.0f },
    { -width / 2,  height / 2, 0.2f, 0xffffffff , 0.0f , 0.0f },
    { -width / 2, -height / 2, 0.2f, 0xffffffff , 0.0f , 1.0f },
#endif
};

void draw()
{
    // メインループ
    TbRenderPass* renderPass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_3D);
    if( !renderPass ){
        return;
    }

#if 1
    TbDraw2dTextureBegin();
    // 矩形描画
    TbDrawRectTextureParam rectParam( s_ResourceTexture.GetData()->GetTexture() );
    rectParam.SetRect( 100.0f , 100.0f , 100.0f , 100.0f );
    TbDrawRectTexture( rectParam );
    TbDraw2dTextureEnd();
#endif

#if 1
    // 行列計算
    {
        // ワールド行列設定
        static TbUint32 count = 0;
        count = (count+1) % 1000;
        TbFloat32 fAngle = static_cast<TbFloat32>(count) * ( 2.0f * TB_PI_RAD ) / 1000.0f;
        TbMatrix worldMtx;
        worldMtx.RotateY( TbRadToAng32( fAngle ) );
//        worldMtx.RotateZ( TbDegToAng32( 90.0f ) );

        // ビュー行列設定
        TbVector4 eye( 0.0f, 3.0f,-5.0f );
        TbVector4 at( 0.0f, 0.0f, 0.0f );
        TbVector4 up( 0.0f, 1.0f, 0.0f );
        TbMatrix viewMtx;
        TbMatrixSetLookAtLH( viewMtx , eye , at , up );

        // 射影行列設定
        TbMatrix projMtx;
        TbMatrixSetPerspectiveLH( projMtx , TbRadToAng32(45.0f*TB_PI_RAD/180.0f) , 1.2f , 0.1f , 10.0f );
        renderPass->SetWorldViewProjectionMatrix(worldMtx,viewMtx,projMtx);
    }

    if( s_ResourceTexture.GetData() ) {
        renderPass->SetTexture( 0 , s_ResourceTexture.GetData()->GetTexture() );
        renderPass->SetBlendState(TbRenderStatePreset::BLEND_NORMAL);
        renderPass->SetFixedShaderVertexFormat( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 );
        renderPass->SetVertexBuffer( 0 , s_VertexBuffer , 0 , sizeof(SIMPLE_VERTEX[0]) );        
        renderPass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 0 , 2 );
    }
#endif
}

}

namespace toybox
{

/*!
 * メイン関数
 * @author Miyake Shunsuke
 * @since 2011.05.13
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{    
    // 初期化
    TbFrameworkSimple::Initialize();

    // テクスチャ生成
    s_ResourceTexture.Create( "tex/sample.bmp" );
    s_ResourceTexture.Activate();

    // 頂点フォーマット、頂点バッファ作成
    s_VertexBuffer = new TbVertexBuffer( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 , sizeof(SIMPLE_VERTEX) );
    s_VertexBuffer->Write(sizeof(SIMPLE_VERTEX),SIMPLE_VERTEX);

    // デバイス設定
    {
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        device.GetFixedShader().SetEnableLighting( TB_FALSE );
        device.SetRasterizerState(TbRenderStatePreset::GetInstance().GetRasterizerState(TbRenderStatePreset::RASTERIZER_CULL_NONE));
    }

    TbDraw2dTextureAllocVertexBuffer(1024*1024);    
    TbFrameworkSimple::SetClearColor(TbColor::Red());
    TbFrameworkSimple::StartMainLoop( NULL , NULL ,  draw , NULL );
}

void TbFramework::OnExitPoint()
{
    TbDraw2dTextureFreeVertexBuffer();
    TB_SAFE_DELETE( s_VertexBuffer );
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
