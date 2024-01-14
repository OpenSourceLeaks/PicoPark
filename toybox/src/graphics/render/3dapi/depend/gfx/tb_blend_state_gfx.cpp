/*!
 * ブレンドステート
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_blend_state.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if (TB_3DAPI_IS_GFX)

namespace toybox
{

/*!
 * 初期化
 * @author teco
 */
void TbBlendStateDepend::Initialize(const TbBlendStateParam& param)
{
    TB_RESQUE(!m_IsInitialized);
    static const TbBlendFactor COLOR2ALPHA_TABLE[] =
    {
        TB_BLEND_FACTOR_ZERO ,
        TB_BLEND_FACTOR_ONE ,
        TB_BLEND_FACTOR_SRC_ALPHA ,
        TB_BLEND_FACTOR_INV_SRC_ALPHA ,
        TB_BLEND_FACTOR_DEST_ALPHA ,
        TB_BLEND_FACTOR_INV_DEST_ALPHA ,
        TB_BLEND_FACTOR_SRC_ALPHA ,
        TB_BLEND_FACTOR_INV_SRC_ALPHA ,
        TB_BLEND_FACTOR_DEST_ALPHA ,
        TB_BLEND_FACTOR_INV_DEST_ALPHA ,
    };

    nn::gfx::BlendTargetStateInfo renderTargetBlendInfo;
    renderTargetBlendInfo.SetDefault();
    {
        int mask = 0;
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_ALPHA) {
            mask |= nn::gfx::ChannelMask_Alpha;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_RED) {
            mask |= nn::gfx::ChannelMask_Red;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_GREEN) {
            mask |= nn::gfx::ChannelMask_Green;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_BLUE) {
            mask |= nn::gfx::ChannelMask_Blue;
        }
        renderTargetBlendInfo.SetChannelMask(mask);
    }
    renderTargetBlendInfo.SetBlendEnabled(param.isEnableBlend);
    nn::gfx::BlendFunction function;
    if (TbBlendOpToGfx(&function,param.blendOp)) {
        renderTargetBlendInfo.SetAlphaBlendFunction(function);
        renderTargetBlendInfo.SetColorBlendFunction(function);
    }
    nn::gfx::BlendFactor factor;
    if (TbBlendFactorToGfx(&factor, param.blendSrcFactor)) {
        renderTargetBlendInfo.SetSourceColorBlendFactor(factor);
    }
    if (TbBlendFactorToGfx(&factor, param.blendDstFactor)) {
        renderTargetBlendInfo.SetDestinationColorBlendFactor(factor);
    }
    if (TbBlendFactorToGfx(&factor, COLOR2ALPHA_TABLE[param.blendSrcFactor])) {
        renderTargetBlendInfo.SetSourceAlphaBlendFactor(factor);
    }
    if (TbBlendFactorToGfx(&factor, COLOR2ALPHA_TABLE[param.blendDstFactor])) {
        renderTargetBlendInfo.SetDestinationAlphaBlendFactor(factor);
    }

    nn::gfx::BlendState::InfoType info;
    info.SetDefault();
    info.SetBlendTargetStateInfoArray(&renderTargetBlendInfo, 1);

    nn::gfx::Device* device = TbGetGfxDevice();

    size_t size = nn::gfx::BlendState::GetRequiredMemorySize(info);
    size_t align = nn::gfx::BlendState::RequiredMemoryInfo_Alignment;
    m_Memory = new TbUint8[size+align];
    void* alignedMemory = nn::util::BytePtr(m_Memory).AlignUp(align).Get();
    m_State.SetMemory(alignedMemory, size);
    m_State.Initialize(device, info);
    m_IsInitialized = TB_TRUE;
}

/*!
 * 後始末
 * @author teco
 */
void TbBlendStateDepend::Finalize()
{
    if (m_IsInitialized) {
        nn::gfx::Device* device = TbGetGfxDevice();
        m_State.Finalize(device);
        TB_SAFE_DELETE_ARRAY(m_Memory);
        m_IsInitialized = TB_FALSE;
    }
}

}

#endif
