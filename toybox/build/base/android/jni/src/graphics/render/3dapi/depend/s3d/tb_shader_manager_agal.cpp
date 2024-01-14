/*!
 * シェーダー管理クラス( GLSL )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_shader_manager.h"
#include "base/util/tb_depend_cast.h"

#if TB_SHADER_API_IS_AGAL

#include "graphics/3dapi/tb_shader_program.h"
#include "graphics/3dapi/tb_render_device.h"
#include "graphics/3dapi/tb_render_target.h"
#include "graphics/3dapi/tb_vertex_buffer.h"
#include "graphics/3dapi/tb_vertex_format.h"
#include "graphics/3dapi/depend/s3d/tb_render_util_s3d.h"
#include "base/math/tb_matrix.h"
#include "base/system/tb_assert.h"
#include "base/io/tb_print.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"
#include "base/string/tb_static_string.h"

namespace toybox
{

// 不正値
TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID =  0xFFFFFFFF;

namespace
{

// デフォルトのシェーダ最大数
const TbUint32 SHADER_DEFAULT_MAX = 20;
    
template<typename T>
void setConstantValue( TbUint32 shaderBit , const TbShaderConstHandle& handle , const T* value , TbUint32 count )
{
    if( handle != TB_SHADER_CONST_HANDLE_INVALID ){
        AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
        TbUint32 registerIndex = handle;
        TbSint32 counter = static_cast<TbSint32>(count);
        TbUint32 valueIndex = 0;
        while( counter > 0 ){
            AS3::ui::var targetValue = AS3::ui::internal::new_Vector_Number();
            for( TbSint32 i = 0; i < counter%4; ++i ) {
                targetValue[i] = AS3::ui::internal::new_Number(value[valueIndex]);
                ++valueIndex;
            }
            if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
                context->setProgramConstantsFromVector(AS3::ui::flash::display3D::Context3DProgramType::VERTEX,
                                                       registerIndex,targetValue,false);
            }
            if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
                context->setProgramConstantsFromVector(AS3::ui::flash::display3D::Context3DProgramType::FRAGMENT,
                                                       registerIndex,targetValue,false);
            }
            counter -= 4;
            ++registerIndex;
        }
    }
}

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
    , m_ModelViewProjMatrixHandle()
    , m_ModelViewProjMatrix()
#if !TB_IS_ENABLE_FIXED_SHADER
    , m_SimulatedFixedShader(NULL)
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
}

/*!
 * 頂点属性を利用可能にする
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbBool TbShaderManagerDepend::EnableVertexAttribute()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    AS3::ui::flash::display3D::Context3D& context = device.GetDepend().GetContext();
    TbShaderManager& common = TbShaderManager::GetInstance();
    const TbShaderProgram* program =  common.GetCurrentProgram();
    if( !program ) {
        return TB_FALSE;
    }
    VertexShader* currentVertexShader =  GetVertexShader(program->GetCurrentShader(TB_SHADER_TYPE_VERTEX));
    if( currentVertexShader && currentVertexShader->format ){
        updateTurnOverMatrix();
        const TbVertexElementS3d* elements = currentVertexShader->format->GetDepend().GetElements();
        TbSint32 elementCount = currentVertexShader->format->GetDepend().GetElementCount();
        for( TbSint32 i = 0; i < elementCount; ++i ){
            TbRenderDeviceDepend::VertexStream* stream = device.GetDepend().GetVertexStream( elements[i].stream );
            if( stream ){
                TbUint32 offset = ((stream->offset + elements[i].offset)) >> 2; // 4分の1
                context->setVertexBufferAt( i , 
                                            stream->vertexBuffer->GetDepend().GetVertexBuffer() , 
                                            offset , 
                                            elements[i].type );
            }else{
                TB_ASSERT( !" Vertex Stream is unset " );
            }
        }
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 頂点シェーダ追加
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addVertexShader( Assembler& shader )
{
    if( !m_VertexShaderPool.IsFull() ){
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
TB_INLINE TbShaderId TbShaderManagerDepend::addPixelShader( Assembler& shader )
{
    if( !m_PixelShaderPool.IsFull() ){
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
 * シェーダー生成
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TB_INLINE TbShaderManagerDepend::Assembler TbShaderManagerDepend::createShader( TbShaderType type , const TbChar8* buffer )
{
    AS3::ui::String typeStr;
    Assembler result;
    if( TB_VERIFY(TbShaderTypeToAS3String(typeStr,type)) ){
        result = Assembler::_new(false);
        result->assemble( typeStr,
                        //"m44 vt0, va0, vc0\n"
                        //"mov op, va0\n"
                        buffer
                      );
    }
    return result;
}

/*!
 * 反転行列設定更新
 * レンダーターゲットがオフスクリーンの場合、ピクセルのメモリ配置が反転してしまうための防止策
 * @author Miyake Shunsuke
 * @since 2011.10.23
 */
void TbShaderManagerDepend::updateTurnOverMatrix()
{
    if( m_ModelViewProjMatrixHandle == TB_SHADER_CONST_HANDLE_INVALID ){
        return;
    }
    TbShaderManager& common = TB_DEPEND_TO_COMMON(TbShaderManager,m_Depend);
    TbRenderDevice& device = TbRenderDevice::GetInstance();

    // miyake_todo : 高速化の余地あり
    if( TB_RENDER_TARGET_TYPE_VIEW == device.GetRenderTarget(0)->GetType() ){
        // 画面に直接描画の場合
        //if( m_BitArray.Test( FLAG_TURN_OVER ) ) // この条件では抜けがありそうなので保留
        if( TB_SHADER_CONST_HANDLE_INVALID != m_ModelViewProjMatrixHandle )
        {
            common.SetConstantMatrix( TB_SHADER_TYPE_BIT_VERTEX , m_ModelViewProjMatrixHandle , &m_ModelViewProjMatrix , 1 );
            m_BitArray.SetBit( FLAG_TURN_OVER , TB_FALSE );
        }
    }else{
        //if( !m_BitArray.Test( FLAG_TURN_OVER ) )
        if( TB_SHADER_CONST_HANDLE_INVALID != m_ModelViewProjMatrixHandle )
        {
            TbMatrix mtx;
            mtx.Scale( 1.0f , -1.0f , 1.0f );
            mtx = mtx * m_ModelViewProjMatrix;
            common.SetConstantMatrix( TB_SHADER_TYPE_BIT_VERTEX , m_ModelViewProjMatrixHandle , &m_ModelViewProjMatrix , 1 );
            m_BitArray.SetBit( FLAG_TURN_OVER , TB_TRUE );
        }
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
    , m_ShaderProgram(NULL)
{
    TbFill(m_ConstHandle,TB_SHADER_CONST_HANDLE_INVALID);
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
            TbShaderManagerDepend::Assembler vasm = m_Depend.createShader( type , buffer );
            if( vasm != TbShaderManagerDepend::Assembler() ){
                result = m_Depend.addVertexShader( vasm );
            }
        }
        break;
    case TB_SHADER_TYPE_PIXEL:
        if( !m_Depend.isFullPixelShaderPool() ){
            TbShaderManagerDepend::Assembler fasm = m_Depend.createShader( type , buffer );
            if( fasm != TbShaderManagerDepend::Assembler() ){
                result = m_Depend.addPixelShader( fasm );
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
    TbFile* file = TbFileManager::GetInstance().CreateFileHandle( filePath , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_PRESET_SHADER );
    file->Open();
    file->Read();
    result = CreateShader( type , bufferType , reinterpret_cast<const TbChar8*>(file->GetReadBuffer()) , file->GetReadBufferSize() ,  entry , args );
    TbFileManager::GetInstance().DeleteFileHandle( file );
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
    AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
    if( m_ShaderProgram == program ) {
        return TB_S_OK;
    }
    if( program ){
        program->Update();
        context->setProgram(program->GetDepend().GetProgram());
    }
    m_ShaderProgram = program;
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
    // 頂点シェーダは128、ピクセルシェーダは28
    // 暫定対応
    return 128;
}

/*!
 * 頂点シェーダから定数ハンドルを取得
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
TbShaderConstHandle TbShaderManager::GetConstantHandle( TbUint32 index )
{
    return index;
}

/*! 
 * bool型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantBool( TbUint32 shaderBit , const TbShaderConstHandle& handle , const TbBool* value ,TbUint32 count )
{
    setConstantValue( shaderBit , handle , value , count );
}

/*! 
 * float32型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantF32( TbUint32 shaderBit , const TbShaderConstHandle& handle , const TbFloat32* value , TbUint32 count )
{
    setConstantValue( shaderBit , handle , value , count );
}

/*! 
 * sint32型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantS32( TbUint32 shaderBit , const TbShaderConstHandle& handle , const TbSint32* value , TbUint32 count )
{
    setConstantValue( shaderBit , handle , value , count );
}

/*! 
 * TbMatrix型頂点シェーダ定数設定( ハンドルから )
 * @author Miyake Shunsuke
 * @since 2010.10.26
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , const TbShaderConstHandle& handle , const TbMatrix* value , TbUint32 count )
{
    TB_UNUSE(shaderBit);
    if( handle != TB_SHADER_CONST_HANDLE_INVALID ){
        AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
        TbUint32 index = handle;
        for( TbUint32 i = 0; i < count; ++i )
        {
            AS3::ui::flash::geom::Matrix3D mtx = AS3::ui::flash::geom::Matrix3D::_new();
            AS3::ui::var mtxRawData = AS3::ui::internal::new_Vector_Number();
            const float* rawArray =reinterpret_cast<const float*>(value);
            for( TbUint32 valIndex = 0; valIndex < 16; ++valIndex ) {
                mtxRawData[valIndex] = AS3::ui::internal::new_Number(rawArray[valIndex]);
            }
            mtx->copyRawDataFrom(mtxRawData);
            if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
                context->setProgramConstantsFromMatrix(AS3::ui::flash::display3D::Context3DProgramType::VERTEX,
                                                       index,mtx,true);
            }
            if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
                context->setProgramConstantsFromMatrix(AS3::ui::flash::display3D::Context3DProgramType::FRAGMENT,
                                                       index,mtx,true);
            }
            index += 4;
        }
    }
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名取得
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
const TbChar8* TbShaderManager::GetElementUsageVarName( TbVertexElement::Usage usage )
{
}

/*!
 * TbVertexElement::Usage に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
void TbShaderManager::SetElementUsageVarName( TbVertexElement::Usage usage , const TbChar8* name )
{
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名取得
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
const TbChar8* TbShaderManager::GetElementUsageUserVarName( TbSint32 index )
{
}

/*!
 * TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
void TbShaderManager::SetElementUsageUserVarName( TbSint32 index , const TbChar8* name )
{
}

/*!
 * テクスチャ変数名取得
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
const TbChar8* TbShaderManager::GetTextureVarName()
{
    return NULL;
}

/*!
 * テクスチャ変数名設定
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
void TbShaderManager::SetTextureVarName( const TbChar8* name )
{
}

/*!
 * world view projection行列更新
 * @author Miyake Shunsuke
 */
void TbShaderManager::updateWVP()
{
    if( TB_SHADER_CONST_HANDLE_INVALID == m_ConstHandle[CONST_HANDLE_WVP] ){
        m_ConstHandle[CONST_HANDLE_WVP] = GetConstantHandle(TB_SHADER_CONST_INDEX_WVP);
    }
    TbMatrix mtx = m_Matrix[MATRIX_PROJECTION] * m_Matrix[MATRIX_VIEW] * m_Matrix[MATRIX_WORLD];
    m_Depend.SetModelViewProjectionMatrix(m_ConstHandle[CONST_HANDLE_WVP],mtx);
}

}

#endif