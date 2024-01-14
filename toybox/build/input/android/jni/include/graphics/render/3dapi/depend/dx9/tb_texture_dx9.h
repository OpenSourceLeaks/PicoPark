/*!
 * @file
 * @brief DirectX9のテクスチャ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_TEXTURE_DX9_H_
#define _INCLUDED_TB_TEXTURE_DX9_H_

#include "./tb_lost_resource_dx9.h"
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbRenderDevice;

/*!
 * テクスチャ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbTextureDepend : public TbLostResourceDx9
{
    friend class TbRenderDevice;
    friend class TbTexture;
public:

    struct SurfaceInfo
    {
        TbColorFormat format;
        TbDimensionS32 dimension;
    };

public:

    // シェーダ渡し用リソース依存部分(特にやることないのでUint32型にしておく)
    typedef TbUint32 ShaderResourceDepend;

    // 深度ステンシル依存部分(特にやることないのでUint32型にしておく)
    typedef TbUint32 DepthStencilDepend;
    
public:

    // コンストラクタ
    TbTextureDepend( const void* fileBuf , const TbUint32 bufSize );

    // コンストラクタ
    TbTextureDepend( const TbTextureDesc& desc );

    // デストラクタ
    ~TbTextureDepend();

public:

    //! テクスチャ取得
    LPDIRECT3DTEXTURE9 GetTextureDx9(){ return m_Texture; }

    //! テクスチャ取得
    const LPDIRECT3DTEXTURE9 GetTextureDx9() const { return m_Texture; }

    //! エラーチェック
    TbBool IsError() const { return m_BitArray.Test( FLAG_ERROR ); }
    
    //!< レベル数取得
    TbUint32 GetLevelCount() const { return m_LevelCount; }
    
    //!< 解像度取得
    const SurfaceInfo* GetSurfaceInfo( TbUint32 level ) const{
        if( level < m_LevelCount ){
            return &m_SurfaceInfo[ level ];
        }
        return nullptr;
    }

public: // 継承関数

    // 解放
    virtual void Release();

    // 再構築
    virtual void Recover();

private:

    // フラグ
    enum {
        FLAG_ERROR , 
        FLAG_CREATE_FROM_BUFFER , // バッファによって生成されている
    };

    enum Type
    {
        TYPE_BUFFER , // バッファから生成
        TYPE_DYNAMIC , // 動的テクスチャ
    };
    
private:

    // 動的テクスチャ生成
    void createDynamic();

    // 解像度リスト生成
    void createSurfaceInfoList();

private:

    TbTextureDesc        m_Desc;              //!< 生成用パラメータ(TYPE_DYNAMIC)
    Type                 m_Type;              //!< タイプ
    TbBitArray16         m_BitArray;          //!< ビット配列
    LPDIRECT3DTEXTURE9   m_Texture;           //!< テクスチャ
    TbUint32             m_LevelCount;        //!< レベル数
    SurfaceInfo*         m_SurfaceInfo;        //!< テクスチャ解像度
    
private: // デバイスロスト用

    TbUint8*             m_RecoverBuf;       //!< デバイスロスト時のリカバー用バッファ
    TbUint32             m_RecoverBufSize;  //!< デバイスロスト

};

}

#endif