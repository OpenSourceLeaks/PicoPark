/*!
 * NUI�̐[�x�}�X�N���ꂽ�J���[�}�b�v�ɗ֊s������������t�B���^�[
 * @author Miyake Shunsuke
 * @since 2011.10.22
 */

#include "fwd.h"
#include "render_filter_nui_outline.h"

#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <framework/sub/tb_framework_simple.h>

using namespace toybox;

namespace
{

static const char* VERTEX_SHADER_FILEPATH_SILHOUETTE = "filter_nui_silhouette.vsh";
static const char* PIXEL_SHADER_FILEPATH_SILHOUETTE  = "filter_nui_silhouette.psh";
static const char* VERTEX_SHADER_FILEPATH_OUTLINE = "filter_nui_outline.vsh";
static const char* PIXEL_SHADER_FILEPATH_OUTLINE  = "filter_nui_outline.psh";

// �e�N�X�`���t�H�[�}�b�g
static const TbVertexElement VERTEX_ELEMENT_SILHOUETTE[] = 
{
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 12 + sizeof(TbColor) , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 } ,
};

// �e�N�X�`���t�H�[�}�b�g
static const TbVertexElement VERTEX_ELEMENT_OUTLINE[] = 
{
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 12 + sizeof(TbColor) , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 } ,
    { 0 , 20 + sizeof(TbColor) , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 1 } ,
};

// ���_�\����
struct PosColTexVertex{
    float   x, y, z; // ���_���W
    TbColor diffuse; // ���_�̐F
    float   u , v;
};

// ���_�\����
struct PosColTex2Vertex{
    float   x, y, z; // ���_���W
    TbColor diffuse; // ���_�̐F
    float   u , v;
    float   u2 , v2;
};

static TbFloat32 OFFSET_Y = 0.0f;

}

/*!
 * �R���X�g���N�^
 * @author Miyake Shunsuke
 * @since 2011.10.22
 */
RenderFilterNuiOutline::RenderFilterNuiOutline( const toybox::TbRectF32& uv )
    : m_TextureSampler( NULL )

    , m_VertexShaderIdSilhouette(TB_SHADER_ID_INVALID)
    , m_PixelShaderIdSilhouette(TB_SHADER_ID_INVALID)
    , m_VertexFormatSilhouette( NULL )
    , m_VertexBufferSilhouette( NULL )
    , m_RenderTextureSilhouette( NULL )

    , m_VertexShaderIdOutline(TB_SHADER_ID_INVALID)
    , m_PixelShaderIdOutline(TB_SHADER_ID_INVALID)
    , m_VertexFormatOutline( NULL )
    , m_VertexBufferOutline( NULL )
{
    // �e�N�X�`���T���v������
    {
        m_TextureSampler = new TbTextureSampler();
        m_TextureSampler->SetMinFilterMode( TB_TEXTURE_FILTER_MODE_POINT );
        m_TextureSampler->SetMagFilterMode( TB_TEXTURE_FILTER_MODE_POINT );
        m_TextureSampler->SetAddressModeU( TB_TEXTURE_ADDRESS_MODE_CLAMP );
        m_TextureSampler->SetAddressModeV( TB_TEXTURE_ADDRESS_MODE_CLAMP );
    }

    // �����_�[�^�[�Q�b�g�p�e�N�X�`��
    {
        TbTextureDesc texDesc;
        texDesc.width = 512;
        texDesc.height = 512;
        texDesc.flags = TbTextureDesc::FLAG_BIT_RENDER_TARGET;
        texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
        texDesc.levels = 1;
        m_RenderTextureSilhouette = new TbTexture( texDesc );
    }

    // �V�F�[�_�쐬
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();;
    {
        m_VertexShaderIdSilhouette = shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_VERTEX ,
                                                                         TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                                         VERTEX_SHADER_FILEPATH_SILHOUETTE ,
                                                                         "MainVS",
                                                                         NULL );
        m_PixelShaderIdSilhouette =  shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_PIXEL ,
                                                                         TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                                         PIXEL_SHADER_FILEPATH_SILHOUETTE,
                                                                         "MainPS",
                                                                         NULL );
        m_ProgramSilhouette = shaderManager.CreateProgram();
        m_ProgramSilhouette->SetShader(TB_SHADER_TYPE_VERTEX,m_VertexShaderIdSilhouette);
        m_ProgramSilhouette->SetShader(TB_SHADER_TYPE_PIXEL,m_PixelShaderIdSilhouette);
        m_ProgramSilhouette->Update();

        m_VertexShaderIdOutline = shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_VERTEX ,
                                                                      TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                                      VERTEX_SHADER_FILEPATH_OUTLINE ,
                                                                      "MainVS",
                                                                      NULL );
        m_PixelShaderIdOutline =  shaderManager.CreateShaderFromFile( TB_SHADER_TYPE_PIXEL ,
                                                                      TB_SHADER_BUFFER_TYPE_SOURCE ,
                                                                      PIXEL_SHADER_FILEPATH_OUTLINE,
                                                                      "MainPS",
                                                                      NULL );
        m_ProgramOutline = shaderManager.CreateProgram();
        m_ProgramOutline->SetShader(TB_SHADER_TYPE_VERTEX,m_PixelShaderIdOutline);
        m_ProgramOutline->SetShader(TB_SHADER_TYPE_PIXEL,m_PixelShaderIdOutline);
        m_ProgramOutline->Update();


#if TB_SHADER_API_IS_GLSL
        shaderManager.GetDepend().setConstantName( 1 , "u_TexWidth" );
        shaderManager.GetDepend().setConstantName( 2 , "u_TexHeight" );
        shaderManager.GetDepend().setConstantName( 3 , "u_OffsetMatrix" );
#endif

   }

    const TbVertexXYZColorTex0 VERTEX_SILHOUETTE[4] = 
    {
        { 1.0f ,  1.0f , 0.1f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , 1.0f , 0.0f }, 
        { 1.0f , -1.0f , 0.1f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , 1.0f , 1.0f },
        {-1.0f ,  1.0f , 0.1f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , 0.0f , 0.0f },
        {-1.0f , -1.0f , 0.1f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , 0.0f , 1.0f },
    };
    const TbFloat32 VERTEX_SCALE = 1.0f;
    const PosColTex2Vertex VERTEX_OUTLINE[4] = 
    {
        { 540.0f/960.0f*VERTEX_SCALE ,  1.0f*VERTEX_SCALE , 0.99f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , uv.x+uv.width , uv.y , uv.x+uv.width , uv.y }, 
        { 540.0f/960.0f*VERTEX_SCALE , -1.0f*VERTEX_SCALE , 0.99f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , uv.x+uv.width , uv.y+uv.height , uv.x+uv.width , uv.y+uv.height },
        {-540.0f/960.0f*VERTEX_SCALE ,  1.0f*VERTEX_SCALE , 0.99f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , uv.x , uv.y , uv.x , uv.y },
        {-540.0f/960.0f*VERTEX_SCALE , -1.0f*VERTEX_SCALE , 0.99f , TB_COLOR_A8R8G8B8(0xFF,0xFF,0xFF,0xFF) , uv.x , uv.y+uv.height , uv.x , uv.y+uv.height },
    };

    // ���_�t�H�[�}�b�g�A���_�o�b�t�@�쐬
    m_VertexFormatSilhouette = new TbVertexFormat( VERTEX_ELEMENT_SILHOUETTE , TB_ARRAY_COUNT_OF(VERTEX_ELEMENT_SILHOUETTE) );
    m_VertexBufferSilhouette = new TbVertexBuffer( m_VertexFormatSilhouette ,  sizeof(VERTEX_SILHOUETTE) );
    m_VertexBufferSilhouette->Write(sizeof(VERTEX_SILHOUETTE),VERTEX_SILHOUETTE);

    m_VertexFormatOutline = new TbVertexFormat( VERTEX_ELEMENT_OUTLINE , TB_ARRAY_COUNT_OF(VERTEX_ELEMENT_OUTLINE) );
    m_VertexBufferOutline = new TbVertexBuffer( m_VertexFormatOutline , sizeof(VERTEX_OUTLINE) );
    m_VertexBufferOutline->Write(sizeof(VERTEX_OUTLINE),VERTEX_OUTLINE);

    // �V�F�[�_�ƒ��_�t�H�[�}�b�g���o�C���h
    shaderManager.BindVertexFormat( m_VertexFormatSilhouette , m_VertexShaderIdSilhouette );
    shaderManager.BindVertexFormat( m_VertexFormatOutline , m_VertexShaderIdOutline );

    m_ConstHandleTexWidth.Set(1);
    m_ConstHandleTexHeight.Set(2);
    m_ConstHandleOffsetMatrix.Set(3);
}

/*!
 * �f�X�g���N�^
 * @author Miyake Shunsuke
 * @since 2011.10.22
 */
RenderFilterNuiOutline::~RenderFilterNuiOutline()
{
    // ��n��
    TB_SAFE_DELETE( m_VertexFormatSilhouette );
    TB_SAFE_DELETE( m_VertexBufferSilhouette );
    TB_SAFE_DELETE( m_RenderTextureSilhouette );
    TB_SAFE_DELETE( m_VertexFormatOutline );
    TB_SAFE_DELETE( m_VertexBufferOutline );
    TB_SAFE_DELETE( m_TextureSampler );

    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    shaderManager.DeleteProgram(m_ProgramSilhouette);
    shaderManager.DeleteProgram(m_ProgramOutline);
}

/*!
 * �`��
 * @author Miyake Shunsuke
 * @since 2011.10.22
 */
TbResult RenderFilterNuiOutline::Render( TbTexture* texture , const TbRectF32& uv )
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbRenderPass* pass = TbGetRenderPass( TbFrameworkSimple::RENDER_PASS_OFFSCREEN_1 );
    TbDepthStencil* oldDepthStencil = device.GetDepthStencil();

    // �����_�����O�p�X1 ( �ڂ����V���G�b�g )
    if(1){
        pass->SetDepthStencil( NULL );
        pass->SetDepthWriteMask( TB_DEPTH_MASK_NONE );
        pass->SetEnableDepthTest( TB_TRUE );

        TbClearColor color( 0 );
        pass->SetRenderTarget( 0 , m_RenderTextureSilhouette->GetRenderTarget() ); // �����_�[�^�[�Q�b�g�ݒ�
        pass->Clear( TB_RENDER_BUFFER_TYPE_COLOR_BIT , color , 0 , 0 );
        pass->SetTextureSampler( 0 , m_TextureSampler );
        pass->SetTexture( 0 , texture );  // �e�N�X�`���ݒ�
        pass->SetShaderProgram( m_ProgramSilhouette ); // �V�F�[�_�ݒ�
        pass->SetEnableBlend(TB_TRUE);

#if 1
        // �V�F�[�_�[�p�����[�^�ݒ�
        TbFloat32 texWidth = static_cast<TbFloat32>(texture->GetWidth(0));
        TbFloat32 texHeight = static_cast<TbFloat32>(texture->GetHeight(0));
        pass->SetShaderConstantF( TB_SHADER_TYPE_BIT_PIXEL , &m_ConstHandleTexWidth , &texWidth , 1 );
        pass->SetShaderConstantF( TB_SHADER_TYPE_BIT_PIXEL , &m_ConstHandleTexHeight , &texHeight , 1 );

        // �`��
        pass->SetVertexBuffer( 0 , m_VertexBufferSilhouette , 0 , sizeof(PosColTexVertex) ); // ���_�o�b�t�@�ݒ�
        pass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 0 , 2 ); // �`��R�}���h���s
#endif

        // ��Ԃ����ɖ߂�
        pass->SetShaderProgram( NULL ); // �V�F�[�_���f�t�H���g�ݒ�
        pass->SetRenderTarget( 0 , device.GetView(0)->GetRenderTarget(0) ); // �����_�[�^�[�Q�b�g
        pass->SetDepthStencil( oldDepthStencil );
        pass->SetDepthWriteMask( TB_DEPTH_MASK_ALL );
        pass->SetEnableDepthTest( TB_TRUE );
        pass->SetTexture( 0 , NULL );
        pass->SetTexture( 1 , NULL );
    }

    // �����_�����O�p�X2( �֊s�� )
    if(1){
        TbRenderPass* pass = TbGetRenderPass( TbFrameworkSimple::RENDER_PASS_3D );

        pass->SetRenderTarget( 0 , device.GetView(0)->GetRenderTarget(0) ); // �����_�[�^�[�Q�b�g�ݒ�
        pass->SetDepthStencil( oldDepthStencil );
        pass->SetDepthWriteMask( TB_DEPTH_MASK_ALL );
        pass->SetEnableDepthTest( TB_TRUE );

        // �e�N�X�`���ݒ�
        pass->SetTexture( 0 , m_RenderTextureSilhouette );
        pass->SetTexture( 1 , texture );
        pass->SetTextureSampler( 0 , m_TextureSampler );
        pass->SetTextureSampler( 1 , m_TextureSampler );
        pass->SetShaderProgram( m_ProgramOutline ); // �V�F�[�_�ݒ�
        
        // �V�F�[�_�p�����[�^�ݒ�

        // ���_�V�F�[�_
        TbMatrix offsetMatrix;
        offsetMatrix.Unit();
        offsetMatrix.Translate( 0.0f , OFFSET_Y , 0.0f );
        pass->SetShaderConstantMatrix( TB_SHADER_TYPE_BIT_VERTEX , &m_ConstHandleOffsetMatrix , &offsetMatrix , 1 );

        // �s�N�Z���V�F�[�_
        TbFloat32 texWidth = static_cast<TbFloat32>(texture->GetWidth(0));
        TbFloat32 texHeight = static_cast<TbFloat32>(texture->GetHeight(0));
        pass->SetShaderConstantF( TB_SHADER_TYPE_BIT_PIXEL , &m_ConstHandleTexWidth , &texWidth , 1 );
        pass->SetShaderConstantF( TB_SHADER_TYPE_BIT_PIXEL ,  &m_ConstHandleTexHeight , &texHeight , 1 );

        // �`��
        pass->SetVertexBuffer( 0 , m_VertexBufferOutline , 0 , sizeof(PosColTex2Vertex) ); // ���_�o�b�t�@�ݒ�
        pass->DrawPrimitive( TB_PT_TRIANGLESTRIP , 0 , 2 ); // �`��R�}���h���s

        pass->SetShaderProgram( NULL ); // �V�F�[�_���f�t�H���g�ݒ�
        pass->SetTexture( 0 , NULL );
        pass->SetTexture( 1 , NULL );

    }

// �f�o�b�O�p
#if 1

    TbDraw2dTextureBegin();

    {
        TbDrawRectTextureParam drawParam(texture);
        drawParam.SetRect(0.0f , 0.0f , 180 , 100);
        drawParam.SetRectUV(uv);
        TbDrawRectTexture( drawParam );
    }
    {
        TbDrawRectTextureParam drawParam(m_RenderTextureSilhouette);
        drawParam.SetRect(0.0f , 100.0f , 180 , 100);
        drawParam.SetRectUV(uv);
        TbDrawRectTexture( drawParam );
    }

    TbDraw2dTextureEnd();
#endif

    return TB_S_OK;
}
