/*!
 * @file
 * @brief テクスチャサンプラ(GX2)
 * @author teco
 */
 
#ifndef _INCLUDED_TB_TEXTURE_SAMPLER_GFX_H_
#define _INCLUDED_TB_TEXTURE_SAMPLER_GFX_H_

#include "./tb_descriptor_pool_gfx.h"

namespace toybox
{

/*!
 * @class 
 */
class TbTextureSamplerDepend
{
    friend class TbTextureSampler;
public:

    // コンストラクタ
    TbTextureSamplerDepend()
    {
    }

    // デストラクタ
    ~TbTextureSamplerDepend(){}
        
public:

    const nn::gfx::Sampler*   GetSampler() const {
        return &m_Sampler;
    }

    // DescriptorPool取得
    const nn::gfx::DescriptorPool* GetDescriptorPool() const
    {
        return m_DescriptorPool.GetDescriptorPool();
    }

    // DescriptorPool取得
    const nn::gfx::DescriptorSlot* GetDescriptorSlot() const
    {
        return m_DescriptorPool.GetDescriptorSlot();
    }

private:

    nn::gfx::Sampler     m_Sampler;
    TbDescriptorPoolGfx  m_DescriptorPool;

};

}

#endif
