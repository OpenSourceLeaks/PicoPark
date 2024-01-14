/*!
 * Player用マテリアル
 * @author teco
 */

#include "fwd.h"
#include "common/material/cmn_sprite_player_material.h"
#include <graphics/render/3dapi/tb_render_device.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_render_pass.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/math/tb_gr_matrix_util.h>
#include <framework/sub/tb_framework_simple.h>
#include "root.h"

 /*!
 * コンストラクタ
 * @author teco
 */
CmnSpritePlayerMaterial::CmnSpritePlayerMaterial()
    : m_PassIndex(TbFrameworkSimple::RENDER_PASS_2D)
    , m_Color(TbColor::White())
    , m_Texture(nullptr)
{
    m_Texture = Root::GetInstance().GetCommonTexture();
    m_MatrixHandle.Set(0);
}

/*!
* デストラクタ
* @author teco
*/
CmnSpritePlayerMaterial::~CmnSpritePlayerMaterial()
{
}
/*!
* 描画開始
* @author teco
*/
void CmnSpritePlayerMaterial::BeginDraw()
{
    TbDraw2dSetRenderPass(m_PassIndex);
    TbDraw2dTextureBegin();
    SysShaderManager& shaderManager = SysShaderManager::GetInstance();

    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(m_PassIndex);
    if (renderPass) {
        renderPass->SetShaderProgram(shaderManager.GetProgram(SYS_SHADER_PLAYER));
    }
    renderPass->SetShaderConstantBuffer(TB_SHADER_TYPE_BIT_VERTEX,&m_MatrixHandle,shaderManager.GetConstantBuffer(SYS_CONSTANT_BUFFER_MATRIX));
}

/*!
* 描画
* @author teco
*/
void CmnSpritePlayerMaterial::Draw(const TbRectF32& rect,
    const TbRectF32& uvRect,
    const TbColor&        color,
    const TbMatrix&       drawMatrix)
{
    // TbDraw系に依存した描画
    TbColor resultColor = m_Color * color;
    TbRectF32 resultRectUV = uvRect;
    TbBool isValidUV = !(TB_FLOAT32_EQUAL(uvRect.x, 0.0f) &&
        TB_FLOAT32_EQUAL(uvRect.y, 0.0f) &&
        TB_FLOAT32_EQUAL(uvRect.width, 0.0f) &&
        TB_FLOAT32_EQUAL(uvRect.height, 0.0f));

    TbDrawRectTextureParam param(m_Texture);
    param.pos.SetXY(rect.x, rect.y);
    param.size.width = rect.width;
    param.size.height = rect.height;
    param.matrix = drawMatrix;
    param.color = resultColor;
    param.rectUV = resultRectUV;
    TbDrawRectTexture(param);
}

/*!
* 描画終了
* @author teco
*/
void CmnSpritePlayerMaterial::EndDraw()
{
    TbDraw2dTextureEnd();
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(m_PassIndex);
    if (renderPass) {
        renderPass->SetShaderProgram(nullptr);
    }
}
