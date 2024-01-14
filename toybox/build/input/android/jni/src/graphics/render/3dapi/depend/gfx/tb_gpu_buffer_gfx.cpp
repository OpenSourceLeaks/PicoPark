/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/depend/gfx/tb_gpu_buffer_gfx.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"

#if TB_3DAPI_IS_GFX

namespace toybox
{

/*!
 * コンストラクタ
 */
TbGpuBufferGfx::TbGpuBufferGfx()
    :  m_Size(0)
    , m_Align(0)
    , m_MappedPtr(nullptr)
{
}

/*!
 * デストラクタ
 */
TbGpuBufferGfx::~TbGpuBufferGfx()
{
    Finalize();    
}

/*!
 * 初期化
 */
void TbGpuBufferGfx::Intialize( const InitParam& param )
{
    if (m_MemoryPool.IsInitialize()) {
        TB_ASSERT(0);
        return;
    }
    m_InitParam = param;
    m_Size = param.bufferSize;

    nn::gfx::Device* device = TbGetGfxDevice();
    TB_ASSERT(device);

    nn::gfx::Buffer::InfoType bufferInfo;
    bufferInfo.SetDefault();
    bufferInfo.SetGpuAccessFlags(param.accessFlag);
    bufferInfo.SetSize(m_Size);

    // メモリープールセットアップ
    if (NN_STATIC_CONDITION(nn::gfx::Buffer::IsMemoryPoolRequired))
    {
        int property = 0;
        if (param.cpuAccesible) {
            property = nn::gfx::MemoryPoolProperty_CpuUncached | nn::gfx::MemoryPoolProperty_GpuCached;
        }
        else {
            property = nn::gfx::MemoryPoolProperty_CpuInvisible |
                nn::gfx::MemoryPoolProperty_GpuCached |
                nn::gfx::MemoryPoolProperty_Compressible;
        }
        m_Align = nn::gfx::Buffer::GetBufferAlignment(device, bufferInfo);
        m_MemoryPool.Initialize(property,m_Size,m_Align);
        m_Buffer.Initialize(device, bufferInfo, m_MemoryPool.Get(), 0, m_Size);
    }
    else
    {
        m_Buffer.Initialize(device, bufferInfo, nullptr, 0, 0);
    }
}

/*!
 * 後始末
 */
void TbGpuBufferGfx::Finalize()
{
    if (!m_MemoryPool.IsInitialize()) {
        return;
    }
    nn::gfx::Device* device = TbGetGfxDevice();
    TB_ASSERT(device);
    Unmap();
    m_Buffer.Finalize(device);
    m_MemoryPool.Finalize();
}

/*!
 * マップ
 */
void* TbGpuBufferGfx::Map()
{
    TB_ASSERT(m_InitParam.cpuAccesible);
    if (!m_MappedPtr) {
        m_MappedPtr = m_Buffer.Map();
    }
    return m_MappedPtr;
}

/*!
 * アンマップ
 */
void TbGpuBufferGfx::Unmap()
{
    TB_ASSERT(m_InitParam.cpuAccesible);
    if (m_MappedPtr) {
        m_Buffer.FlushMappedRange(0, m_Size);
        m_Buffer.Unmap();
        m_MappedPtr = nullptr;
    }
}

/*!
 * 読み込み
 */
TbResult TbGpuBufferGfx::Read(void* readPtr, TbSizeT offset, TbSizeT size)
{
    TbBool isEnableUnmap = !m_MappedPtr;
    Map();
    TbUint8* src = reinterpret_cast<TbUint8*>(m_MappedPtr);
    TbMemCopy(readPtr, &src[offset], size);
    if (isEnableUnmap) {
        Unmap();
    }
    return TB_S_OK;
}

/*!
 * 書き込み
 */
TbResult TbGpuBufferGfx::Write(const void* writePtr, TbUint32 offset, TbSizeT size)
{
    TbBool isEnableUnmap = !m_MappedPtr;
    Map();
    TbUint8* dst = reinterpret_cast<TbUint8*>(m_MappedPtr);
    TbMemCopy(&dst[offset], writePtr, size);
    if (isEnableUnmap) {
        Unmap();
    }
    return TB_S_OK;
}

}

#endif