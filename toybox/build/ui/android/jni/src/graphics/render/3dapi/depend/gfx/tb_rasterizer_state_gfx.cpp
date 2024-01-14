/*!
 * ラスタライザステート
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_rasterizer_state.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if (TB_3DAPI_IS_GFX)

namespace toybox
{

/*!
 * 初期化
 * @author teco
 */
void TbRasterizerStateDepend::Initialize(const TbRasterizerStateParam& param)
{
    TB_RESQUE(!m_IsInitialized);

    nn::gfx::RasterizerState::InfoType info;
    info.SetDefault();
    nn::gfx::CullMode cullMode;
    if (TbCullModeToGfx(&cullMode,param.cullMode)) {
        info.SetCullMode(cullMode);
    }
    nn::gfx::FillMode fillMode;
    if (TbFillModeToGfx(&fillMode, param.fillMode)) {
        info.SetFillMode(fillMode);
    }
    if (param.isFrontFaceClockWise) {
        info.SetFrontFace(nn::gfx::FrontFace_Cw);
    }else {
        info.SetFrontFace(nn::gfx::FrontFace_Ccw);
    }

    nn::gfx::Device* device = TbGetGfxDevice();
    m_State.Initialize(device, info);
    m_IsInitialized = TB_TRUE;
}

/*!
 * 後始末
 * @author teco
 */
void TbRasterizerStateDepend::Finalize()
{
    if (m_IsInitialized) {
        nn::gfx::Device* device = TbGetGfxDevice();
        m_State.Finalize(device);
        m_IsInitialized = TB_FALSE;
    }
}

}

#endif
