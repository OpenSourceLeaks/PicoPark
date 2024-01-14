/*!
 * @file
 * @brief シェーダ管理クラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_shader_program.h"

#if TB_SHADER_API_IS_HLSL

#include "graphics/render/3dapi/tb_render_device.h"
#include "base/math/tb_matrix.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.10
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
    if( m_BitArray.Test(FLAG_ENABLE_UPDATE) )
    {
        m_BitArray.SetBit( FLAG_ENABLE_UPDATE , TB_FALSE );
        TbShaderId vertexShaderId = m_UpdateShaderId[TB_SHADER_TYPE_VERTEX];
        TbShaderId pixelShaderId = m_UpdateShaderId[TB_SHADER_TYPE_PIXEL];
        TbFill(m_UpdateShaderId,TB_SHADER_ID_INVALID);
        m_CurrentShaderId[TB_SHADER_TYPE_VERTEX] = vertexShaderId;
        m_CurrentShaderId[TB_SHADER_TYPE_PIXEL] = pixelShaderId;
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
    return TB_TRUE;
}
}

#endif
