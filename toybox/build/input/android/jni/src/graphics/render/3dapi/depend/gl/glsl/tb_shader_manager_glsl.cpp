/*!
 * シェーダー管理クラス( GLSL )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "base/util/tb_depend_cast.h"

#if TB_SHADER_API_IS_GLSL

#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/depend/gl/tb_render_util_gl.h"
#include "base/math/tb_matrix.h"
#include "base/system/tb_assert.h"
#include "base/io/tb_print.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"
#include "base/string/tb_static_string.h"

namespace toybox
{

// 不正値
TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID;

namespace
{
    const TbUint32 LOG_BUFFER_SIZE = 512;
    TbChar8 s_LogBuffer[ LOG_BUFFER_SIZE ];

    TbStaticString32 s_UsageVarName[ TbVertexElement::USAGE_MAX - 1 ] =
    {
        "a_Position",
        "a_Blendweight",
        "a_BlendIndices",
        "a_Normal",
        "a_Psize",
        "a_Texcoord",
        "a_Tangent",
        "a_Binormal",
        "a_Tessfactor",
        "a_PositionT",
        "a_Color",
        "a_Fog",
        "a_Depth",
        "a_Sample",
    };

    const TbSint32 USAGE_VAR_USER_MAX = 5;

    TbStaticString32 s_UsageVarNameUser[ USAGE_VAR_USER_MAX ] =
    {
        "a_UserData0",
        "a_UserData1",
        "a_UserData2",
        "a_UserData3",
        "a_UserData4",
    };

    // システム予約用uniform変数名
    enum {
        UNIFORM_VAR_SYSTEM_MODELVIEWPROJECT_MATRIX ,
        UNIFORM_VAR_SYSTEM_MAX 
    };

    const TbUint32 CONSTANT_MAX = 10;
    TbStaticString32 s_ConstantName[ CONSTANT_MAX ] = 
    {
        "u_ModelViewProjMatrix" ,
    };

    TbStaticString32 s_TextureVarName = "u_Texture";
    
    // デフォルトのシェーダ最大数
    const TbUint32 SHADER_DEFAULT_MAX = 20;

    // 定数ハンドルを更新
    TbBool updateConstHandle( TbShaderProgram* program , TbShaderConstHandleDepend& handle ) {
        if( program ){
            program->GetDepend().UpdateConstantHandle(handle);
        }
        return TB_SHADER_GLSL_CONST_HANDLE_INVALID != handle.handle;
    }
}

/*!
 * 設定
 * @author Miyake_Shunsuke
 * @since 2014.04.30
 */
void TbShaderConstHandle::Set( TbUint32 index )
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    const TbChar8* srcName = shaderManager.GetDepend().GetConstantName( index );
    TbStrCopy( m_Depend.name , TB_ARRAY_COUNT_OF(m_Depend.name) , srcName );
    m_Depend.handle = TB_SHADER_GLSL_CONST_HANDLE_INVALID;
}

// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderManagerDepend::TbShaderManagerDepend()
    : m_BitArray()
    , m_VertexShaderPool( SHADER_DEFAULT_MAX )
    , m_PixelShaderPool( SHADER_DEFAULT_MAX  )
    , m_ModelViewProjMatrix()
#if !TB_IS_ENABLE_FIXED_SHADER
    , m_SimulatedFixedShader(nullptr)
#endif
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderManagerDepend::~TbShaderManagerDepend()
{
    // シェーダ、プログラム削除
    {
        VertexShaderPool::Iterator ite = m_VertexShaderPool.Begin();
        VertexShaderPool::Iterator endIte = m_VertexShaderPool.End();
        VertexShader* vertexShader = nullptr;
        for( ; ite != endIte; ++ite ){
            vertexShader = &*ite;
            glDeleteShader( vertexShader->shader );
        }
    }
    //
    {
        PixelShaderPool::Iterator ite = m_PixelShaderPool.Begin();
        PixelShaderPool::Iterator endIte = m_PixelShaderPool.End();
        for( ; ite != endIte; ++ite ){
            glDeleteShader( *ite );
        }
    }
}

/*!
 * 頂点属性を利用可能にする
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManagerDepend::EnableVertexAttribute()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbShaderManager& common = TbShaderManager::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return TB_FALSE;
    }
    VertexShader* currentVertexShader =  GetVertexShader(program->GetCurrentShader(TB_SHADER_TYPE_VERTEX));
    if( currentVertexShader && currentVertexShader->format ){
        updateTurnOverMatrix();
        TbVertexStreamGL* stream = nullptr;
        const TbVertexElementGL* elements = currentVertexShader->format->GetDepend().GetElements();
        TbSint32 elementCount = currentVertexShader->format->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            stream = device.GetDepend().GetVertexStream( elements[i].stream );
            if( stream ){
                glEnableVertexAttribArray( i );
                glBindBuffer( GL_ARRAY_BUFFER , stream->vertexBuffer->GetDepend().GetVboId() );
                glVertexAttribPointer( i , 
                                       elements[i].size ,  
                                       elements[i].type , 
                                       elements[i].isNormalized , 
                                       stream->stride , 
                                       reinterpret_cast<const void*>(static_cast<TbAddr>(stream->offset + elements[i].offset))
                                     );
            }else{
                TB_ASSERT( !" Vertex Stream is unset " );
            }
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 頂点属性を利用可能にする
 * @param 頂点配列
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManagerDepend::EnableVertexAttribute( const void* vertexs , TbUint32 stride )
{
    TbShaderManager& common = TbShaderManager::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return TB_FALSE;
    }
    VertexShader* currentVertexShader =  GetVertexShader(program->GetCurrentShader(TB_SHADER_TYPE_VERTEX));
    if( currentVertexShader && currentVertexShader->format ){
        updateTurnOverMatrix();
        const TbVertexElementGL* elements = currentVertexShader->format->GetDepend().GetElements();
        TbSint32 elementCount = currentVertexShader->format->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            glEnableVertexAttribArray( i );
            glVertexAttribPointer( i , 
                                   elements[i].size ,  
                                   elements[i].type , 
                                   elements[i].isNormalized , 
                                   stride , 
                                   static_cast<const TbUint8*>(vertexs) + elements[i].offset
                                 );
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 頂点属性を利用不可にする
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManagerDepend::DisableVertexAttribute()
{
    TbShaderManager& common = TbShaderManager::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return TB_FALSE;
    }
    VertexShader* currentVertexShader =  GetVertexShader(program->GetCurrentShader(TB_SHADER_TYPE_VERTEX));
    if( currentVertexShader && currentVertexShader->format ){
        TbSint32 elementCount = currentVertexShader->format->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            glDisableVertexAttribArray( i );
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 定数名設定
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
const TbChar8* TbShaderManagerDepend::GetConstantName( const TbUint32 index )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    if( index < TB_ARRAY_COUNT_OF(s_ConstantName) ){
        return s_ConstantName[index].GetBuffer();
    }
    return nullptr;
}

/*!
 * 定数名設定
 * @author Miyake Shunsuke
 * @since 2011.08.31
 */
TbResult TbShaderManagerDepend::SetConstantName( const TbUint32 index , const TbChar8* name )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    if( index >= common.GetConstantCount() ){
        return TB_E_FAIL;
    }
    s_ConstantName[ index ] = name;
    return TB_S_OK;
}

/*!
 * 頂点シェーダ追加
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addVertexShader( GLuint shader )
{
    if( shader && !m_VertexShaderPool.IsFull() ){
        VertexShader vertexShader;
        vertexShader.shader = shader;
        TbShaderId result = m_VertexShaderPool.Add( vertexShader );
        return result;
    }
    return TB_SHADER_ID_INVALID;
}

/*!
 * ピクセルシェーダ追加
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addPixelShader( GLuint shader )
{
    if( shader && !m_PixelShaderPool.IsFull() ){
        return m_PixelShaderPool.Add( shader );
    }
    return TB_SHADER_ID_INVALID;
}

/*!
 * 頂点フォーマットとシェーダをバインド
 * 既に設定されている場合は、
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManagerDepend::bindVertexFormat( const TbVertexFormat* format , const TbShaderId shaderId )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    VertexShader* shader = m_VertexShaderPool.GetAt( shaderId );
    if( shader ){
        shader->format = format;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*! 
 * テクスチャサンプラ変数情報更新
 * @author Miyake Shunsuke
 * @since 2011.02.02
 */
void TbShaderManagerDepend::updateSamplerVar()
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return;
    }
    // デバイスに設定されているテクスチャがあれば、シェーダ側でも利用していると仮定する
    // 利用している場合は、変数にバインドする
    if( device.GetActiveTextureCount() > 0 )
    {
        TbSint32 texCount = device.GetActiveTextureCount();
        TbSint32 setCount = 0;

#if 0 // デバッグ用
        GLint uniformCount = 0;
        GLint maxNameSize = 0;
        GLint length;
        GLint size;
        GLenum type;
        glGetProgramiv(m_CurrentVertexShader->program,GL_ACTIVE_UNIFORMS,&uniformCount);
        glGetProgramiv(m_CurrentVertexShader->program,GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxNameSize);
        char name[1024];
        for( GLuint u = 0; u < uniformCount; ++u ){
            glGetActiveUniform(m_CurrentVertexShader->program,u,maxNameSize,&length,&size,&type,name);
        }
#endif
        GLuint programId  = program->GetDepend().GetProgram();
        for( TbSint32 i = 0; i < TbRenderDevice::TEXTURE_STAGE_MAX && setCount < texCount ; ++i ){
            if( device.GetTexture(i) ){
                TbStaticString128 varName = TbStaticString128::Format( "%s%d" , common.GetTextureVarName() , i );
                TbSint32 location = glGetUniformLocation( programId , varName.GetBuffer() );
                if( location >= 0 ){
                    glUniform1i( location , i );
                }
                ++setCount;
            }
        }
    }
}

/*!
 * テクスチャサンプラ変数情報更新
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
void TbShaderManagerDepend::updateSamplerVar( TbUint32 slot )
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return;
    }
    GLuint programId  = program->GetDepend().GetProgram();
    TbStaticString128 varName = TbStaticString128::Format( "%s%d" , common.GetTextureVarName() , slot );
    TbSint32 location = glGetUniformLocation( programId , varName.GetBuffer() );
    if( location >= 0 ){
        glUniform1i( location , slot );
    }
}


/*!
 * シェーダー生成
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
GLuint TbShaderManagerDepend::createShader( GLenum type , const TbChar8* buffer , TbUint32 bufferSize )
{
    GLuint shader = glCreateShader( type );
    GLint size = bufferSize;
    const char* source[] = {
        buffer,
    };
    glShaderSource( shader , 1 , source , 0 == bufferSize ? nullptr : &size );
    glCompileShader( shader );
    if( isCompileError( shader ) ){
        TB_ASSERT(!"GLSL Compile Error");
        glDeleteShader( shader );
        return 0;
    }
    return shader;
}

/*!
 * コンパイルエラーチェック
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbBool TbShaderManagerDepend::isCompileError( GLuint shader )
{
    GLint   logSize;
    GLsizei length;
    GLint   isCompiled = 0;

    glGetShaderiv( shader , GL_COMPILE_STATUS , &isCompiled );
    if( !isCompiled ) {
        glGetShaderiv( shader , GL_INFO_LOG_LENGTH , &logSize );
        if( logSize > 0 ) {
            glGetShaderInfoLog( shader , LOG_BUFFER_SIZE , &length , s_LogBuffer );
            TbPrintf( "[TbShaderManager::isCompileError]%s\n" , s_LogBuffer );
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 反転行列設定更新
 * レンダーターゲットがオフスクリーンの場合、ピクセルのメモリ配置が反転してしまうための防止策
 * @author Miyake Shunsuke
 * @since 2011.10.23
 */
void TbShaderManagerDepend::updateTurnOverMatrix()
{
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();

    TbShaderConstHandle& constHandle = common.m_ConstHandle[TbShaderManager::CONST_HANDLE_WVP];
    // miyake_todo : 高速化の余地あり
    if( TB_RENDER_TARGET_TYPE_VIEW == device.GetRenderTarget(0)->GetType() ){
        // 画面に直接描画の場合
        //if( m_BitArray.Test( FLAG_TURN_OVER ) ) // この条件では抜けがありそうなので保留
        common.SetConstantMatrix( TB_SHADER_TYPE_BIT_VERTEX , constHandle , &m_ModelViewProjMatrix , 1 );
        m_BitArray.SetBit( FLAG_TURN_OVER , TB_FALSE );
    }else{
        //if( !m_BitArray.Test( FLAG_TURN_OVER ) )
        TbMatrix mtx;
        mtx.Scale( 1.0f , -1.0f , 1.0f );
        mtx = mtx * m_ModelViewProjMatrix;
        common.SetConstantMatrix( TB_SHADER_TYPE_BIT_VERTEX , constHandle , &m_ModelViewProjMatrix , 1 );
        m_BitArray.SetBit( FLAG_TURN_OVER , TB_TRUE );
    }
}

//

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderManager::TbShaderManager()
    : m_BitArray()
    , m_Depend()
    , m_ShaderProgram(nullptr)
{
    TbFill(m_ConstHandle,TB_SHADER_CONST_HANDLE_INVALID);
    m_ConstHandle[CONST_HANDLE_WVP].Set(TB_SHADER_CONST_INDEX_WVP);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderManager::~TbShaderManager()
{
}

/*!
 * シェーダプログラム生成(メモリから)
 * @param bufferType 引き渡すバッファの種類
 * @param buffer シェーダバッファ
 * @param entry GLSLでは無視されます
 * @param args GLSLでは無視されます
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderId TbShaderManager::CreateShader( TbShaderType type ,
                                          TbShaderBufferType bufferType , 
                                          const TbChar8* buffer ,
                                          const TbUint32 bufferSize ,
                                          const TbChar8* entry ,
                                          const TbChar8** args )
{
    TbShaderId result = TB_SHADER_ID_INVALID;
    if( !TB_VERIFY(buffer) ){
        return result;
    }
    switch( type )
    {
    case TB_SHADER_TYPE_VERTEX:
        if( !m_Depend.isFullVertexShaderPool() ){
            GLuint shader = m_Depend.createShader( GL_VERTEX_SHADER , buffer , bufferSize );
            if( shader > 0){
                result = m_Depend.addVertexShader( shader );
            }
        }
        break;
    case TB_SHADER_TYPE_PIXEL:
        if( !m_Depend.isFullPixelShaderPool() ){
            GLuint shader = m_Depend.createShader( GL_FRAGMENT_SHADER , buffer , bufferSize );
            if( shader > 0){
                result = m_Depend.addPixelShader( shader );
            }
        }
        break;
    default:
        {
            TB_ASSERT(0);
        }
        break;
    }
    return result;
}

/*!
 * 頂点シェーダプログラム生成(ファイルから)(完了復帰)
 * @param bufferType 引き渡すバッファの種類
 * @param buffer シェーダバッファ
 * @param entry GLSLでは無視されます
 * @param args GLSLでは無視されます
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderId TbShaderManager::CreateShaderFromFile( TbShaderType type ,
                                                  TbShaderBufferType bufferType , 
                                                  const TbChar8* filePath ,
                                                  const TbChar8* entry ,
                                                  const TbChar8** args )
{
    TbShaderId result = TB_SHADER_ID_INVALID;
    TbFile file( filePath , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_PRESET_SHADER );
    file.Open();
    file.Read();
    result = CreateShader( type , bufferType , reinterpret_cast<const TbChar8*>(file.GetReadBuffer()) , file.GetReadBufferSize() ,  entry , args );
    return result;
}

/*!
 * プログラム作成
 * @author Miyake Shunsuke
 * @since 2013.04.22
 */
TbShaderProgram* TbShaderManager::CreateProgram()
{
    return TB_NEW TbShaderProgram();
}

/*!
 * プログラム削除
 * @author Miyake Shunsuke
 * @since 2013.04.22
 */
void TbShaderManager::DeleteProgram( TbShaderProgram* program )
{
    TB_DELETE program;
}

/*!
 * シェーダプログラム設定
 * @author Miyake Shunsuke
 * @since 2013.04.21
 */
TbResult TbShaderManager::SetProgram( TbShaderProgram* program )
{
    if( m_ShaderProgram == program ) {
        return TB_S_OK;
    }
    m_ShaderProgram = program;
    if( program ){
        program->Update();
        glUseProgram( program->GetDepend().GetProgram() );
        TbCheckErrorGL();
        m_Depend.updateSamplerVar();

        // GLSLの場合はシェーダ変わるとuniform変数の場所が変わるのでリセットしておく
        for( TbUint32 i =0 ; i < TB_ARRAY_COUNT_OF(m_ConstHandle); ++i ) {
            m_ConstHandle[CONST_HANDLE_WVP].m_Depend.handle = TB_SHADER_GLSL_CONST_HANDLE_INVALID;
        }

    }else{
        glUseProgram( 0 );
        TbCheckErrorGL();
    }
    return TB_S_OK;
}

/*!
 * 頂点フォーマットを頂点シェーダにバインド
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManager::BindVertexFormat( const TbVertexFormat* format , const TbShaderId vertexShaderId )
{
    return m_Depend.bindVertexFormat( format , vertexShaderId );
}

/*!
 * 利用可能な定数数取得
 * @author Miyake Shunsuke
 * @since 2011.08.31
 */
TbUint32 TbShaderManager::GetConstantCount() const
{
    return CONSTANT_MAX;
}

/*! 
 * bool型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantBool( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbBool* value ,TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( updateConstHandle(m_ShaderProgram, handle.m_Depend) ){
        glUniform1iv( handle.m_Depend.handle , count , value );
    }
}

/*! 
 * float32型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantF32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbFloat32* value , TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( updateConstHandle(m_ShaderProgram, handle.m_Depend) ){
        glUniform1fv( handle.m_Depend.handle , count , value );
    }
}

/*! 
 * sint32型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantS32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbSint32* value , TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( updateConstHandle(m_ShaderProgram, handle.m_Depend) ){
        glUniform1iv( handle.m_Depend.handle , count , reinterpret_cast<const GLint*>(value) );
    }
}

/*! 
 * TbMatrix型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix* value , TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( updateConstHandle(m_ShaderProgram, handle.m_Depend) ){
        glUniformMatrix4fv( handle.m_Depend.handle , count , TB_FALSE , reinterpret_cast<const TbFloat32*>(value) );
    }
}

/*! 
 * TbMatrix型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2014.10.26
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix43* value , TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( updateConstHandle(m_ShaderProgram, handle.m_Depend) ){
#if TB_GLAPI_IS_GL2
        glUniformMatrix4x3fv( handle.m_Depend.handle , count , TB_FALSE , reinterpret_cast<const TbFloat32*>(value) );
#else
        TB_ASSERT(!"unsupported");
#endif
    }
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名取得
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
const TbChar8* TbShaderManager::GetElementUsageVarName( TbVertexElement::Usage usage )
{
    if( 0 <= usage && usage < TB_ARRAY_COUNT_OF( s_UsageVarName ) ){
        return s_UsageVarName[ usage ].GetBuffer();
    }
    return "";
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
void TbShaderManager::SetElementUsageVarName( TbVertexElement::Usage usage , const TbChar8* name )
{
    if( 0 <= usage && usage < TB_ARRAY_COUNT_OF( s_UsageVarName ) ){
        s_UsageVarName[ usage ] = name;
    }
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名取得
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
const TbChar8* TbShaderManager::GetElementUsageUserVarName( TbSint32 index )
{
    if( 0 <= index && index < TB_ARRAY_COUNT_OF( s_UsageVarNameUser ) ){
        return s_UsageVarNameUser[ index ].GetBuffer();
    }
    return "";
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
void TbShaderManager::SetElementUsageUserVarName( TbSint32 index , const TbChar8* name )
{
    if( 0 <= index && index < TB_ARRAY_COUNT_OF( s_UsageVarNameUser ) ){
        s_UsageVarNameUser[ index ] = name;
    }
}

/*!
 * テクスチャ変数名取得
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
const TbChar8* TbShaderManager::GetTextureVarName()
{
    return s_TextureVarName.GetBuffer();
}

/*!
 * テクスチャ変数名設定
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
void TbShaderManager::SetTextureVarName( const TbChar8* name )
{
    s_TextureVarName = name;
}

/*!
 * world view projection行列更新
 * @author Miyake Shunsuke
 */
void TbShaderManager::updateWVP()
{
    TbMatrix mtx = m_Matrix[MATRIX_PROJECTION] * m_Matrix[MATRIX_VIEW] * m_Matrix[MATRIX_WORLD];
    m_Depend.SetModelViewProjectionMatrix(mtx);
}

}

#endif