/*!
 * @file
 * @brief DescriptorPool
 */

#ifndef _INCLUDED_TB_DESCRIPTR_POOL_GFX_H_
#define _INCLUDED_TB_DESCRIPTR_POOL_GFX_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/depend/gfx/tb_memory_pool_gfx.h>

namespace toybox
{

class TbDescriptorPoolGfx
{
public:

    // コンストラクタ
    TbDescriptorPoolGfx();

    // デストラクタ
    ~TbDescriptorPoolGfx();

public:

    // テクスチャとして初期化
    void InitializeAsTexture(nn::gfx::TextureView* tex);

    // サンプラとして初期化
    void InitializeAsSampler(nn::gfx::Sampler* sampler);

    // コンスタントバッファとして初期化
    void InitializeAsConstantBuffer(nn::gfx::GpuAddress buf, size_t bufSize);
    
    // 更新
    void Update();

public:

    // 後始末
    void Finalize();

public:

    // DescriptorPool取得
    const nn::gfx::DescriptorPool* GetDescriptorPool() const
    {
        return &m_DescriptorPool;
    }

    // DescriptorPool取得
    const nn::gfx::DescriptorSlot* GetDescriptorSlot() const
    {
        return &m_Slot;
    }

private:

    enum Type
    {
        TYPE_NONE ,
        TYPE_TEXTURE ,
        TYPE_SAMPLER , 
        TYPE_CONSTANT_BUFFER ,
        TYPE_MAX
    };

    struct Param
    {
        Param()
            : size(0)
            , texture(nullptr)
        {
        }
        TbSint32 size;
        nn::gfx::GpuAddress  constantBuffer;
        union
        {
            nn::gfx::TextureView*   texture;
            nn::gfx::Sampler*       sampler;
        };
    };

private:

    // 共通初期化
    void init(nn::gfx::DescriptorPoolType type, TbSint32 slotCount);

private:

    Type                        m_Type;
    nn::gfx::DescriptorPool     m_DescriptorPool;
    TbMemoryPoolGfx             m_MemoryPool;       //!< メモリーｐｕ－ｒｕ
    TbSint32                    m_BaseIndex;        //!< ベース
    nn::gfx::DescriptorSlot     m_Slot;
    Param                       m_Param;

};

}

#endif