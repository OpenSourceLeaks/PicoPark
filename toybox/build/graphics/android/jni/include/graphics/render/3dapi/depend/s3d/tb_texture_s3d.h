/*!
 * テクスチャの依存部分 ( Stage3Dの依存部分)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#ifndef _INCLUDED_TB_TEXTURE_S3D_H_
#define _INCLUDED_TB_TEXTURE_S3D_H_

#include <base/math/tb_geometry.h>

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

    typedef TbUint32 ShaderResourceDepend; // 特に依存データは必要ないので基本型にしておく
    typedef TbUint32 DepthStencilDepend;   // 特に依存データは必要ないので基本型にしておく
    typedef AS3::ui::flash::display3D::textures::Texture TextureS3d;
    
public:

    // コンストラクタ
    TbTextureDepend();

    // デストラクタ
    ~TbTextureDepend();

public:
    
    // テクスチャ取得
    TextureS3d& GetTexture() {
        return m_Texture;
    }
    
    // テクスチャ取得
    const TextureS3d& GetTexture() const {
        return m_Texture;
    }
   
private:

    // ファイルから生成
    TbResult setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType );

    // セットアップ
    TbResult setup( TbTexture* texture , const TbTextureDesc& desc );

private:
    TextureS3d      m_Texture;
    TbBitArray16    m_BitArray;         //!< ビット配列
    TbUint32        m_Pitch;            //!< ピッチ幅
    TbUint32        m_BufSize;          //!< バッファサイズ
    TbUint8*        m_Buffer;           //!< バッファ
};

}

#endif