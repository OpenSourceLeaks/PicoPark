/*!
 * ビューポートシザーステート
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_viewport_scissor_state.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if (TB_3DAPI_IS_GFX)

namespace toybox
{

/*!
 * 初期化
 * @author teco
 */
void TbViewportScissorStateDepend::Initialize(const TbViewportScissorStateParam& param)
{
    TB_RESQUE(!m_IsInitialized);

    nn::gfx::ViewportScissorState::InfoType info;
    info.SetDefault();
    nn::gfx::ScissorStateInfo scissorInfo;
    scissorInfo.SetDefault();
    scissorInfo.SetOriginX(param.viewPortRect.x);
    scissorInfo.SetOriginX(param.viewPortRect.y);
    scissorInfo.SetWidth(param.viewPortRect.width);
    scissorInfo.SetHeight(param.viewPortRect.height);
    info.SetScissorStateInfoArray(&scissorInfo, 1);
    nn::gfx::ViewportStateInfo viewportInfo;
    viewportInfo.SetDefault();
    viewportInfo.SetOriginX(param.viewPortRect.x);
    viewportInfo.SetOriginX(param.viewPortRect.y);
    viewportInfo.SetWidth(param.viewPortRect.width);
    viewportInfo.SetHeight(param.viewPortRect.height);
    info.SetViewportStateInfoArray(&viewportInfo, 1);

    nn::gfx::Device* device = TbGetGfxDevice();
    m_State.Initialize(device, info);
    m_IsInitialized = TB_TRUE;
}

/*!
 * 後始末
 * @author teco
 */
void TbViewportScissorStateDepend::Finalize()
{
    if (m_IsInitialized) {
        nn::gfx::Device* device = TbGetGfxDevice();
        m_State.Finalize(device);
        m_IsInitialized = TB_FALSE;
    }
}

}

#endif
