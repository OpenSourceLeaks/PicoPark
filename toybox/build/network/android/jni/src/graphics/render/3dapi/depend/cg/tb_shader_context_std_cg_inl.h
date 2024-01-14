/*!
 * Cg用シェーダー管理クラスのプラットフォーム別共通実装部分
 * DirectX10以降には未対応
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#if defined( TB_STD_SHADER_CG )

namespace toybox
{

/*!
 * 頂点シェーダプログラム生成(ファイルから)
 * @return 識別子
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TB_INLINE TbShaderId TbShaderManagerDepend::createVertexShaderFromFile( CGenum programType , 
                                                                        const char* filePath ,
                                                                        const char* entry ,
                                                                        const char** args )
{
    if( !m_VertexShaderPool.IsFull() )
    {
        CGprogram program = createShaderFromFile( m_LatestVertexProfile , programType , filePath , entry , args );
        if( program ){
            TbVertexShader vertexShader = TB_NEW _TbVertexShader();
            vertexShader->shader = program;
            return m_VertexShaderPool.Add( vertexShader );
        }
    }
    return TB_SHADER_ID_INVALID;
}

/*!
 * ピクセルシェーダプログラム生成(ファイルから)
 * @return 識別子
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TB_INLINE TbShaderId TbShaderManagerDepend::createPixelShaderFromFile( CGenum programType , 
                                                                       const char* filePath ,
                                                                       const char* entry ,
                                                                       const char** args )
{
    if( !m_PixelShaderPool.IsFull() )
    {
        CGprogram program = createShaderFromFile( m_LatestPixelProfile , programType , filePath , entry , args );
        if( program ){
            return m_PixelShaderPool.Add( program );
        }
    }
    return TB_SHADER_ID_INVALID;
}
    
/*!
 * エラーチェック
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TB_INLINE void TbShaderManagerDepend::checkError( const char* situation )
{
    CGerror error;
    const char *string = cgGetLastErrorString(&error);
    if (error != CG_NO_ERROR) {
        printf("%s : %s\n", situation, string );
        if (error == CG_COMPILER_ERROR) {
            printf("%s\n", cgGetLastListing(m_Context));
        }
    }
}

/*!
 * パラメータ設定モード設定
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
void TbShaderManagerDepend::SetParameterSettingMode( ParameterSettingMode mode )
{
    m_ParameterSettingMode = mode;
    cgSetParameterSettingMode( m_Context , static_cast<CGenum>(m_ParameterSettingMode) );
}

/*!
 * 頂点シェーダパラメータ更新(設定モードがDEFERREDの時は呼ばなければいけない
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
void TbShaderManagerDepend::UpdateVertexParameter( TbSint32 shaderId )
{
    TbVertexShader shader = m_VertexShaderPool.Get( shaderId );
    if( shader ){
        cgUpdateProgramParameters( shader->shader );
    }
}

/*!
 * ピクセルシェーダパラメータ更新(設定モードがDEFERREDの時は呼ばなければいけない
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
void TbShaderManagerDepend::UpdatePixelParameter( TbSint32 shaderId )
{
    TbPixelShader pixelShader = m_PixelShaderPool.Get( shaderId );
    if( pixelShader ){
        cgUpdateProgramParameters( pixelShader );
    }
}

//


/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderManager::TbShaderManager( TbRenderDevice* device , TbUint32 vertexMax , TbUint32 pixelMax )
    : m_BitArray()
    , m_Device( device )
    , m_Depend( device , vertexMax , pixelMax )
{
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
 * ファイルから頂点シェーダプログラム生成(完了復帰)
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderId TbShaderManager::CreateVertexShaderFromFile( TbShaderBufferType bufferType , 
                                                        const char* filepath ,
                                                        const char* entry ,
                                                        const char** args )
{
    return m_Depend.createVertexShaderFromFile( (bufferType == TB_SHADER_BUFFER_TYPE_SOURCE ) ? CG_SOURCE : CG_OBJECT ,
                                                filepath ,
                                                entry ,
                                                args );
}

/*!
 * ファイルからピクセルシェーダプログラム生成(完了復帰)
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderId TbShaderManager::CreatePixelShaderFromFile( TbShaderBufferType bufferType , 
                                                       const char* filepath ,
                                                       const char* entry ,
                                                       const char** args )
{
    return m_Depend.createPixelShaderFromFile(   (bufferType == TB_SHADER_BUFFER_TYPE_SOURCE ) ? CG_SOURCE : CG_OBJECT ,
                                                filepath ,
                                                entry ,
                                                args );
}

}

#endif
