/*!
 * 固定機能シミュレータ(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_fixed_shader.h"

#if TB_3DAPI_IS_S3D

#include "graphics/3dapi/tb_render_device.h"
#include "graphics/3dapi/tb_shader_program.h"
#include "graphics/3dapi/tb_fixed_light.h"
#include "graphics/3dapi/tb_fixed_material.h"
#include "graphics/3dapi/tb_vertex_format.h"
#include "base/system/tb_assert.h"
#include "base/util/tb_depend_cast.h"

namespace toybox
{

namespace
{

    // 頂点シェーダ用コード(頂点カラー)
    const char VERTEX_SHADER_COLOR_CODE[] =
        "m44 op, va0, vc0\n"
        "mov v0, va1"; // copy color

    // 頂点シェーダ用コード(頂点カラー)
    const char VERTEX_SHADER_COLOR_TEX_CODE[] =
        "m44 op, va0, vc0\n"
        "mov v0, va1\n" // copy color
        "mov v1, va2"; // copy uv

    // ピクセルシェーダ用コード(頂点カラー)
    const char PIXEL_SHADER_COLOR_CODE[] =
        "mov oc, v0\n";
    
    // ピクセルシェーダ用コード(頂点カラー+テクスチャ)
    // 暫定頂点カラーは無視
    const char PIXEL_SHADER_COLOR_TEX_CODE[] =
        "tex ft1, v1, fs0<2d,linear,nomip>\n"
        "mov oc, ft1";

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
 * @since 2010.09.20
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
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShaderDepend::~TbFixedShaderDepend()
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    shaderManager.DeleteProgram( m_Program->program );
}

/*!
 * デバイス生成時に呼ぶ関数
 * @author Miyake Shunsuke
 * @since 2011.04.04
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
                                                                VERTEX_SHADER_COLOR_TEX_CODE , 
                                                                sizeof(VERTEX_SHADER_COLOR_TEX_CODE) );
        TbShaderId pixelShaderId = shaderManager.CreateShader(  TB_SHADER_TYPE_PIXEL ,
                                                                TB_SHADER_BUFFER_TYPE_SOURCE , 
                                                                PIXEL_SHADER_COLOR_TEX_CODE , 
                                                                sizeof(PIXEL_SHADER_COLOR_TEX_CODE) );
        program->SetShader(TB_SHADER_TYPE_VERTEX,vertexShaderId);
        program->SetShader(TB_SHADER_TYPE_PIXEL,pixelShaderId);
        program->Update();
        m_Program[SHADER_TYPE_COL_TEX].program = program;
//        m_Program[SHADER_TYPE_COL_TEX].mvpMatrixHandle = program->GetDepend().GetConstantHandle("u_ModelViewProjMatrix");
    }

}

/*!
 * 頂点属性を利用可能にする (StreamSourceを利用)
 * @author Miyake_Shunsuke
 * @since 2012.06.27
 */
TbBool TbFixedShaderDepend::EnableVertexAttribute()
{
    // どのシェーダを使うか確定
    updateShader();

    // ここでシェーダ未設定は不正とみなす
    TB_ASSERT(m_CurrentShader!=SHADER_TYPE_INVALID);

    // シェーダ側へ送る
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    return device.GetShaderManager().GetDepend().EnableVertexAttribute();
}

/*!
 * 行列設定
 * @author Miyake_Shunsuke
 * @since 2012.06.28
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
 * @since 2012.06.28
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
 * @since 2012.06.29
 */
void TbFixedShaderDepend::updateShader()
{
    if( m_FormatBitArray.IsEmpty() ) {
        // フォーマット未設定
        TB_ASSERT("Invalid Format");
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
            shader.GetDepend().SetSimulatedFixedShader( program );
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
 * @since 2012.07.03
 */
void TbFixedShaderDepend::setVertexFormat( const TbVertexFormat* format )
{
    m_BitArray.SetBit(TbFixedShaderDepend::FLAG_CHANGED_FORMAT);

    // フォーマットの用途をビット配列化しておく
    m_FormatBitArray.Clear();

    if( format ){
    
        TbSint32 elementCount = format->GetDepend().GetElementCount();
        const TbVertexElementS3d* elements = format->GetDepend().GetElements();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            if( FORMAT_FLAG_CONVERT_TABLE[elements[i].usage] != FORMAT_FLAG_INVALID ){
                m_FormatBitArray.SetBit(FORMAT_FLAG_CONVERT_TABLE[elements[i].usage]);
            }
        }

    }
}

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShader::TbFixedShader()
    : m_VertexFormat( NULL )
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShader::~TbFixedShader()
{
}

/*!
 * 頂点フォーマット設定
 * @author Miyake Shunsuke
 * @since 2010.05.10
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
 * @since 2011.03.17
 */
TbResult TbFixedShader::SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix44& matrix )
{
    return m_Depend.SetMatrix(matrixType,matrix);
}

/*!
 * 固定機能シェーダに渡す行列取得
 * @author Miyake Shunsuke
 * @since 2011.05.15
 */
TbResult TbFixedShader::GetMatrix( TbFixedShaderMatrixType matrixType , TB_OUT TbMatrix44& matrix )
{
    return m_Depend.GetMatrix(matrixType,matrix);
}

/*!
 * シェーディングモード設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetShadeMode( TbFixedShaderShadeMode shadeMode )
{
#if 0
    static const GLenum CONVERT_TABLE[] = {
        GL_FLAT , 
        GL_SMOOTH , 
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_FIXED_SHADER_SHADE_MODE_MAX );
    if( 0 <= shadeMode && shadeMode < TB_FIXED_SHADER_SHADE_MODE_MAX ){
        glShadeModel( CONVERT_TABLE[ shadeMode ] );
        return TB_S_OK;
    }
#endif
    return TB_E_FAIL;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.03.23
 */
TbResult TbFixedShader::SetEnableLighting( TbBool isEnable )
{
#if 0
    SET_ENABLE_FUNC_GL[!!isEnable](GL_LIGHTING);
#endif
    return TB_S_OK;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetEnableLight( TbUint32 index , TbBool isEnable )
{
#if 0
    if( index < TB_ARRAY_COUNT_OF(LIGHT_TABLE) ){
        SET_ENABLE_FUNC_GL[!!isEnable](LIGHT_TABLE[index]);
        return TB_S_OK;
    }
#endif
    return TB_E_FAIL;
}

/*!
 * ライト設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetLight( TbUint32 index , const TbFixedLight* light )
{
#if 0
    if( light && index < TB_ARRAY_COUNT_OF(LIGHT_TABLE) ){
        switch( light->GetType() )
        {
        case TB_LIGHT_TYPE_POINT:
            {
                const TbFloat32* color = reinterpret_cast<const TbFloat32*>(&light->GetColor().value);
                glLightfv( LIGHT_TABLE[index] , GL_DIFFUSE , color );
                glLightfv( LIGHT_TABLE[index] , GL_SPECULAR , color );
                glLightfv( LIGHT_TABLE[index] , GL_AMBIENT , color );

                TbFloat32 pos[] = { light->GetPosition().GetX() , light->GetPosition().GetY() , light->GetPosition().GetZ() , 1.0f };
                glLightfv( LIGHT_TABLE[index] , GL_POSITION , pos );
            }
            break;
        case TB_LIGHT_TYPE_DIRECTION:
            {
                const TbFloat32* color = reinterpret_cast<const TbFloat32*>(&light->GetColor().value);
                glLightfv( LIGHT_TABLE[index] , GL_DIFFUSE , color );
              //  glLightfv( LIGHT_TABLE[index] , GL_SPECULAR , color );
              //  glLightfv( LIGHT_TABLE[index] , GL_AMBIENT , color );

                TbFloat32 direction[] = { -light->GetDirection().GetX() , -light->GetDirection().GetY() , -light->GetDirection().GetZ() , 0.0f };
                glLightfv( LIGHT_TABLE[index] , GL_POSITION , direction );
            }
            break;
        default:
            return TB_E_FAIL;
        }
        return TB_S_OK;
    }
#endif
    return TB_E_FAIL;
}

/*!
 *マテリアル設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetMaterial( const TbFixedMaterial* material )
{
#if 0
    if( material ){
        const TbFloat32* color = reinterpret_cast<const TbFloat32*>(&(material->GetColor().value));
        glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE , color );
       // glMaterialfv( GL_FRONT_AND_BACK , GL_SPECULAR , color );
       // glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT , color );
        return TB_S_OK;
    }
#endif
    return TB_E_FAIL;
}

#if 0
static const GLenum CONVERT_TABLE_TEX_ARG[] = {
    GL_TEXTURE , 
    GL_PRIMARY_COLOR , 
};

static const GLenum CONVERT_TABLE_TEX_OP[] = {
    GL_REPLACE , 
    GL_MODULATE , 
    GL_ADD ,
    GL_SUBTRACT
};
#endif

/*!
 * テクスチャ合成方法設定( 入力RGB )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
#if 0
    static const GLenum CONVERT_TABLE_ARG_INDEX[] = {
        GL_SOURCE0_RGB , 
        GL_SOURCE1_RGB , 
        GL_SOURCE2_RGB , 
    };

    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    if( !m_Device ){
        return TB_E_FAIL;
    }
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    if( m_Device->GetDepend().GetCurrentTextureStage() != stage ){
        m_Device->GetDepend().SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
#endif
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( RGB演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorOp( TbUint32 stage , TbTextureOp op )
{
#if 0
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_OP) == TB_TEXTURE_OP_MAX );
    if( m_Device->GetDepend().GetCurrentTextureStage() != stage ){
        m_Device->GetDepend().SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , GL_COMBINE_RGB , CONVERT_TABLE_TEX_OP[op] );    
#endif
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( 入力アルファ )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
#if 0
    static const GLenum CONVERT_TABLE_ARG_INDEX[] = {
        GL_SOURCE0_ALPHA , 
        GL_SOURCE1_ALPHA , 
        GL_SOURCE2_ALPHA , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    if( !m_Device ){
        return TB_E_FAIL;
    }
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    if( m_Device->GetDepend().GetCurrentTextureStage() != stage ){
        m_Device->GetDepend().SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
#endif
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( アルファ演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaOp( TbUint32 stage , TbTextureOp op )
{
#if 0
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_OP) == TB_TEXTURE_OP_MAX );
    if( m_Device->GetDepend().GetCurrentTextureStage() != stage ){
        m_Device->GetDepend().SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , GL_COMBINE_ALPHA , CONVERT_TABLE_TEX_OP[op] );    
#endif
    return TB_S_OK;
}

}

#endif
