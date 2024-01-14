/*!
 * �e�N�X�`���ւ̃����_�����O�T���v��
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/image/tb_image_bmp.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/time/tb_tick.h>
#include <base/math/tb_matrix.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <input/tb_keyboard.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>

namespace
{

using namespace toybox;

TbVertexBuffer* s_VertexBuffer = NULL;   // ���_�o�b�t�@
TbVertexFormat* s_VertexFormat = NULL;   // ���_�t�H�[�}�b�g
TbDepthStencil* s_DepthStencil = NULL;   // �[�x�X�e���V���o�b�t�@
TbTexture*      s_RenderTexture = NULL;  // �����_�[�e�N�X�`��
TbTextureSampler*   s_TextureSampler = NULL; // �e�N�X�`���T���v��
TbResourceHandleTexture   s_ResourceTexture;
TbFilePathId        s_RootPathId = TB_FILE_PATH_ID_NONE; // ���[�g�p�XID

// ���_�o�b�t�@
float width  = 1.0f;
float height = 1.0f;

const TbVertexXYZColorTex0 SIMPLE_VERTEX[4] = 
{
    { width/2,  height/2, 0.1f, TbColor(0xffffffff) , 1.0f , 0.0f }, 
    { width/2, -height/2, 0.1f, TbColor(0xffffffff) , 1.0f , 1.0f },
    {-width/2,  height/2, 0.1f, TbColor(0xffffffff) , 0.0f , 0.0f },
    {-width/2, -height/2, 0.1f, TbColor(0xffffffff) , 0.0f , 1.0f },
};

const char TEX_FILE[] = "tex/sample.tga";

void* alloc( TbUint32 size ){
    return new TbUint8[size];
}

void update()
{
    // �����_�[�e�N�X�`�����t�@�C���o��
    if( TbKeyboard::IsOn(TB_KEY_A) ){
        TbFile texFile( "output.bmp" , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
        void* texBuffer = NULL;
        TbUint32 pitch;
        TbUint32 height;
        if( TB_S_OK == s_RenderTexture->Lock( 0 , TB_LOCK_FLAG_BIT_READ_ONLY , &texBuffer , pitch , height ) ){
            TbImageExportInfo exportInfo;
            exportInfo.buffer = texBuffer;
            exportInfo.bufSize = pitch * height;
            exportInfo.width = s_RenderTexture->GetWidth(0);
            exportInfo.height = s_RenderTexture->GetHeight(0);
            exportInfo.colorFormat = s_RenderTexture->GetFormat(0);
            exportInfo.flag = 0;
            TbImageExportResult exportResult;
            if( TbImageBmp::Export( exportResult , exportInfo , alloc ) ){
                texFile.Write( exportResult.fileBuf , exportResult.fileBufSize ); 
            }
        }
        s_RenderTexture->Unlock( 0 );
    }
}

void draw()
{
    // 3D�����_�����O
    {
        TbRenderPass* renderPass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_OFFSCREEN_1);

        // �����_�[�e�N�X�`���ݒ�
        renderPass->SetDepthStencil( s_DepthStencil );
        renderPass->SetRenderTarget( 0 , s_RenderTexture->GetRenderTarget() );
        renderPass->Clear( TB_RENDER_BUFFER_TYPE_COLOR_BIT | TB_RENDER_BUFFER_TYPE_DEPTH_BIT , TbColor::Red() , 1.0f , 0 );
        renderPass->SetTexture( 0 , s_ResourceTexture.GetTexture() );

        // �s��v�Z
        {
#if 1
            // ���[���h�s��ݒ�
            static TbUint32 count = 0;
            count = count++ % 1000;
            TbFloat32 fAngle = static_cast<TbFloat32>(count) * ( 2.0f * TB_PI_RAD ) / 1000.0f;
            TbMatrix worldMtx;
            worldMtx.RotateY( TbRadToAng32( fAngle ) );

            // �r���[�s��ݒ�
            TbVector4 eye( 0.0f, 3.0f,-5.0f );
            TbVector4 at( 0.0f, 0.0f, 0.0f );
            TbVector4 up( 0.0f, 1.0f, 0.0f );
            TbMatrix viewMtx;
            TbMatrixSetLookAtLH( viewMtx , eye , at , up );

            // �ˉe�s��ݒ�
            TbMatrix projMtx;
            TbMatrixSetPerspectiveLH( projMtx , TB_PI_ANG32 / 4 , 1.0f , 0.0f , 100.0f );
            renderPass->SetWorldViewProjectionMatrix( worldMtx , viewMtx , projMtx );
#else
            renderPass->SetWorldViewProjectionMatrix( TbMatrix::IDENTITY , TbMatrix::IDENTITY , TbMatrix::IDENTITY );
#endif
        }

        renderPass->SetFixedShaderVertexFormat( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 );
        renderPass->SetVertexBuffer( 0 , s_VertexBuffer , 0 , sizeof(TbVertexXYZColorTex0) );
        renderPass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 0 , 2 );

        // �����_�[�^�[�Q�b�g�A�[�x�o�b�t�@�����ɖ߂�
        renderPass->SetDepthStencil( NULL );
        renderPass->SetRenderTarget( 0 , TbRenderDevice::GetInstance().GetView(0)->GetRenderTarget(0) );
    }

    // 2D�����_�����O
    {
        // �����_�[�e�N�X�`���`��
        TbDraw2dSetRenderPass(TbFrameworkSimple::RENDER_PASS_2D);
        TbDraw2dTextureBegin();
        TbDrawRectTextureParam drawParam(s_RenderTexture);
        drawParam.SetRect(0.0f , 100.0f , 320.0f , 240.0f);
        TbDrawRectTexture( drawParam );
        TbDraw2dTextureEnd();
    }
}

}

namespace toybox
{

/*!
 * �G���g���[�|�C���g
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    // ������
    TbFrameworkSimple::Initialize();
    TbFrameworkSimple::SetClearColor( TbClearColor::Blue());

    // �f�o�C�X�ݒ�
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.SetCullMode( TB_CULL_MODE_NONE );

    // �����_�[�e�N�X�`������
    TbTextureDesc renderTextureDesc;
    renderTextureDesc.width = 512;
    renderTextureDesc.height = 512;
    renderTextureDesc.levels = 0;
    renderTextureDesc.flags = TbTextureDesc::FLAG_BIT_RENDER_TARGET;
    renderTextureDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
    s_RenderTexture = new TbTexture( renderTextureDesc );
    s_DepthStencil = new TbDepthStencil( renderTextureDesc.width , renderTextureDesc.height , TB_COLOR_FORMAT_D16 );

    // �e�N�X�`������
    s_ResourceTexture.Create( TEX_FILE , 0 , TB_FILE_SYNC );
    s_ResourceTexture.Activate();

    // ���_�t�H�[�}�b�g�A���_�o�b�t�@�쐬
    s_VertexBuffer = new TbVertexBuffer( &TbVertexFormat::XYZ_COLOR_TEX0SIZE2 ,  sizeof(SIMPLE_VERTEX[0])*4 );
    s_VertexBuffer->Write(sizeof(SIMPLE_VERTEX[0])*4,SIMPLE_VERTEX);

    // ���C�����[�v
    TbFrameworkSimple::StartMainLoop( NULL , update , draw , NULL );
}
    
/*!
 * �I��
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    TB_SAFE_DELETE( s_VertexFormat );
    TB_SAFE_DELETE( s_VertexBuffer );
    TB_SAFE_DELETE( s_RenderTexture );
    TB_SAFE_DELETE( s_TextureSampler );
    TbFrameworkSimple::Finalize();
}

/*!
 * ��ʂ̏c�A���T�C�Y���ς����
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
void TbFramework::OnResizeDisplay( TbSint32 w , TbSint32 h )
{
    // ���ɉ������Ȃ�
}

}
