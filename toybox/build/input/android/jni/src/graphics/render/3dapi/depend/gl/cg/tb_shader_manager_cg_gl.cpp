/*!
 * シェーダー管理クラス( Cg )
 * DirectX10以降には未対応
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_manager.h"

#if ( TB_SHADER_API_TARGET ==TB_SHADER_CG ) && (TB_3DAPI_TARGET == TB_3DAPI_GL)

#include "../../cg/tb_shader_context_std_cg_inl.h" // 共通部分

#include <CG/cgGL.h>
#pragma comment(lib, "cgGL.lib")

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
    , m_Context( nullptr )
    , m_Device( device )
    , m_LatestVertexProfile( CG_PROFILE_UNKNOWN )
    , m_LatestPixelProfile( CG_PROFILE_UNKNOWN )
    , m_VertexShaderPool( vertexMax )
    , m_PixelShaderPool( pixelMax )
{
    m_Context = cgCreateContext();  // コンテキストの作成
    checkError( "[ShaderCg]CreateContext" );

    m_Device->GetDepend().SetCurrent( TB_TRUE );
    cgGLRegisterStates( m_Context ); // CgFx用にコンテキストを登録
    m_LatestVertexProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
    m_LatestPixelProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );
    cgGLSetOptimalOptions( m_LatestVertexProfile );
    cgGLSetOptimalOptions( m_LatestPixelProfile );
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
        TbVertexShader shader = nullptr;
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
}

/*!
 * シェーダプログラム生成(ファイルから)
 * @return 識別子
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
TB_INLINE CGprogram TbShaderManagerDepend::createShaderFromFile( CGprofile profile ,
                                                       CGenum programType , 
                                                       const char* filePath ,
                                                       const char* entry ,
                                                       const char** args )
{
    char const ** options = args;    
    CGprogram program = cgCreateProgramFromFile( m_Context ,
        programType ,
        filePath ,
        profile ,
        entry ,
        options );
    checkError( "create program" );
    // ロード( 多分、ここも適切じゃないんだろうなと思いつつ )
    cgGLLoadProgram( program );
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
    if( !m_BitArray.Test( FLAG_INIT ) )
    {
        m_Device->GetDepend().SetCurrent( TB_TRUE );
        cgGLRegisterStates( m_Context ); // cgGL用のコンテキストを登録
        m_LatestVertexProfile = cgGLGetLatestProfile( CG_GL_VERTEX );
        m_LatestPixelProfile = cgGLGetLatestProfile( CG_GL_FRAGMENT );
        cgGLSetOptimalOptions( m_LatestVertexProfile );
        cgGLSetOptimalOptions( m_LatestPixelProfile );
        m_BitArray.SetBit( FLAG_INIT , TB_TRUE );
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
    m_BitArray.SetBit( FLAG_VERTEX_SHADER_ACTIVE , shader != nullptr );
    if( shader ){
        cgGLBindProgram( shader->shader );
        cgGLEnableProfile( m_Depend.getLatestVertexProfile() );
        m_Depend.checkError( "bind vertex" );
        return TB_TRUE;
    }else{
        cgGLBindProgram( nullptr );
        cgGLDisableProfile( m_Depend.getLatestVertexProfile() );
        m_Depend.checkError( "bind vertex" );
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
    cgGLBindProgram( shader );
    m_BitArray.SetBit( FLAG_PIXEL_SHADER_ACTIVE , shader > 0 );
    if( shader ){
        cgGLEnableProfile( m_Depend.getLatestPixelProfile() );
    }else{
        cgGLDisableProfile( m_Depend.getLatestPixelProfile() );
    }
    m_Depend.checkError( "bind pixel" );
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
