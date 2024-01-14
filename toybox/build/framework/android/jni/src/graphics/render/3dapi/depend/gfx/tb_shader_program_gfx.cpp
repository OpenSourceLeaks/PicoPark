/*!
 * シェーダー管理クラス( GFX )
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_program.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "base/util/tb_depend_cast.h"

#if TB_SHADER_API_IS_GFX

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"
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
s */
TbShaderProgramDepend::TbShaderProgramDepend()
    : m_BitArray()
    , m_VertexShaderId(TB_SHADER_ID_INVALID)
    , m_PixelShaderId(TB_SHADER_ID_INVALID)
    , m_Common(nullptr)
{
}
 
/*!
 * デストラクタ
 */
TbShaderProgramDepend::~TbShaderProgramDepend()
{
}

/*!
 * 定数ハンドル更新
 * @author Miyake_Shunsuke
 */
TbBool  TbShaderProgramDepend::UpdateConstantHandle(TbUint32 shaderBit, TbShaderConstHandleDepend& handle )
{
    TbShaderManager& shaderManager = TbShaderManager::GetInstance();
    for (TbUint32 i = TB_SHADER_TYPE_BEGIN; i < TB_SHADER_TYPE_END; ++i) {
        TbShaderType type = static_cast<TbShaderType>(i);
        if (!(shaderBit & TB_BIT(i))) {
            continue;
        }
        if (TbShaderConstHandleDepend::INVALID_HANDLE != handle.handle[i]) {
            continue;
        }
        nn::gfx::Shader* shader = shaderManager.GetDepend().GetShader(type,m_Common->GetCurrentShader(type));
        if (!shader) {
            return TB_FALSE;
        }
        nn::gfx::ShaderStage stage = TbShaderTypeToGfx(type);
        handle.handle[i] = shader->GetInterfaceSlot(stage,nn::gfx::ShaderInterfaceType_ConstantBuffer, handle.name);
        if (handle.handle[i] < 0) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
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
    m_Depend.SetCommon(this);
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
        TbShaderId vertexShaderId = m_UpdateShaderId[TB_SHADER_TYPE_VERTEX];
        TbShaderId pixelShaderId = m_UpdateShaderId[TB_SHADER_TYPE_PIXEL];
        TbFill(m_UpdateShaderId, TB_SHADER_ID_INVALID);
        m_CurrentShaderId[TB_SHADER_TYPE_VERTEX] = vertexShaderId;
        m_CurrentShaderId[TB_SHADER_TYPE_PIXEL] = pixelShaderId;
    }
    return TB_S_OK;
}

/*!
 * 有効なプログラムか
 * @author Miyake Shunsuke
 */
TbBool TbShaderProgram::IsValid() const
{
    return TB_TRUE;
}

}

#endif