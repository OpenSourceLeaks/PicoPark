/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_MEMORY_POOL_GFX_H_
#define _INCLUDED_TB_MEMORY_POOL_GFX_H_

namespace toybox
{

class TbMemoryPoolGfx
{
public:

    // コンストラクタ
    TbMemoryPoolGfx()
        : m_Memory(nullptr)
    {
    }

    //デストラクタ
    ~TbMemoryPoolGfx()
    {
        Finalize();
    }

public:

    // 初期化
    void Initialize(int memoryPoolProperty, size_t size, size_t align)
    {
        nn::gfx::Device* device = TbGetGfxDevice();
        nn::gfx::MemoryPoolInfo memoryInfo;
        memoryInfo.SetDefault();
        memoryInfo.SetMemoryPoolProperty(memoryPoolProperty);

        size_t memoryAlignment = nn::gfx::MemoryPool::GetPoolMemoryAlignment(device, memoryInfo);
        size_t granularity = nn::gfx::MemoryPool::GetPoolMemorySizeGranularity(device, memoryInfo);
        align = TbMax(memoryAlignment, align);
        m_Memory = new TbUint8[size + align];
        void* memoryAligned = nn::util::BytePtr(m_Memory).AlignUp(memoryAlignment).Get();
        size_t memoryPoolSizeAligned = nn::util::align_down(size + align, granularity);
        memoryInfo.SetPoolMemory(memoryAligned, memoryPoolSizeAligned);
        m_MemoryPool.Initialize(device, memoryInfo);
    }

    // 終了
    void Finalize()
    {
        if( m_Memory )
        {
            nn::gfx::Device* device = TbGetGfxDevice();
            m_MemoryPool.Finalize(device);
            TB_SAFE_DELETE_ARRAY(m_Memory);
        }
    }

    // 初期化済みか
    TbBool IsInitialize() const {
        return m_Memory != nullptr;
    }

public:

    // nn::gfx::MemoryPool取得
    nn::gfx::MemoryPool* Get() {
        return &m_MemoryPool;
    }

    // nn::gfx::MemoryPool取得
    const nn::gfx::MemoryPool* Get() const {
        return &m_MemoryPool;
    }

private:

    TbUint8*            m_Memory;
    nn::gfx::MemoryPool m_MemoryPool;

};

}

#endif