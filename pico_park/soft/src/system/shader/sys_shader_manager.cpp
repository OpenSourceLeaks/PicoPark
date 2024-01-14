/*!
 * シェーダ管理クラス
 * @author teco
 */

#include "fwd.h"
#include "sys_shader_manager.h"
#include <graphics/render/3dapi/tb_render_device.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <base/io/tb_file_manager.h>

// シェーダファイルパス
static const char SHADER_ROOT_PATH[] = "/shader/" TB_SHADER_API_STR;

static const char* SHADER_TABLE[] =
{
    "color_change_2d"
};

/*!
 * コンストラクタ
 * @author teco
 */
SysShaderManager::SysShaderManager()
{
    TbFillNull(m_Program);
    TbFillNull(m_VertexFormat);
    TbFillNull(m_ConstantBuffer);
}

/*!
 * 初期化
 * @author teco
 */
void SysShaderManager::Initialize()
{
    TbShaderManager& shaderManager = TbRenderDevice::GetInstance().GetShaderManager();

    // シェーダルートパス設定
    {
        TbStaticString512 path = TbFileManager::GetInstance().GetRootPath(TB_FILE_PATH_ID_PRESET_ROOT);
        path.Append(SHADER_ROOT_PATH);
        TbFileManager::GetInstance().SetRootPath(TB_FILE_PATH_ID_PRESET_SHADER, path.GetBuffer());
    }

    
    m_VertexFormat[VERTEX_FORMAT_XYZ_COLOR_TEX0SIZE2] = TB_NEW TbVertexFormat(TbVertexFormat::TYPE_XYZ_COLOR_TEX0SIZE2);

    TbMatrix matrix;
    TbMatrixSetOrthoOffCenterLH(matrix,
        0.0f,
        static_cast<TbFloat32>(CMN_WINDOW_WIDTH),
        -static_cast<TbFloat32>(CMN_WINDOW_HEIGHT),
        0.0f, 0.1f, 10.0f);
    m_ConstantBuffer[SYS_CONSTANT_BUFFER_MATRIX] = TB_NEW TbConstantBuffer(sizeof(TbMatrix));
    m_ConstantBuffer[SYS_CONSTANT_BUFFER_MATRIX]->Write(0,matrix);

    for ( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Program); ++i  )
    {
        TbStaticString256 path = TbStaticString256::Format("%s.vs", SHADER_TABLE[i]);
        TbShaderId vertexShaderId = shaderManager.CreateShaderFromFile(TB_SHADER_TYPE_VERTEX,
            TB_SHADER_BUFFER_TYPE_SOURCE,
            path.GetBuffer(),
            "main",
            NULL);
        path = TbStaticString256::Format("%s.ps", SHADER_TABLE[i]);
        TbShaderId pixelShaderId = shaderManager.CreateShaderFromFile(TB_SHADER_TYPE_PIXEL,
            TB_SHADER_BUFFER_TYPE_SOURCE,
            path.GetBuffer(),
            "main",
            NULL);
        m_Program[i] = shaderManager.CreateProgram();
        m_Program[i]->SetShader(TB_SHADER_TYPE_VERTEX, vertexShaderId);
        m_Program[i]->SetShader(TB_SHADER_TYPE_PIXEL, pixelShaderId);
        m_Program[i]->Update();
        shaderManager.BindVertexFormat( m_VertexFormat[VERTEX_FORMAT_XYZ_COLOR_TEX0SIZE2] , vertexShaderId);
    }
}


/*!
 * 後始末
 * @author teco
 */
void SysShaderManager::Finalize()
{
    TbShaderManager& shaderManager = TbRenderDevice::GetInstance().GetShaderManager();
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Program); ++i)
    {
        if (m_Program[i]) {
            shaderManager.DeleteProgram(m_Program[i]);
            m_Program[i] = nullptr;
        }
    }
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Program); ++i)
    {
        TB_SAFE_DELETE(m_VertexFormat[i]);
    }
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ConstantBuffer); ++i)
    {
        TB_SAFE_DELETE(m_ConstantBuffer[i]);
    }
}

/*!
 * リロード
 * @author teco
 */
void SysShaderManager::Reload()
{
#if TB_PLATFORM_IS_WIN
    TbShaderManager& shaderManager = TbRenderDevice::GetInstance().GetShaderManager();
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Program); ++i)
    {
        TbStaticString256 path = TbStaticString256::Format("%s.vs", SHADER_TABLE[i]);
        shaderManager.ReloadShaderFromFile(m_Program[i]->GetCurrentShader(TB_SHADER_TYPE_VERTEX),
                                           TB_SHADER_TYPE_VERTEX,
                                           TB_SHADER_BUFFER_TYPE_SOURCE,
                                           path.GetBuffer(),
                                           "main",
                                           NULL);
        path = TbStaticString256::Format("%s.ps", SHADER_TABLE[i]);
        shaderManager.ReloadShaderFromFile(m_Program[i]->GetCurrentShader(TB_SHADER_TYPE_PIXEL),
                                           TB_SHADER_TYPE_PIXEL,
                                           TB_SHADER_BUFFER_TYPE_SOURCE,
                                           path.GetBuffer(),
                                           "main",
                                           NULL);
    }
#endif
}
