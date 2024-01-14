/*!
 * シェーダー管理クラス( Cg )
 * DirectX10以降には未対応
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_shader_manager.h"

#if ( TB_SHADER_API_TARGET ==TB_SHADER_CG ) && (TB_3DAPI_TARGET == TB_3DAPI_DX9)

#include "../../cg/tb_shader_context_std_cg_inl.h" // 共通部分

#include <CG/cgD3D9.h>
#pragma comment(lib, "cgD3D9.lib")

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
TbShaderManagerDepend::TbShaderManagerDepend( TbRenderDevice* device , TbUint32 vertexMax , TbUint32 pixelMax , TbUint32 effectMax )
    : m_BitArray()
    , m_ParameterSettingMode( PARAMETER_SETTING_MODE_IMMEDIATE )
    , m_Context( NULL )
    , m_Device( device )
    , m_LatestVertexProfile( CG_PROFILE_UNKNOWN )
    , m_LatestPixelProfile( CG_PROFILE_UNKNOWN )
    , m_VertexShaderPool( vertexMax )
    , m_PixelShaderPool( pixelMax )
{
    m_Context = cgCreateContext();  // コンテキストの作成
    checkError( "[ShaderCg]CreateContext" );
    m_LatestVertexProfile = cgD3D9GetLatestVertexProfile();
    m_LatestPixelProfile = cgD3D9GetLatestPixelProfile();
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
TbShaderManagerDepend::~TbShaderManagerDepend()
{
    {
        TbVertexShaderPool::Iterator ite = m_VertexShaderPool.Begin();
        TbVertexShaderPool::Iterator iteEnd = m_VertexShaderPool.End();
        TbVertexShader shader = NULL;
        while( ite != iteEnd ){
            shader = (*ite);
            cgDestroyProgram( shader->shader );
            TB_SAFE_DELETE( shader );
            ++ite;
        }
    }

    {
        TbPixelShaderPool::Iterator ite = m_PixelShaderPool.Begin();
        TbPixelShaderPool::Iterator iteEnd = m_PixelShaderPool.End();
        while( ite != iteEnd )
        {
            cgDestroyProgram( *ite );
            ++ite;
        }
    }

    cgDestroyContext( m_Context );
    cgD3D9SetDevice( NULL );
}

/*!
 * 頂点シェーダプログラム生成(ファイルから)
 * @return 識別子
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
CGprogram TbShaderManagerDepend::createShaderFromFile( CGprofile profile ,
                                                       CGenum programType , 
                                                       const char* filePath ,
                                                       const char* entry ,
                                                       const char** args )
{
    char const ** options = cgD3D9GetOptimalOptions(profile);
    CGprogram program = cgCreateProgramFromFile( m_Context ,
        programType ,
        filePath ,
        profile ,
        entry ,
        options );
    checkError( "create program" );
    // ロード( 多分、ここも適切じゃないんだろうなと思いつつ )
    cgD3D9LoadProgram( program , TB_FALSE , 0 ); // 暫定引数
    return program;
}


#if 0

/*!
 * デバイス設定
 * 一度コンパイルされたシェーダを再ロードする必要がある
 * @author Miyake Shunsuke
 * @since 2010.06.14
 */
void TbShaderManager::SetDevice( TbRenderDevice* device )
{
    m_Device = device;
    cgD3D9SetDevice( m_Device->GetDepend().GetDevice() );

    if( !m_BitArray.Test( FLAG_INIT ) )
    {
        m_LatestVertexProfile = cgD3D9GetLatestVertexProfile();
        m_LatestPixelProfile = cgD3D9GetLatestPixelProfile();
        m_BitArray.SetBit( FLAG_INIT , TB_TRUE );
    }

    TbPool<CGprogram,NULL>::Iterator ite = m_VertexShaderPool.Begin();
    TbPool<CGprogram,NULL>::Iterator iteEnd = m_VertexShaderPool.End();
    while( ite != iteEnd ){
        cgD3D9LoadProgram( *ite , TB_FALSE , 0 ); // 引数はとりあえず
        ++ite;
    }
    ite = m_PixelShaderPool.Begin();
    iteEnd = m_PixelShaderPool.End();
    while( ite != iteEnd ){
        cgD3D9LoadProgram( *ite , TB_FALSE , 0 );
        ++ite;
    }
}

#endif

//

/*!
 * 頂点シェーダバインド
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TbBool TbShaderManager::BindVertexShader( const TbSint32 shaderId )
{
    TbVertexShader shader = m_Depend.getVertexShader( shaderId );
    if( shader ){
        HRESULT hResult = cgD3D9BindProgram( shader->shader );
        m_Depend.checkError( "bind vertex" );
        m_BitArray.SetBit( FLAG_VERTEX_SHADER_ACTIVE , TB_TRUE );
        return TB_TRUE;
    }

    return TB_FALSE;
}

/*!
 * ピクセルシェーダバインド
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TbBool TbShaderManager::BindPixelShader(const TbSint32 shaderId)
{
    TbPixelShader shader = m_Depend.getPixelShader( shaderId );
    if( shader ){
        HRESULT hResult = cgD3D9BindProgram( shader );
        m_Depend.checkError( "bind pixel" );
        m_BitArray.SetBit( FLAG_PIXEL_SHADER_ACTIVE , TB_TRUE );
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 描画開始
 * @author Miyake Shunsuke
 * @since 2010.07.16
 */
void TbShaderManager::BeginScene()
{
}

}

#endif
