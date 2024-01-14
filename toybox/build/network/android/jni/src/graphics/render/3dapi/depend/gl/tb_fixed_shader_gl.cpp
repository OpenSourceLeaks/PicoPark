/*!
 * @file
 * @brief OpenGLの固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_fixed_shader.h"

#if (TB_3DAPI_IS_GL) && TB_IS_ENABLE_FIXED_SHADER

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_fixed_light.h"
#include "graphics/render/3dapi/tb_fixed_material.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "base/system/tb_assert.h"
#include "base/util/tb_depend_cast.h"

namespace toybox
{

namespace
{

// TbVertexElement::USAGEと固定シェーダ用、GLenumと頂点バッファ設定関数の対応
typedef void (_stdcall *SetVertexAttribPointerFunc)( GLint size , GLenum type , GLsizei stride , const GLvoid* pointer );
typedef void (_stdcall *SetVertexAttribPointerFunc2)( GLenum type , GLsizei stride , const GLvoid* pointer );
struct UsageFunc{
    GLenum array;
    SetVertexAttribPointerFunc func;
    SetVertexAttribPointerFunc2 func2;
};
static const UsageFunc USAGE_FUNC[] = 
{
    { GL_VERTEX_ARRAY , ::glVertexPointer , nullptr } , // USAGE_POSITION
    { 0 , nullptr , nullptr } , // USAGE_BLENDWEIGHT
    { 0 , nullptr , nullptr } , // USAGE_BLENDINDICES
    { GL_NORMAL_ARRAY , nullptr , ::glNormalPointer } , // USAGE_NORMAL
    { 0 , nullptr , nullptr } , // USAGE_PSIZE
    { GL_TEXTURE_COORD_ARRAY , ::glTexCoordPointer } ,// USAGE_TEXCOORD
    { 0 , nullptr , nullptr } , // USAGE_TANGENT
    { 0 , nullptr , nullptr } , // USAGE_BINORMAL
    { 0 , nullptr , nullptr } , // USAGE_TESSFACTOR
    { 0 , nullptr , nullptr } , // USAGE_POSITIONT
    { GL_COLOR_ARRAY , ::glColorPointer } ,// USAGE_COLOR
    { 0 , nullptr , nullptr } , // USAGE_FOG
    { 0 , nullptr , nullptr } , // USAGE_DEPTH
    { 0 , nullptr , nullptr } , // USAGE_SAMPLE
    { 0 , nullptr , nullptr } , // USAGE_USER
};

typedef void (_stdcall *SetEnableFuncType)( GLenum value );
static const SetEnableFuncType SET_ENABLE_FUNC_GL[] =
{
    ::glDisable ,
    ::glEnable ,
};

static const GLenum LIGHT_TABLE[] =
{
    GL_LIGHT0 , 
    GL_LIGHT1 , 
    GL_LIGHT2 , 
    GL_LIGHT3 , 
    GL_LIGHT4 , 
    GL_LIGHT5 , 
    GL_LIGHT6 , 
    GL_LIGHT7 , 
};

}

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShaderDepend::TbFixedShaderDepend()
    : m_BitArray()
    , m_WorldMatrix()
    , m_ViewMatrix()
    , m_BaseTexMatrix()
{
//    memo:テクスチャ座標は反転させずに、テクスチャのメモリ配置を上下逆にする
//    (そもそもDirectXとOpenGLは、デフォルトでメモリ配置も上下逆である)
//    m_BaseTexMatrix.Translate( 0.0f , 1.0f , 0.0f );
//    m_BaseTexMatrix.Set( 1 , 1 , -1.0f );
}

/*!
 * デストラクタ
 * 
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShaderDepend::~TbFixedShaderDepend()
{
}

/*!
 * デバイス生成時に呼ぶ関数
 * @author Miyake Shunsuke
 * @since 2011.04.04
 */
void TbFixedShaderDepend::Setup()
{
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMultMatrixf( reinterpret_cast<GLfloat*>(&m_BaseTexMatrix) );
//    glTexEnvi( GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_COMBINE );
}

/*!
 * 頂点属性を利用可能にする (StreamSourceを利用)
 * @author Miyake_Shunsuke
 * @since 2012.06.27
 */
TbBool TbFixedShaderDepend::EnableVertexAttribute()
{
    TbFixedShader& common = TB_DEPEND_TO_COMMON(TbFixedShader,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbRenderDeviceDepend& deviceDepend = device.GetDepend();
    const TbVertexFormat* fixedVertexFormat = common.GetVertexFormat();
    if( fixedVertexFormat ) {
        updateProjectionMatrix();
        // テクスチャを必要とするかどうか
        if( fixedVertexFormat->GetTexcoordCount() > 0 ){
            glEnable( GL_TEXTURE_2D );
        }else{
            glDisable( GL_TEXTURE_2D );
        }
        // フォーマットから頂点バッファ設定
        TbVertexStreamGL* stream = nullptr;
        const TbVertexElementGL* elements = fixedVertexFormat->GetDepend().GetElements();
        TbSint32 elementCount = fixedVertexFormat->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            stream = deviceDepend.GetVertexStream( elements[i].stream );
            if( stream && ( USAGE_FUNC[ elements[i].usage ].func || USAGE_FUNC[ elements[i].usage ].func2 ) ){
                glEnableClientState( USAGE_FUNC[ elements[i].usage ].array );

                glBindBuffer( GL_ARRAY_BUFFER , stream->vertexBuffer->GetDepend().GetVboId() );
                if( USAGE_FUNC[ elements[i].usage ].func ){
                    // size指定用
                    USAGE_FUNC[ elements[i].usage ].func( 
                            elements[i].size ,  
                            elements[i].type , 
                            stream->stride , 
                            reinterpret_cast<const void*>( stream->offset + elements[i].offset ) );
                }else if( USAGE_FUNC[ elements[i].usage ].func2 ){
                    // size固定用
                    USAGE_FUNC[ elements[i].usage ].func2( 
                            elements[i].type , 
                            stream->stride , 
                            reinterpret_cast<const void*>( stream->offset + elements[i].offset ) );
                }

            }else{
                if( !stream ){
                    TB_ASSERT( !" Vertex Stream is unset " );
                }
                if( !( USAGE_FUNC[ elements[i].usage ].func || USAGE_FUNC[ elements[i].usage ].func2 ) ){
                    TB_ASSERT( !"Invalid Format" );
                }
            }
        }
    }
    return TB_TRUE;
}

/*!
 * 頂点属性を利用可能にする
 * @author Miyake_Shunsuke
 * @since 2012.06.27
 */
TbBool TbFixedShaderDepend::EnableVertexAttribute( const void* vertexs , TbUint32 stride )
{
    TbFixedShader& common = TB_DEPEND_TO_COMMON(TbFixedShader,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbRenderDeviceDepend& deviceDepend = device.GetDepend();
    const TbVertexFormat* fixedVertexFormat = common.GetVertexFormat();
    if( fixedVertexFormat ) {
        updateProjectionMatrix();
        // テクスチャを必要とするかどうか
        if( fixedVertexFormat->GetTexcoordCount() > 0 ){
            glEnable( GL_TEXTURE_2D );
        }else{
            glDisable( GL_TEXTURE_2D );
        }
        // フォーマットから頂点バッファ設定
        TbVertexStreamGL* stream = nullptr;
        const TbVertexElementGL* elements = fixedVertexFormat->GetDepend().GetElements();
        TbSint32 elementCount = fixedVertexFormat->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            if( ( USAGE_FUNC[ elements[i].usage ].func || USAGE_FUNC[ elements[i].usage ].func2 ) ){
                glEnableClientState( USAGE_FUNC[ elements[i].usage ].array );
                if( USAGE_FUNC[ elements[i].usage ].func ){
                    USAGE_FUNC[ elements[i].usage ].func( 
                            elements[i].size , 
                            elements[i].type , 
                            stride , 
                            static_cast<const TbUint8*>(vertexs) + elements[i].offset );
                }else if( USAGE_FUNC[ elements[i].usage ].func2 ){
                    USAGE_FUNC[ elements[i].usage ].func2( 
                            elements[i].type , 
                            stride , 
                            static_cast<const TbUint8*>(vertexs) + elements[i].offset );
                }
            }else{
                TB_ASSERT( !"Invalid Format" );
            }
        }
    }
    return TB_TRUE;
}

/*!
 * 頂点属性の使用不可にする
 * @author Miyake_Shunsuke
 * @since 2012.06.27
 */
TbBool TbFixedShaderDepend::DisableVertexAttribute()
{
    TbFixedShader& common = TB_DEPEND_TO_COMMON(TbFixedShader,m_Depend);
    const TbVertexFormat* fixedVertexFormat = common.GetVertexFormat();
    if( fixedVertexFormat ){
        // フォーマットから頂点バッファ設定
        TbVertexStreamGL* stream = nullptr;
        const TbVertexElementGL* elements = fixedVertexFormat->GetDepend().GetElements();
        TbSint32 elementCount = fixedVertexFormat->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            if( USAGE_FUNC[ elements[i].usage ].func || USAGE_FUNC[ elements[i].usage ].func2 ){ 
                glDisableClientState( USAGE_FUNC[ elements[i].usage ].array );
            }else{
                TB_ASSERT( !"Invalid Format" );
            }
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 射影行列更新
 * @author Miyake Shunsuke
 * @since 2013.03.12
 */
void TbFixedShaderDepend::updateProjectionMatrix()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    if( TB_RENDER_TARGET_TYPE_VIEW == device.GetRenderTarget(0)->GetType() ){
        // 画面に直接描画の場合
        glMultMatrixf( reinterpret_cast<GLfloat*>(&m_ProjectionMatrix) );
    }else{
        TbMatrix mtx;
        mtx.Scale( 1.0f , -1.0f , 1.0f );
        mtx = mtx * m_ProjectionMatrix;
        glMultMatrixf( reinterpret_cast<GLfloat*>(&mtx) );
    }
}

////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.09.20
 */
TbFixedShader::TbFixedShader()
    : m_VertexFormat( nullptr )
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
    m_VertexFormat = format;
}

/*!
 * 行列設定
 * @author Miyake Shunsuke
 * @since 2011.03.17
 */
TbResult TbFixedShader::SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix44& matrix )
{
    typedef void (TbFixedShaderDepend::*SetterFunc)( const TbMatrix& matrix );
    // 行列モードテーブル
    static const GLenum MATRIX_MODE_TABLE[ TB_FIXED_SHADER_MATRIX_MAX ] =
    {
        GL_MODELVIEW , 
        GL_MODELVIEW , 
        GL_PROJECTION , 
        GL_TEXTURE , 
    };
    // 行列セット関数テーブル
    static const SetterFunc SETTER_FUNC_TABLE[ TB_FIXED_SHADER_MATRIX_MAX ] =
    {
        &TbFixedShaderDepend::SetWorldMatrix ,
        &TbFixedShaderDepend::SetViewMatrix ,
        &TbFixedShaderDepend::SetProjectionMatrix ,
        nullptr 
    };
    if( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX ) // 行列タイプは正しいか
    {
        if( SETTER_FUNC_TABLE[ matrixType ] ){
            (m_Depend.*SETTER_FUNC_TABLE[ matrixType ])( matrix );
        }
        glMatrixMode( MATRIX_MODE_TABLE[ matrixType ] );
        glLoadIdentity();
        if( GL_MODELVIEW == MATRIX_MODE_TABLE[ matrixType ] ) {
            TbMatrix modelView = m_Depend.GetViewMatrix() * m_Depend.GetWorldMatrix();
            glMultMatrixf( reinterpret_cast<GLfloat*>(&modelView) );
        }else if( GL_TEXTURE == MATRIX_MODE_TABLE[ matrixType ] ){
            TbMatrix texMatrix = m_Depend.GetBaseTexMatrix() * (matrix);
            glMultMatrixf( reinterpret_cast<GLfloat*>(&texMatrix) );
        }else{
            glMultMatrixf( reinterpret_cast<const GLfloat*>(&matrix) );
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 固定機能シェーダに渡す行列取得
 * @author Miyake Shunsuke
 * @since 2011.05.15
 */
TbResult TbFixedShader::GetMatrix( TbFixedShaderMatrixType matrixType , TB_OUT TbMatrix44& matrix )
{
    // 行列タイプ
    static const GLenum MATRIX_MODE_TABLE[ TB_FIXED_SHADER_MATRIX_MAX ] =
    {
        GL_MODELVIEW_MATRIX , 
        GL_MODELVIEW_MATRIX , 
        GL_PROJECTION_MATRIX , 
        GL_TEXTURE_MATRIX , 
    };
    if( 0 <= matrixType && matrixType < TB_FIXED_SHADER_MATRIX_MAX ) // 行列タイプは正しいか
    {
        glGetFloatv( MATRIX_MODE_TABLE[ matrixType ] , reinterpret_cast<GLfloat*>(&matrix) );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * シェーディングモード設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetShadeMode( TbFixedShaderShadeMode shadeMode )
{
    static const GLenum CONVERT_TABLE[] = {
        GL_FLAT , 
        GL_SMOOTH , 
    };
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_FIXED_SHADER_SHADE_MODE_MAX );
    if( 0 <= shadeMode && shadeMode < TB_FIXED_SHADER_SHADE_MODE_MAX ){
        glShadeModel( CONVERT_TABLE[ shadeMode ] );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.03.23
 */
TbResult TbFixedShader::SetEnableLighting( TbBool isEnable )
{
    SET_ENABLE_FUNC_GL[!!isEnable](GL_LIGHTING);
    return TB_S_OK;
}

/*!
 * ライト有効設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetEnableLight( TbUint32 index , TbBool isEnable )
{
    if( index < TB_ARRAY_COUNT_OF(LIGHT_TABLE) ){
        SET_ENABLE_FUNC_GL[!!isEnable](LIGHT_TABLE[index]);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * ライト設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetLight( TbUint32 index , const TbFixedLight* light )
{
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
    return TB_E_FAIL;
}

/*!
 *マテリアル設定
 * @author Miyake Shunsuke
 * @since 2011.06.19
 */
TbResult TbFixedShader::SetMaterial( const TbFixedMaterial* material )
{
    if( material ){
        const TbFloat32* color = reinterpret_cast<const TbFloat32*>(&(material->GetDiffuseColor().value));
        glMaterialfv( GL_FRONT_AND_BACK , GL_DIFFUSE , color );
       // glMaterialfv( GL_FRONT_AND_BACK , GL_SPECULAR , color );
       // glMaterialfv( GL_FRONT_AND_BACK , GL_AMBIENT , color );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

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
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_OP) == TB_TEXTURE_OP_MAX );

/*!
 * テクスチャ合成方法設定( 入力RGB )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    static const GLenum CONVERT_TABLE_ARG_INDEX[] = {
        GL_SOURCE0_RGB , 
        GL_SOURCE1_RGB , 
        GL_SOURCE2_RGB , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    if( device.GetDepend().GetCurrentTextureStage() != stage ){
        device.GetDepend().SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( RGB演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureColorOp( TbUint32 stage , TbTextureOp op )
{
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    if( deviceDepend.GetCurrentTextureStage() != stage ){
        deviceDepend.SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , GL_COMBINE_RGB , CONVERT_TABLE_TEX_OP[op] );    
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( 入力アルファ )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaArg( TbUint32 stage , TbUint32 argIndex , TbTextureArg arg )
{
    static const GLenum CONVERT_TABLE_ARG_INDEX[] = {
        GL_SOURCE0_ALPHA , 
        GL_SOURCE1_ALPHA , 
        GL_SOURCE2_ALPHA , 
    };

    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) == TB_TEXTURE_ARG_MAX );
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    if( TB_ARRAY_COUNT_OF(CONVERT_TABLE_ARG_INDEX) <= argIndex || 
        TB_ARRAY_COUNT_OF(CONVERT_TABLE_TEX_ARG) <= arg ){
        return TB_E_INVALIDARG;
    }
    if( deviceDepend.GetCurrentTextureStage() != stage ){
        deviceDepend.SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , CONVERT_TABLE_ARG_INDEX[argIndex] , CONVERT_TABLE_TEX_ARG[arg] );
    return TB_S_OK;
}

/*!
 * テクスチャ合成方法設定( アルファ演算方式 )
 * @author Miyake Shunsuke
 * @since 2011.09.23
 */
TbResult TbFixedShader::SetTextureAlphaOp( TbUint32 stage , TbTextureOp op )
{
    TbRenderDeviceDepend& deviceDepend = TbRenderDevice::GetInstance().GetDepend();
    if( deviceDepend.GetCurrentTextureStage() != stage ){
        deviceDepend.SetCurrentTextureStage( stage );
    }
    glTexEnvi( GL_TEXTURE_ENV , GL_COMBINE_ALPHA , CONVERT_TABLE_TEX_OP[op] );    
    return TB_S_OK;
}

}

#endif
