/*!
 * @file
 * @brief GFXの定数バッファ依存コード
 */

#ifndef _INCLUDED_TB_CONSTANT_BUFFER_GFX_H_
#define _INCLUDED_TB_CONSTANT_BUFFER_GFX_H_

#include "tb_gpu_buffer_gfx.h"
#include "./tb_descriptor_pool_gfx.h"

#define TB_CONSTANT_BUFFER_ENABLE_DESCRIPTOR (0)

namespace toybox
{

class TbConstantBufferDepend
{
    friend class TbConstantBuffer;
public:
    // コンストラクタ
    TbConstantBufferDepend(const TbSint32 size);
    // デストラクタ
    ~TbConstantBufferDepend();
public:
    //! Buffer取得
    TbGpuBufferGfx*   GetBuffer() {
        return &m_Buffer;
    }
    // GPUアドレス取得
    TbResult GetGpuAddress(nn::gfx::GpuAddress* gpuAddress ) {
        m_Buffer.GetGpuAddress(gpuAddress);
        return TB_S_OK;
    }

#if TB_CONSTANT_BUFFER_ENABLE_DESCRIPTOR
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
#endif

private:
    TbGpuBufferGfx       m_Buffer;
    TbUint32             m_Offset;
#if TB_CONSTANT_BUFFER_ENABLE_DESCRIPTOR
    TbDescriptorPoolGfx  m_DescriptorPool;
#endif
};

}

#endif