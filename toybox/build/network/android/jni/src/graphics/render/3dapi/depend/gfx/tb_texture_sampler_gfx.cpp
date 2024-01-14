/*!
 * @file
 * @brief 
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture_sampler.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if (TB_3DAPI_IS_GFX)

namespace toybox
{
    
/*!
 * コンストラクタ
 * @author teco
 */
TbTextureSampler::TbTextureSampler()
    : m_IsInitialized(TB_FALSE)
    , m_Depend()
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbTextureSampler::~TbTextureSampler()
{
    Finalize();
}

/*!
 * 初期化
 * @author teco
 */
void TbTextureSampler::Initialize(const TbTextureSamplerParam& param)
{
    TB_ASSERT(!m_IsInitialized);
    nn::gfx::FilterMode gfxFilterMode;
    nn::gfx::TextureAddressMode gfxAddressU;
    nn::gfx::TextureAddressMode gfxAddressV;
    nn::gfx::Device* device = TbGetGfxDevice();
    if (TbTextureAddressModeToGfx(&gfxAddressU, param.addressModeU) &&
        TbTextureAddressModeToGfx(&gfxAddressV, param.addressModeV) &&
        TbTextureFilterModeToGfx(&gfxFilterMode, param.minFilterMode, param.magFilterMode))
    {
        nn::gfx::Sampler::InfoType info;
        info.SetDefault();
        info.SetFilterMode(gfxFilterMode);
        info.SetAddressU(gfxAddressU);
        info.SetAddressV(gfxAddressV);
        info.SetAddressW(gfxAddressU);
        m_Depend.m_Sampler.Initialize(device, info);
        m_Depend.m_DescriptorPool.InitializeAsSampler(&m_Depend.m_Sampler);
        m_IsInitialized = TB_TRUE;
    }
}

/*!
* 後始末
* @author teco
*/
void TbTextureSampler::Finalize()
{
    nn::gfx::Device* device = TbGetGfxDevice();
    if (m_IsInitialized) {
        m_Depend.m_DescriptorPool.Finalize();
        m_Depend.m_Sampler.Finalize(device);
        m_IsInitialized = TB_FALSE;
    }
}

}

#endif
