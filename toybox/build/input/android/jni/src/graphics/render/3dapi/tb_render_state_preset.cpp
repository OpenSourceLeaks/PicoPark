/*!
 * レンダーステートプリセット
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_state_preset.h"

namespace toybox
{

/*!
 * 初期化
 * @author teco
 */
void TbRenderStatePreset::Initialize()
{
    // ブレンド
    {
        TbBlendStateParam param;
        m_BlendState[BLEND_OPACITY].Initialize(param);
        param.isEnableBlend = TB_TRUE;
        param.blendSrcFactor = TB_BLEND_FACTOR_SRC_ALPHA;
        param.blendDstFactor = TB_BLEND_FACTOR_INV_SRC_ALPHA;
        m_BlendState[BLEND_NORMAL].Initialize(param);
    }

    // 深度ステンシル
    {
        TbDepthStencilStateParam param;
        param.isEnableDepthTest = TB_TRUE;
        m_DepthStencilState[DEPTH_STENCIL_3D_NORMAL].Initialize(param);
        param.isEnableDepthTest = TB_FALSE;
        param.depthWriteMask = TB_DEPTH_MASK_NONE;
        m_DepthStencilState[DEPTH_STENCIL_2D].Initialize(param);
    }

    // ラスタライザ
    {
        TbRasterizerStateParam param;
        param.cullMode = TB_CULL_MODE_NONE;
        m_RasterizerState[RASTERIZER_CULL_NONE].Initialize(param);
    }

}

}
