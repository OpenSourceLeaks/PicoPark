/*!
 * @file
 * @brief 
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture_sampler.h"

#if (TB_3DAPI_IS_GX2)

namespace toybox
{
    
/*!
 * 更新
 * @author teco
 */
void TbTextureSampler::UpdateGX2Sampler()
{
    GX2TexClamp         clamp;
    GX2TexXYFilterType  filterType;;
    if( TbTextureAddressModeToGX2TexClamp(clamp,addressModeU) &&
        TbTextureFilterModeToGX2TexXYFilterType(filterType,minFilterMode) )
    {
        GX2InitSampler( &m_Sampler , &clamp , &filterType; );
    }
}

/*!
 * コンストラクタ
 * @author teco
 */
TbTextureSampler::TbTextureSampler()
    : m_Depend()
{
    m_Depend.UpdateGX2Sampler();
}

/*!
 * デストラクタ
 * @author teco
 */
TbTextureSampler::~TbTextureSampler()
{
}

/*!
 * 縮小フィルタ設定
 * @author teco
 */
void TbTextureSampler::SetMinFilterMode( TbTextureFilterMode filterMode )
{
    if( m_Depend.minFilterMode != mode )
    {
        m_Depend.minFilterMode = filterMode;
        m_Depend.magFilterMode = filterMode;
        m_Depend.UpdateGX2Sampler();
    }
}

/*!
 * 縮小フィルタ取得
 * @author teco
 */
TbTextureFilterMode TbTextureSampler::GetMinFilterMode() const
{
    return m_Depend.minFilterMode;
}

/*!
 * 拡大フィルタ設定
 * @author teco
 */
void TbTextureSampler::SetMagFilterMode( TbTextureFilterMode filterMode )
{
    if( m_Depend.magFilterMode != mode )
    {
        m_Depend.minFilterMode = filterMode;
        m_Depend.magFilterMode = filterMode;
        m_Depend.UpdateGX2Sampler();
    }
}

/*!
 * 拡大フィルタ取得
 * @author teco
 */
TbTextureFilterMode TbTextureSampler::GetMagFilterMode() const
{
    return m_Depend.magFilterMode;
}

/*!
 * アドレッシングモード設定(U座標)
 * @author teco
 */
void TbTextureSampler::SetAddressModeU( TbTextureAddressMode mode )
{
    if( m_Depend.addressModeU != mode )
    {
        m_Depend.addressModeU = mode;
        m_Depend.addressModeV = mode;
        m_Depend.UpdateGX2Sampler();
    }
}

/*!
 * アドレッシングモード設定(U座標)
 * @author teco
 */
TbTextureAddressMode TbTextureSampler::GetAddressModeU() const
{
    return m_Depend.addressModeU;
}

/*!
 * アドレッシングモード設定(V座標)
 * @author teco
 */
void TbTextureSampler::SetAddressModeV( TbTextureAddressMode mode )
{
    if( m_Depend.addressModeV != mode )
    {
        m_Depend.addressModeU = mode;
        m_Depend.addressModeV = mode;
        m_Depend.UpdateGX2Sampler();
    }
}

/*!
 * アドレッシングモード取得(V座標)
 * @author teco
 */
TbTextureAddressMode TbTextureSampler::GetAddressModeV() const
{
    return m_Depend.addressModeV;
}

}

#endif
