/*!
 * @file
 * @brief OpenGLのテクスチャ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_TEXTURE_STD_GL_H_
#define _INCLUDED_TB_TEXTURE_STD_GL_H_

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

    struct SurfaceInfo
    {
        TbColorFormat format;
        TbDimensionS32 dimension;
    };

public:

    typedef TbUint32 ShaderResourceDepend; // 特に依存データは必要ないので基本型にしておく
    typedef TbUint32 DepthStencilDepend;   // 特に依存データは必要ないので基本型にしておく

public:

    // コンストラクタ
    TbTextureDepend();

    // デストラクタ
    ~TbTextureDepend();

public:

    // テクスチャ取得
    GX2Texture* GetTexture() const { return &m_Texture; }
   
    // 横幅取得
    inline TbUint32 GetWidth() const { return m_SurfaceInfo.dimension.width; }

    // 縦幅取得
    inline TbUint32 GetHeight() const { return m_SurfaceInfo.dimension.height; }

    // フォーマット取得
    inline TbColorFormat GetFormat() const { return m_SurfaceInfo.format; }

private:

    enum {
        FLAG_LOCK , 
        FLAG_LOCK_READ_ONLY , 
    };

private:

    // ファイルから生成
    TbResult setup( TbTexture* texture , const void* fileBuf , const TbUint32 bufSize , const TbTextureFileType fileType );

    // セットアップ
    TbResult setup( TbTexture* texture , const TbTextureDesc& desc );

private:

    TbBitArray16                m_BitArray;         //!< ビット配列
    GX2Texture                  m_Texture;          //!< テクスチャ
    SurfaceInfo                 m_SurfaceInfo;      //!< サーフェイスインフォ
    TbUint32                    m_Pitch;            //!< ピッチ幅
    TbUint32                    m_BufSize;          //!< バッファサイズ
    TbUint8*                    m_Buffer;           //!< バッファ
};

}

#endif