/*!
 * シェーダープログラムクラス( S3D )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_shader_program.h"
#include "graphics/3dapi/tb_shader_manager.h"

#if TB_SHADER_API_IS_AGAL

#include "graphics/3dapi/tb_render_device.h"
#include "graphics/3dapi/tb_render_target.h"
#include "graphics/3dapi/tb_vertex_buffer.h"
#include "base/io/tb_print.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderProgramDepend::TbShaderProgramDepend()
    : m_BitArray()
    , m_Program()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    m_Program = device.GetDepend().GetContext()->createProgram();
}
 
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderProgramDepend::~TbShaderProgramDepend()
{
}

//

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
 */
TbShaderProgram::TbShaderProgram()
    : m_BitArray()
    , m_Depend()
    , m_VertexFormat(NULL)
{
    TbFill(m_UpdateShaderId,TB_SHADER_ID_INVALID);
    TbFill(m_CurrentShaderId,TB_SHADER_ID_INVALID);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.07.10
 */
TbShaderProgram::~TbShaderProgram()
{
}

/*!
 * シェーダ更新
 * @author Miyake Shunsuke
 * @since 2010.08.10
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

        TbRenderDevice& device = TbRenderDevice::GetInstance();
        if( vertexShaderId == currnetVertexShaderId && pixelShaderId == currentPixelShaderId ) {
            return TB_S_OK;
        }

        TbShaderManagerDepend::VertexShader* vertexShader = shaderManager.GetDepend().GetVertexShader( vertexShaderId );
        TbShaderManagerDepend::PixelShader* pixelShader = shaderManager.GetDepend().GetPixelShader( pixelShaderId );

        TbFill(m_UpdateShaderId,TB_SHADER_ID_INVALID);
        m_CurrentShaderId[TB_SHADER_TYPE_VERTEX] = vertexShaderId;
        m_CurrentShaderId[TB_SHADER_TYPE_PIXEL] = pixelShaderId;
        
        AS3::ui::flash::utils::ByteArray vsByte;
        AS3::ui::flash::utils::ByteArray psByte;
        if( vertexShader ){
            vsByte = vertexShader->shader->agalcode;
        }
        if( pixelShader ){
            psByte = (*pixelShader)->agalcode;
        }
        m_Depend.GetProgram()->upload(vsByte,psByte);
    }

    return TB_S_OK;
}

/*!
 * 有効なプログラムか
 * @author Miyake Shunsuke
 * @since 2013.04.17
 */
TbBool TbShaderProgram::IsValid() const
{
    return m_Depend.IsLinked();
}

}

#endif