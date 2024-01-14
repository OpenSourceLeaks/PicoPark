/*!
 * @file
 * @brief 固定機能シェーダが使えないハードでの、固定機能シェーダシミュレータ
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_shader.h"

#if (TB_SHADER_API_IS_GFX) && !TB_IS_ENABLE_FIXED_SHADER

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_fixed_light.h"
#include "graphics/render/3dapi/tb_fixed_material.h"
#include "graphics/render/3dapi/tb_vertex_format.h"
#include "base/system/tb_assert.h"
#include "base/util/tb_depend_cast.h"

namespace toybox
{

namespace
{

    // 頂点シェーダ用コード
    const char VERTEX_SHADER_CODE[] =
        "#version 430                                            \n"
        "out gl_PerVertex                                        \n"
        "{                                                       \n"
        "   vec4 gl_Position;                                    \n"
        "};                                                      \n"
        "out vec4 v_FrontColor;                                  \n"
        "out vec2 v_Texcoord0;                                   \n"
        "in vec4 a_Position0;                                     \n"
        "in vec4 a_Color0;                                       \n"
        "in vec2 a_Texcoord0;                                    \n"
        "                                                        \n"
        "layout( std140 ) uniform u_ModelViewProjMatrix         \n"
        "{                                                      \n"
        "   uniform mat4 mat;                                   \n"
        "};                                                     \n"
        "                                                        \n"
        "void main()                                             \n"
        "{                                                       \n"
        "    gl_Position =  mat * a_Position0;                   \n"
        "    v_FrontColor = a_Color0;                            \n"
        "    v_Texcoord0  = a_Texcoord0;                         \n"
        "}                                                       \n";

    // ピクセルシェーダ用コード
    const char PIXEL_SHADER_CODE[] =
        "#version 430                                            \n"
        "layout( location = 0 ) out vec4 o_Color;                \n"
        "uniform sampler2D u_Texture0;                           \n"
        "in   vec4 v_FrontColor;                                 \n"
        "in   vec2 v_Texcoord0;                                  \n"
        "void main() {                                           \n"
        "    vec4 texColor = texture( u_Texture0, v_Texcoord0 ); \n"
        "    vec4 result = v_FrontColor * texColor;              \n"
        "    // gamma                                            \n"
        "    o_Color = pow(result,vec4(2.2,2.2,2.2,1.0)); \n"
        "}";

}

// TbVertexElement::UsageからTbFixedShaderDepend::FomatFlagへの変換テーブル
const TbFixedShaderDepend::FormatFlag TbFixedShaderDepend::FORMAT_FLAG_CONVERT_TABLE[] =
{
    FORMAT_FLAG_POS , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_NORMAL , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_TEXCOORD , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_COLOR , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
    FORMAT_FLAG_INVALID , 
};

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbFixedShaderDepend::TbFixedShaderDepend()
    : m_BitArray()
    , m_CurrentShader( SHADER_TYPE_INVALID )
    , m_FormatBitArray()
{
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(FORMAT_FLAG_CONVERT_TABLE)==(TbVertexElement::USAGE_MAX));

    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Matrix); ++i ){
        m_Matrix[i].Unit(); // 正規行列設定
    }
    m_BitArray.SetBit(FLAG_CHANGED_MVP_MATRIX);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFixedShaderDepend::~TbFixedShaderDepend()
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Program); ++i ){
        if( m_Program[i].program ){
            shaderManager.DeleteProgram( m_Program[i].program );
            m_Program[i].program = nullptr;
        }
    }
}

/*!
 * デバイス生成時に呼ぶ関数
 * @author Miyake Shunsuke
 */
void TbFixedShaderDepend::Setup()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbShaderManager& shaderManager = device.GetShaderManager();

    // 頂点カラー + テクスチャ用シェーダ
    {
        TbShaderProgram* program = shaderManager.CreateProgram();
        TbShaderId vertexShaderId = shaderManager.CreateShader( TB_SHADER_TYPE_VERTEX ,
                                                                TB_SHADER_BUFFER_TYPE_SOURCE , 
                                                                VERTEX_SHADER_CODE ,
                                                                TB_ARRAY_COUNT_OF(VERTEX_SHADER_CODE)-1 );
        TbShaderId pixelShaderId = shaderManager.CreateShader(  TB_SHADER_TYPE_PIXEL ,
                                                                TB_SHADER_BUFFER_TYPE_SOURCE , 
                                                                PIXEL_SHADER_CODE , 
                                                                TB_ARRAY_COUNT_OF(PIXEL_SHADER_CODE) - 1);
        program->SetShader(TB_SHADER_TYPE_VERTEX,vertexShaderId);
        program->SetShader(TB_SHADER_TYPE_PIXEL,pixelShaderId);
        program->Update();
        m_Program[SHADER_TYPE_COL_TEX].program = program;

        TbShaderConstHandle handle;
    }

}

/*!
 * 行列設定
 * @author Miyake_Shunsuke
 */
TbResult TbFixedShaderDepend::SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix& matrix )
{
    if( TB_VERIFY( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX ) ){
        m_Matrix[ matrixType ] = matrix;
        m_BitArray.SetBit(FLAG_CHANGED_MVP_MATRIX);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 行列取得
 * @author Miyake_Shunsuke
 */
TbResult TbFixedShaderDepend::GetMatrix( TbFixedShaderMatrixType matrixType , TbMatrix& matrix )
{
    if( TB_VERIFY( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX ) ){
        matrix = m_Matrix[ matrixType ];
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * シェーダ更新
 * @author Miyake_Shunsuke
 */
void TbFixedShaderDepend::Activate()
{
    if( m_FormatBitArray.IsEmpty() ) {
        // フォーマット未設定
        return;
    }

    TbFixedShader& common = TB_DEPEND_TO_COMMON(TbFixedShader,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbShaderManager& shader = device.GetShaderManager();
    if( m_BitArray.Test( FLAG_CHANGED_FORMAT ) )
    {
        m_CurrentShader = SHADER_TYPE_INVALID;

        if( m_FormatBitArray.Test(FORMAT_FLAG_POS) )
//            m_FormatBitArray.Test(FORMAT_FLAG_COLOR) && 
//            m_FormatBitArray.Test(FORMAT_FLAG_TEXCOORD) )
        {
            // シェーダを決定する
            m_CurrentShader = SHADER_TYPE_COL_TEX;
        }

        TB_ASSERT_MSG( m_CurrentShader != SHADER_TYPE_INVALID , "Unsupported Format" );

        m_BitArray.SetBit( FLAG_CHANGED_FORMAT , TB_FALSE );
    }

    // シェーダ変更
    if( m_CurrentShader != SHADER_TYPE_INVALID )
    {
        TbShaderProgram* program = m_Program[m_CurrentShader].program;
        if( program != shader.GetCurrentProgram() )
        {
            // 注) ピクセルシェーダだけ別というのは未サポート
            shader.BindVertexFormat( common.m_VertexFormat , program->GetCurrentShader(TB_SHADER_TYPE_VERTEX) );
            shader.SetProgram( program );
        
            // ModelViewProjection行列の再設定が必要
            m_BitArray.SetBit( FLAG_CHANGED_MVP_MATRIX , TB_TRUE );
        }else if( program ) {
            if( common.m_VertexFormat != program->GetVertexFormat() ){
                shader.BindVertexFormat( common.m_VertexFormat , program->GetCurrentShader(TB_SHADER_TYPE_VERTEX) );
                program->Update();
            }
        }
    }

    if( m_BitArray.Test( FLAG_CHANGED_MVP_MATRIX ) ){
        // uniform変数にモデルビュー射影行列を設定する
        shader.SetWorldViewProjectionMatrix( m_Matrix[TB_FIXED_SHADER_MATRIX_WORLD] ,
                                             m_Matrix[TB_FIXED_SHADER_MATRIX_VIEW] ,
                                             m_Matrix[TB_FIXED_SHADER_MATRIX_PROJECTION] );
        m_BitArray.SetBit( FLAG_CHANGED_MVP_MATRIX , TB_FALSE );
    }
}

/*!
 * 頂点フォーマット設定
 * @author Miyake_Shunsuke
 */
void TbFixedShaderDepend::setVertexFormat( const TbVertexFormat* format )
{
    m_BitArray.SetBit(TbFixedShaderDepend::FLAG_CHANGED_FORMAT);

    // フォーマットの用途をビット配列化しておく
    m_FormatBitArray.Clear();

    if( format ){    
        TbSint32 elementCount = format->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            const TbVertexElement* element = format->GetDepend().GetElement(i);
            if( FORMAT_FLAG_CONVERT_TABLE[element->usage] != FORMAT_FLAG_INVALID ){
                m_FormatBitArray.SetBit(FORMAT_FLAG_CONVERT_TABLE[element->usage]);
            }
        }
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        TbShaderManager& shader = device.GetShaderManager();
        if (!shader.GetCurrentProgram()) {
            Activate();
        }
    }
}

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbFixedShader::TbFixedShader()
    : m_VertexFormat( nullptr )
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFixedShader::~TbFixedShader()
{
}

/*!
 * 頂点フォーマット設定
 * @author Miyake Shunsuke
 */
void TbFixedShader::SetVertexFormat( const TbVertexFormat* format )
{
    if( m_VertexFormat != format ) 
    {
        m_VertexFormat = format;
        m_Depend.setVertexFormat( format );
    }
}

/*!
 * 行列設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix44& matrix )
{
    return m_Depend.SetMatrix(matrixType,matrix);
}

/*!
 * 固定機能シェーダに渡す行列取得
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::GetMatrix( TbFixedShaderMatrixType matrixType , TB_OUT TbMatrix44& matrix )
{
    return m_Depend.GetMatrix(matrixType,matrix);
}

/*!
 * シェーディングモード設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetShadeMode( TbFixedShaderShadeMode shadeMode )
{
    return TB_E_FAIL;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetEnableLighting( TbBool isEnable )
{
    return TB_S_OK;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetEnableLight( TbUint32 index , TbBool isEnable )
{
    return TB_E_FAIL;
}

/*!
 * ライト設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetLight( TbUint32 index , const TbFixedLight* light )
{
    return TB_E_FAIL;
}

/*!
 *マテリアル設定
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetMaterial( const TbFixedMaterial* material )
{
    return TB_E_FAIL;
}

/*!
 * テクスチャ合成方法設定( 入力RGB )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( RGB演算方式 )
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetTextureColorOp( TbUint32 stage , TbTextureOp op )
{
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( 入力アルファ )
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( アルファ演算方式 )
 * @author Miyake Shunsuke
 */
TbResult TbFixedShader::SetTextureAlphaOp( TbUint32 stage , TbTextureOp op )
{
    return TB_S_OK;
}

}

#endif
