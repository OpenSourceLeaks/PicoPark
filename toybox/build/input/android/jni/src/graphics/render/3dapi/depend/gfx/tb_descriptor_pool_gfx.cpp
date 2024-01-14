/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/depend/gfx/tb_descriptor_pool_gfx.h"
#include "graphics/render/3dapi/depend/gfx/tb_render_util_gfx.h"
#include "base/math/tb_math_util.h"

#if TB_3DAPI_IS_GFX

namespace
{
    TbSint32 getBaseIndex(NVNdeviceInfo param)
    {
        int result = 0;
#if NN_GFX_IS_TARGET_NVN
        nn::gfx::Device* device = toybox::TbGetGfxDevice();
        nn::gfx::Device::DataType& deviceData = nn::gfx::AccessorToData(*device);
        nvnDeviceGetInteger(deviceData.pNvnDevice, param, &result);
#endif
        return result;
    }
}

namespace toybox
{

// コンストラクタ
TbDescriptorPoolGfx::TbDescriptorPoolGfx()
    : m_Type(TYPE_NONE)
    , m_BaseIndex(0)
    , m_Slot()
{
}

// デストラクタ
TbDescriptorPoolGfx::~TbDescriptorPoolGfx()
{
    Finalize();
}

// テクスチャとして初期化
void TbDescriptorPoolGfx::InitializeAsTexture(nn::gfx::TextureView* tex)
{
    m_BaseIndex = getBaseIndex(NVN_DEVICE_INFO_RESERVED_TEXTURE_DESCRIPTORS);
    init(nn::gfx::DescriptorPoolType_TextureView, m_BaseIndex+2);
    m_Type = TYPE_TEXTURE;
    m_Param.texture =tex;
    Update();
    m_Slot.Offset(nn::gfx::DescriptorPool::GetDescriptorSlotIncrementSize(TbGetGfxDevice(), nn::gfx::DescriptorPoolType_TextureView));
}

// サンプラとして初期化
void TbDescriptorPoolGfx::InitializeAsSampler(nn::gfx::Sampler* sampler)
{
    m_BaseIndex = getBaseIndex(NVN_DEVICE_INFO_RESERVED_SAMPLER_DESCRIPTORS);
    init(nn::gfx::DescriptorPoolType_Sampler, m_BaseIndex+1);
    m_Type = TYPE_SAMPLER;
    m_Param.sampler = sampler;
    Update();
}

// コンスタントバッファとして初期化
void TbDescriptorPoolGfx::InitializeAsConstantBuffer(nn::gfx::GpuAddress buf,size_t bufSize)
{
    init(nn::gfx::DescriptorPoolType_BufferView, m_BaseIndex+1);
    m_Type = TYPE_CONSTANT_BUFFER;
    m_Param.constantBuffer = buf;
    m_Param.size = bufSize;
    Update();
}

// 更新
void TbDescriptorPoolGfx::Update()
{
    if (!m_MemoryPool.IsInitialize()) {
        return;
    }
    m_DescriptorPool.BeginUpdate();
    switch (m_Type)
    {
    case TYPE_TEXTURE:
        m_DescriptorPool.SetTextureView(m_BaseIndex + 1, m_Param.texture);
        break;
    case TYPE_SAMPLER:
        m_DescriptorPool.SetSampler(m_BaseIndex, m_Param.sampler);
        break;
    case TYPE_CONSTANT_BUFFER:
        m_DescriptorPool.SetBufferView(m_BaseIndex,m_Param.constantBuffer, m_Param.size);
        break;
    };
    m_DescriptorPool.EndUpdate();
    m_DescriptorPool.GetDescriptorSlot(&m_Slot, m_BaseIndex);
}

// 後始末
void TbDescriptorPoolGfx::Finalize()
{
    if (m_MemoryPool.IsInitialize()) {
        nn::gfx::Device* device = toybox::TbGetGfxDevice();
        m_DescriptorPool.Finalize(device);
        m_MemoryPool.Finalize();
    }
}

// 共通初期化
void TbDescriptorPoolGfx::init(nn::gfx::DescriptorPoolType type,TbSint32 slotCount)
{
    nn::gfx::Device* device = toybox::TbGetGfxDevice();
    nn::gfx::DescriptorPool::InfoType info;
    info.SetDefault();
    info.SetDescriptorPoolType(type);
    info.SetSlotCount(slotCount);
    size_t size = nn::gfx::DescriptorPool::CalculateDescriptorPoolSize(device, info);
    size_t align = nn::gfx::DescriptorPool::GetDescriptorPoolAlignment(device,info);
    m_MemoryPool.Initialize(nn::gfx::MemoryPoolProperty_CpuUncached|nn::gfx::MemoryPoolProperty_GpuCached,
                            size,
                            align);
    m_DescriptorPool.Initialize(device, info, m_MemoryPool.Get(), 0, size);
}

}

#endif