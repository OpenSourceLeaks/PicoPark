/*!
 * シェーダー管理クラス( GLSL )
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "base/util/tb_depend_cast.h"

#if TB_SHADER_API_IS_GLSL

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

// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbShaderProgramDepend::TbShaderProgramDepend()
    : m_BitArray()
    , m_Program()
{
    m_Program = glCreateProgram();
}
 
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbShaderProgramDepend::~TbShaderProgramDepend()
{
    glDeleteProgram(m_Program);
}

/*!
 * 定数ハンドル更新
 * @author Miyake_Shunsuke
 */
void TbShaderProgramDepend::UpdateConstantHandle( TbShaderConstHandleDepend& handle )
{
    if( handle.handle == TB_SHADER_GLSL_CONST_HANDLE_INVALID ){
        if( handle.name[0] != '\0' && m_Program ){
            handle.handle = glGetUniformLocation( m_Program , handle.name );
        }
    }
}

/*!
 * リンクエラーチェック
 * @author Miyake Shunsuke
 */
TbResult TbShaderProgramDepend::link()
{
    glLinkProgram( m_Program );
    
    GLint logSize;
    GLsizei length;
    GLint isLinked = 0;
    m_BitArray.SetBit(FLAG_LINK,TB_FALSE);

    glGetProgramiv( m_Program , GL_LINK_STATUS , &isLinked );
    if( !isLinked )
    {
        glGetProgramiv( m_Program , GL_INFO_LOG_LENGTH , &logSize );
        if( logSize > 0 )
        {
             const TbUint32 LOG_BUFFER_SIZE = 512;
            TbChar8 logBuffer[ LOG_BUFFER_SIZE ];
            glGetProgramInfoLog( m_Program , LOG_BUFFER_SIZE , &length , logBuffer );
            TbPrintf( "[TbShaderProgram::isLinkError]program=%d\n" , m_Program );
            TbPrintf( "[TbShaderProgram::isLinkError]%s\n" , logBuffer );
            return TB_E_FAIL;
        }
    }

    m_BitArray.SetBit(FLAG_LINK);
    return TB_S_OK;
}

//

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbShaderProgram::TbShaderProgram()
    : m_BitArray()
    , m_Depend()
    , m_VertexFormat(nullptr)
{
    TbFill(m_UpdateShaderId,TB_SHADER_ID_INVALID);
    TbFill(m_CurrentShaderId,TB_SHADER_ID_INVALID);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbShaderProgram::~TbShaderProgram()
{
}

/*!
 * シェーダ更新
 * @author Miyake Shunsuke
 */
TbResult TbShaderProgram::Update()
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    TbShaderId vertexShaderId = m_UpdateShaderId[TB_SHADER_TYPE_VERTEX];
    TbShaderId pixelShaderId = m_UpdateShaderId[TB_SHADER_TYPE_PIXEL];
    TbShaderId currnetVertexShaderId = m_CurrentShaderId[TB_SHADER_TYPE_VERTEX];
    TbShaderId currentPixelShaderId = m_CurrentShaderId[TB_SHADER_TYPE_PIXEL];
    TbBool isEnableLink = TB_FALSE;
    if( m_BitArray.Test(FLAG_ENABLE_UPDATE) )
    {
        m_BitArray.SetBit( FLAG_ENABLE_UPDATE , TB_FALSE );

        if( vertexShaderId == currnetVertexShaderId && pixelShaderId == currentPixelShaderId ) {
            return TB_S_OK;
        }

        TbShaderManagerDepend::VertexShader* vertexShader = shaderManager.GetDepend().GetVertexShader( vertexShaderId );
        TbShaderManagerDepend::PixelShader* pixelShader = shaderManager.GetDepend().GetPixelShader( pixelShaderId );
        TbShaderManagerDepend::VertexShader* currentVertexShader = shaderManager.GetDepend().GetVertexShader( currnetVertexShaderId );
        TbShaderManagerDepend::PixelShader* currentPixelShader = shaderManager.GetDepend().GetPixelShader( currentPixelShaderId );

        GLuint program = m_Depend.GetProgram();
        if( currentVertexShader ){
            glDetachShader( program , currentVertexShader->shader );
        }
        if( currentPixelShader ){
            glDetachShader( program , *currentPixelShader );
        }
        TbFill(m_CurrentShaderId,TB_SHADER_ID_INVALID); // 現在の設定が無効化

        if( vertexShader ){
            glAttachShader( program , vertexShader->shader );
        }
        if( pixelShader ){
            glAttachShader( program , *pixelShader );
        }

        TbFill(m_UpdateShaderId,TB_SHADER_ID_INVALID);
        m_CurrentShaderId[TB_SHADER_TYPE_VERTEX] = vertexShaderId;
        m_CurrentShaderId[TB_SHADER_TYPE_PIXEL] = pixelShaderId;
        isEnableLink = TB_TRUE;
    }

    TbShaderManagerDepend::VertexShader* currentVertexShader = shaderManager.GetDepend().GetVertexShader( m_CurrentShaderId[TB_SHADER_TYPE_VERTEX] );
    if( currentVertexShader )
    {
        const TbVertexFormat* format = currentVertexShader->format;
        if( format != m_VertexFormat ){
            // 頂点シェーダにバインドされているフォーマットには、様々な要素がある( 頂点座標、頂点カラー、テクスチャ座標
            // それらとシェーダ内のAttribute変数とを関連付ける必要がある。
            const TbVertexElementGL* elements = format->GetDepend().GetElements();
            TbSint32 elementCount = format->GetDepend().GetElementCount();
            for( TbSint32 i = 0; i < elementCount; ++i ){
                const TbChar8* usageName = shaderManager.GetElementUsageVarName( static_cast<TbVertexElement::Usage>(elements[i].usage));
                TbStaticString128 name = TbStaticString128::Format( "%s%d" , usageName , elements[i].usageIndex );
                TbCheckErrorGL();
                glBindAttribLocation( m_Depend.GetProgram() , i , name.GetBuffer() );
                TbCheckErrorGL();
            }
            m_VertexFormat = format;
            isEnableLink = TB_TRUE;
        }
    }

    if( isEnableLink ){
        if( TB_FAILED(m_Depend.link()) ){
            return TB_E_FAIL;
        }
    }

    return TB_S_OK;
}

/*!
 * 有効なプログラムか
 * @author Miyake Shunsuke
 */
TbBool TbShaderProgram::IsValid() const
{
    return m_Depend.IsLinked();
}

}

#endif