/*!
 * 深度ステンシルステート
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_depth_stencil_state.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if (TB_3DAPI_IS_GFX)

namespace toybox
{

/*!
 * 初期化
 * @author teco
 */
void TbDepthStencilStateDepend::Initialize(const TbDepthStencilStateParam& param)
{
    TB_RESQUE(!m_IsInitialized);

    nn::gfx::DepthStencilState::InfoType info;
    info.SetDefault();

    // depth
    info.SetDepthTestEnabled(param.isEnableDepthTest);    
    nn::gfx::ComparisonFunction function;
    if (TbCmpFuncToGfx(&function,param.depthTestFunc)) {
        info.SetDepthComparisonFunction(function);
    }
    info.SetDepthWriteEnabled(TB_DEPTH_MASK_ALL == param.depthWriteMask);
    // info.SetDepthBoundsTestEnabledって何？

    // stencil
    info.SetStencilTestEnabled(param.isEnableStencilTest);
    info.SetStencilWriteMask(param.stencilWriteMask);
    info.SetStencilReadMask(param.stencilReadMask);

#if 0 // gfxはサポートできていない？
    nn::gfx::StencilStateInfo stencilStateInfo;
    param.stencilOp;
    param.stencilTestFunc;
    param.stencilTestRef;
#endif

    nn::gfx::Device* device = TbGetGfxDevice();
    m_State.Initialize(device, info);
    m_IsInitialized = TB_TRUE;
}

/*!
 * 後始末
 * @author teco
 */
void TbDepthStencilStateDepend::Finalize()
{
    if (m_IsInitialized) {
        nn::gfx::Device* device = TbGetGfxDevice();
        m_State.Finalize(device);
        m_IsInitialized = TB_FALSE;
    }
}

}

#endif
