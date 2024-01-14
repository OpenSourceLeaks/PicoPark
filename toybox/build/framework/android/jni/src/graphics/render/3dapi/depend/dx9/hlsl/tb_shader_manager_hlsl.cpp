/*!
 * @file
 * @brief シェーダ管理クラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_shader_program.h"

#if TB_SHADER_API_IS_HLSL

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_constant_buffer.h"
#include "base/math/tb_matrix.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"

namespace toybox
{

TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID;

namespace
{
    const TbChar8* SHADER_PROFILE[] = {
        "vs_3_0",
        "ps_3_0",
        nullptr
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SHADER_PROFILE)==TB_SHADER_TYPE_MAX);

    // デフォルトのシェーダ最大数
    const TbUint32 SHADER_DEFAULT_MAX = 20;

    class TbShaderIncludeHandler : public ID3DXInclude
    {
    public:

        // コンストラクタ
        TbShaderIncludeHandler()
                : m_File()
        {}

        // デストラクタ
        ~TbShaderIncludeHandler() {}

    public:
        // ファイルオープン
        STDMETHOD(Open)( D3DXINCLUDE_TYPE IncludeType,
                         LPCSTR pFileName,
                         LPCVOID pParentData,
                         LPCVOID *ppData,
                         UINT *pByteLen )
        {
            m_File.Open( pFileName , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_PRESET_SHADER );
            m_File.Read();
            *ppData = m_File.GetReadBuffer();
            *pByteLen = (m_File.GetReadBufferSize());
            return S_OK;
        }

        // ファイルクローズ
        STDMETHOD(Close)(LPCVOID pData)
        {
            m_File.Release();
            return S_OK;
        }

    protected:
        const char* m_IncludePath;
        TbFile m_File;
    };

    const TbUint32 SYSTEM_CONST_HANDLE_SIZE[] = 
    {
        16 , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SYSTEM_CONST_HANDLE_SIZE)==TB_SHADER_CONST_INDEX_SYSTEM_MAX);

    ID3DXBuffer* compileShader(TbShaderType type,
                       TbShaderBufferType bufferType,
                       const TbChar8* buffer,
                       const TbUint32 bufferSize,
                       const TbChar8* entry,
                       const TbChar8** args )
    {
        if (type < TB_SHADER_TYPE_BEGIN || TB_SHADER_TYPE_END <= type) {
            return nullptr;
        }
        static const TbUint32 ARG_MAX = 5;
        ID3DXBuffer* shaderCode;
        ID3DXBuffer* errorMsg;
        TbShaderIncludeHandler includeHandler;

        TbStaticString64 argsStr[ARG_MAX];
        D3DXMACRO macro[ARG_MAX];
        if (args)
        {
            TbMemClear(macro);
            TbUint32 i = 0;
            while (args[i] && i < ARG_MAX)
            {
                argsStr[i] = args[i];
                TbSizeT pos = argsStr[i].Find('=');
                if (pos == TB_STRING_NPOS) {
                    macro[i].Name = args[i];
                }
                else {
                    argsStr[i][pos] = '\0';
                    macro[i].Name = argsStr[i].GetBuffer();
                    macro[i].Definition = &argsStr[i].GetBuffer()[pos + 1];
                }
                ++i;
            }
        }
        else
        {
            macro[0].Name = nullptr;
            macro[0].Definition = nullptr;
        }

        HRESULT hr = D3DXCompileShader(buffer,
            bufferSize,
            macro,
            &includeHandler,
            entry,
            SHADER_PROFILE[type],
            0,
            &shaderCode,
            &errorMsg,
            nullptr);
        if (FAILED(hr))
        {
            if (errorMsg) {
                TB_ASSERT_MSG(TB_FALSE, (char*)errorMsg->GetBufferPointer());
            }
            else {
                TB_ASSERT(TB_FALSE);
            }
            TB_SAFE_RELEASE(shaderCode);
            return nullptr;
        }
        return shaderCode;
    }
    
}

/*!
 * 現在バインドされているプログラムから定数ハンドルを取得
 * @param index インデックス番号( レジスタ番号と1:1です )
 * @author Miyake Shunsuke
 * @since 2011.08.31
 */
void TbShaderConstHandle::Set( TbUint32 index )
{
    TbShaderManager& shader = TbShaderManager::GetInstance();
    if( index < shader.GetConstantCount() ){
        m_Depend.reg = shader.GetDepend().GetConstRegister(index);
    }
}


/*!
 * コンストラクタ
 */
TbShaderManagerDepend::TbShaderManagerDepend()
    : m_Device( nullptr )
    , m_VertexShaderPool(SHADER_DEFAULT_MAX)
    , m_PixelShaderPool(SHADER_DEFAULT_MAX)
{
    TbMemClear(m_RegisterList);
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_RegisterList); ++i ){
        if( i < TB_ARRAY_COUNT_OF(SYSTEM_CONST_HANDLE_SIZE) ){
            m_RegisterList[i+1] = SYSTEM_CONST_HANDLE_SIZE[i];
        }else{
            m_RegisterList[i+1] = 1;
        }
        m_RegisterList[i+1] += m_RegisterList[i];
    }
}

/*!
 * デストラクタ
 */
TbShaderManagerDepend::~TbShaderManagerDepend()
{
    deleteShaderAll( TB_FALSE );
}

/*!
 * 頂点シェーダ追加
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TB_INLINE TbShaderId TbShaderManagerDepend::addVertexShader( LPDIRECT3DVERTEXSHADER9 shader )
{
    VertexShader vertexShader;
    vertexShader.shader = shader;
    return m_VertexShaderPool.Add( vertexShader );
}

/*!
 * シェーダ全削除
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
void TbShaderManagerDepend::deleteShaderAll( TbBool isEnableReset )
{
    if( isEnableReset ){
        m_Device->GetDepend().GetDeviceDX9()->SetVertexShader(nullptr);
        m_Device->GetDepend().GetDeviceDX9()->SetPixelShader(nullptr);
    }

    {
        VertexShaderPool::Iterator ite = m_VertexShaderPool.Begin();
        VertexShaderPool::Iterator endIte = m_VertexShaderPool.End();
        for( ; ite != endIte; ++ite ){
            TB_SAFE_RELEASE( (*ite).shader );
        }
        m_VertexShaderPool.Clear();
    }
    {
        PixelShaderPool::Iterator ite = m_PixelShaderPool.Begin();
        PixelShaderPool::Iterator endIte = m_PixelShaderPool.End();
        LPDIRECT3DPIXELSHADER9 pixelShader = nullptr;
        for(; ite != endIte; ++ite ){
            pixelShader = (*ite);
            TB_SAFE_RELEASE( pixelShader );
        }
        m_PixelShaderPool.Clear();
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
 * ピクセルシェーダプログラム生成(メモリから)
 * @author Miyake Shunsuke
 * @since 2010.07.20
 */
TbShaderId TbShaderManager::CreateShader( TbShaderType type ,
                                          TbShaderBufferType bufferType , 
                                          const TbChar8* buffer ,
                                          const TbUint32 bufferSize ,
                                          const TbChar8* entry ,
                                          const TbChar8** args )
{
    ID3DXBuffer* shaderCode = compileShader(type,bufferType,buffer,bufferSize,entry,args);

    // 頂点シェーダを作成します。
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    switch( type )
    {
    case TB_SHADER_TYPE_VERTEX:
        {
            LPDIRECT3DVERTEXSHADER9 vertexShader;
            HRESULT hr = device.GetDepend().GetDeviceDX9()->CreateVertexShader( (DWORD*)shaderCode->GetBufferPointer(), 
                                                                              &vertexShader );
            if( FAILED(hr) ){
                return TB_SHADER_ID_INVALID;
            }
            return m_Depend.addVertexShader( vertexShader );
        }
        break;
    case TB_SHADER_TYPE_PIXEL: // 頂点シェーダ
        {
            LPDIRECT3DPIXELSHADER9 pixelShader;
            HRESULT hr = device.GetDepend().GetDeviceDX9()->CreatePixelShader( (DWORD*)shaderCode->GetBufferPointer(),
                                                                        &pixelShader );
            TB_SAFE_RELEASE( shaderCode );
            if( FAILED(hr) ){
                return TB_SHADER_ID_INVALID;
            }
            return m_Depend.addPixelShader( pixelShader );
        }
        break;
    }
    TB_SAFE_RELEASE( shaderCode );
    return TB_SHADER_ID_INVALID;
}

/*!
 * シェーダプログラム生成(ファイルから)(完了復帰)
 * @param type シェーダタイプ
 * @param bufferType 引き渡すバッファの種類
 * @param filePath ファイル名
 * @param entry エントリー名
 * @param args 引数
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderId TbShaderManager::CreateShaderFromFile( TbShaderType type ,
                                                  TbShaderBufferType bufferType , 
                                                  const char* filePath ,
                                                  const char* entry ,
                                                  const char** args )
{
    TbShaderId result = TB_SHADER_ID_INVALID;
    TbFile file( filePath , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_PRESET_SHADER );
    file.Open();
    file.Read();
    result = CreateShader( type , bufferType , reinterpret_cast<const TbChar8*>(file.GetReadBuffer()) , file.GetReadBufferSize() , entry , args );
    return result;
}

/*!
 * シェーダリロード) 
 * @author teco
 */
void TbShaderManager::ReloadShader( TbShaderId id,
                                    TbShaderType type,
                                    TbShaderBufferType bufferType,
                                    const TbChar8* buffer,
                                    const TbUint32 bufferSize,
                                    const TbChar8* entry ,
                                    const TbChar8** args )
{
    ID3DXBuffer* shaderCode = compileShader(type, bufferType, buffer, bufferSize, entry, args);
    // 頂点シェーダを作成します。
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    switch (type)
    {
    case TB_SHADER_TYPE_VERTEX:
    {
        LPDIRECT3DVERTEXSHADER9 vertexShader;
        HRESULT hr = device.GetDepend().GetDeviceDX9()->CreateVertexShader((DWORD*)shaderCode->GetBufferPointer(),
            &vertexShader);
        if (FAILED(hr)) {
            return;
        }
        TbShaderManagerDepend::VertexShader* shader= m_Depend.getVertexShader(id);
        TB_SAFE_RELEASE(shader->shader);
        shader->shader = vertexShader;
    }
    break;
    case TB_SHADER_TYPE_PIXEL: // 頂点シェーダ
    {
        LPDIRECT3DPIXELSHADER9 pixelShader;
        HRESULT hr = device.GetDepend().GetDeviceDX9()->CreatePixelShader((DWORD*)shaderCode->GetBufferPointer(),
            &pixelShader);
        TB_SAFE_RELEASE(shaderCode);
        if (FAILED(hr)) {
            return;
        }
        TbShaderManagerDepend::PixelShader* shader = m_Depend.m_PixelShaderPool.GetAt(id);
        TB_SAFE_RELEASE(*shader);
        *shader = pixelShader;
    }
    break;
    }
    TB_SAFE_RELEASE(shaderCode);
}

/*!
 * シェーダリロード(ファイルから)(完了復帰)
 * @param type シェーダタイプ
 * @param bufferType 引き渡すバッファの種類
 * @param filePath ファイル名
 * @param entry エントリー名
 * @param args 引数
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
void TbShaderManager::ReloadShaderFromFile(TbShaderId id,
                                           TbShaderType type,
                                           TbShaderBufferType bufferType,
                                           const TbChar8* filepath,
                                           const TbChar8* entry ,
                                           const TbChar8** args )
{
    TbFile file(filepath, TB_FILE_OPERATE_READ, TB_FILE_SYNC, TB_FILE_PATH_ID_PRESET_SHADER);
    file.Open();
    file.Read();
    ReloadShader(id,type, bufferType, reinterpret_cast<const TbChar8*>(file.GetReadBuffer()), file.GetReadBufferSize(), entry, args);
}

/*!
 * 登録されているシェーダ全破棄
 */
void TbShaderManager::DeleteShaderAll()
{
    m_Depend.deleteShaderAll( TB_TRUE );
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

    TbRenderDevice& device = TbRenderDevice::GetInstance();

    // 頂点シェーダ
    {
        LPDIRECT3DVERTEXSHADER9 vertexShader = nullptr;
        const TbVertexFormat* format = nullptr;
        if( !program ){
#if TB_IS_ENABLE_FIXED_SHADER
            format = device.GetFixedShader().GetVertexFormat();
#endif
        }else{
            TbShaderManagerDepend::VertexShader* shader = m_Depend.getVertexShader( program->GetCurrentShader(TB_SHADER_TYPE_VERTEX) );
            if( TB_VERIFY(shader) ){
                vertexShader = shader->shader;
                format = shader->format;
            }else{
                return TB_E_FAIL;
            }
        }
        HRESULT hr = device.GetDepend().GetDeviceDX9()->SetVertexShader( vertexShader );
        if( device.GetDepend().GetCurrentVertexFormat() != format ){
            device.GetDepend().SetCurrentVertexFormat( format );
        }
        if(!TB_VERIFY(SUCCEEDED(hr))){
            TB_E_FAIL;
        }
    }

    // ピクセルシェーダ
    {
        TbShaderManagerDepend::PixelShader pixelShader = nullptr;
        if(program){
            pixelShader = m_Depend.getPixelShader( program->GetCurrentShader(TB_SHADER_TYPE_PIXEL ));
        }
        HRESULT hr = device.GetDepend().GetDeviceDX9()->SetPixelShader( pixelShader);
        if(!TB_VERIFY(SUCCEEDED(hr))){
            return TB_E_FAIL;
        }
    }

    m_ShaderProgram = program;
    return TB_S_OK;
}

/*!
 * 頂点フォーマットを頂点シェーダにバインド
 * @author Miyake Shunsuke
 */
TbBool TbShaderManager::BindVertexFormat( const TbVertexFormat* format , const TbShaderId vertexShaderId )
{
    TbShaderManagerDepend::VertexShader* shader = m_Depend.getVertexShader( vertexShaderId );
    if( shader ){
        shader->format = format;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 利用可能な定数数取得
 * @author Miyake Shunsuke
 */
TbUint32 TbShaderManager::GetConstantCount() const
{
    return m_Depend.CONST_HANDLE_MAX;
}

/*!
 * 頂点シェーダBool定数設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantBool( TbUint32 shaderBit , TbShaderConstHandle& handle,const TbBool* data,TbUint32 count)
{
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
        device->SetVertexShaderConstantB(handle.m_Depend.reg,data,count);
    }
    if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
        device->SetPixelShaderConstantB(handle.m_Depend.reg,data,count);
    }
}

/*!
 * 頂点シェーダFloat定数設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantF32( TbUint32 shaderBit , TbShaderConstHandle& handle,const TbFloat32* data,TbUint32 count)
{
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
        device->SetVertexShaderConstantF(handle.m_Depend.reg,data,count);
    }
    if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
        device->SetPixelShaderConstantF(handle.m_Depend.reg,data,count);
    }
}

/*!
 * 頂点シェーダInt定数設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantS32(  TbUint32 shaderBit , TbShaderConstHandle& handle,const TbSint32* data,TbUint32 count)
{
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
        device->SetVertexShaderConstantI(handle.m_Depend.reg,reinterpret_cast<const int*>(data),count);
    }
    if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
        device->SetPixelShaderConstantI(handle.m_Depend.reg,reinterpret_cast<const int*>(data),count);
    }
}

/*!
 * シェーダ行列型定数設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle,const TbMatrix* data,TbUint32 count)
{
    HRESULT hr = S_OK;
    count = count*sizeof(*data)/sizeof(TbFloat32);
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
        hr = device->SetVertexShaderConstantF(handle.m_Depend.reg,reinterpret_cast<const TbFloat32*>(data),count);
        TB_ASSERT(SUCCEEDED(hr));
    }
    if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
        hr = device->SetPixelShaderConstantF(handle.m_Depend.reg,reinterpret_cast<const TbFloat32*>(data),count);
        TB_ASSERT(SUCCEEDED(hr));
    }
}

/*!
 * シェーダ行列型定数設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle,const TbMatrix43* data,TbUint32 count)
{
    HRESULT hr = S_OK;
    count = count*sizeof(*data)/sizeof(TbFloat32);
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if( shaderBit & TB_SHADER_TYPE_BIT_VERTEX ){
        hr = device->SetVertexShaderConstantF(handle.m_Depend.reg,reinterpret_cast<const TbFloat32*>(data),count);
        TB_ASSERT(SUCCEEDED(hr));
    }
    if( shaderBit & TB_SHADER_TYPE_BIT_PIXEL ){
        hr = device->SetPixelShaderConstantF(handle.m_Depend.reg,reinterpret_cast<const TbFloat32*>(data),count);
        TB_ASSERT(SUCCEEDED(hr));
    }
}

/*!
 * 定数バッファ設定
 * @param reg レジスタ番号
 * @param data 値
 * @param count 値の数
 * @author Miyake Shunsuke
 */
void TbShaderManager::SetConstantBuffer(TbUint32 shaderBit, TbShaderConstHandle& handle, TbConstantBuffer* buffer)
{
    TB_RESQUE(buffer);
    HRESULT hr = S_OK;
    TbSint32 size =  buffer->GetSize();
    TbSint32 count = size / sizeof(TbFloat32);
    TbFloat32* data = reinterpret_cast<TbFloat32*>(buffer->GetDepend().GetBuffer());
    LPDIRECT3DDEVICE9 device = TbRenderDevice::GetInstance().GetDepend().GetDeviceDX9();
    if (shaderBit & TB_SHADER_TYPE_BIT_VERTEX) {
        hr = device->SetVertexShaderConstantF(handle.m_Depend.reg, data, count);
        TB_ASSERT(SUCCEEDED(hr));
    }
    if (shaderBit & TB_SHADER_TYPE_BIT_PIXEL) {
        hr = device->SetPixelShaderConstantF(handle.m_Depend.reg, data, count);
        TB_ASSERT(SUCCEEDED(hr));
    }
}

/*!
 * world view projection行列更新
 * @author Miyake Shunsuke
 */
void TbShaderManager::updateWVP()
{
    if( TB_SHADER_CONST_HANDLE_INVALID == m_ConstHandle[CONST_HANDLE_WVP] ){
        m_ConstHandle[CONST_HANDLE_WVP].Set(TB_SHADER_CONST_INDEX_WVP);
    }
    TbMatrix mtx = m_Matrix[MATRIX_PROJECTION] * m_Matrix[MATRIX_VIEW] * m_Matrix[MATRIX_WORLD];
    mtx.Transpose();
    SetConstantMatrix(TB_SHADER_TYPE_BIT_VERTEX,m_ConstHandle[CONST_HANDLE_WVP],&mtx,1);
}

}

#endif
