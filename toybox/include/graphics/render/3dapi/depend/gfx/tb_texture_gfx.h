/*!
 * @file
 * @brief gfxのテクスチャ依存コード
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TEXTURE_GFX_H_
#define _INCLUDED_TB_TEXTURE_GFX_H_

#include <base/math/tb_geometry.h>
#include "./tb_descriptor_pool_gfx.h"
#include "graphics/render/3dapi/depend/gfx/tb_memory_pool_gfx.h"

namespace toybox
{

class TbTexture;
struct TbTextureDesc;
class TbRenderDevice;

/*!
 * テクスチャ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbTextureDepend
{
    friend class TbTexture;

public:

    struct SurfaceInfo
    {
        TbColorFormat format;
        TbDimensionS32 dimension;
    };

public:

    // コンストラクタ
    TbTextureDepend();

    // デストラクタ
    ~TbTextureDepend();

public:

    // テクスチャ取得
    nn::gfx::Texture* GetTexture() { return m_MemoryPool.IsInitialize() ? &m_Texture : nullptr; }

    // テクスチャ取得
    nn::gfx::TextureView* GetTextureView() { return m_MemoryPool.IsInitialize() ? &m_TextureView : nullptr; }
   
    // 横幅取得
    inline TbUint32 GetWidth() const { return m_SurfaceInfo.dimension.width; }

    // 縦幅取得
    inline TbUint32 GetHeight() const { return m_SurfaceInfo.dimension.height; }

    // フォーマット取得
    inline TbColorFormat GetFormat() const { return m_SurfaceInfo.format; }

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

    // ファイルから生成
    TbResult setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType );

    // セットアップ
    TbResult setup( TbTexture* texture , const TbTextureDesc& desc );
    
    // メモリープールセットアップ
    TbSizeT setupMemoryPool( nn::gfx::Texture::InfoType& info , TbBool isCompressible );

    // テクスチャビュー作成
    void setupTextureView();

private:

    TbBitArray16                m_BitArray;         //!< ビット配列
    nn::gfx::Texture            m_Texture;          //!< テクスチャ
    nn::gfx::TextureView        m_TextureView;      //!< テクスチャビューs
    TbMemoryPoolGfx             m_MemoryPool;       //!< メモリーｐｕ－ｒｕ

    SurfaceInfo                 m_SurfaceInfo;      //!< サーフェイスインフォ
    TbUint32                    m_Pitch;            //!< ピッチ幅
    TbUint32                    m_BufSize;          //!< バッファサイズ

    TbDescriptorPoolGfx         m_DescriptorPool;
};

}

#endif