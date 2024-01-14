/*!
 * テクスチャリソースクラス
 * @author Miyake Shunsuke
 * @since 2010.09.25
 */

#ifndef _INCLUDED_TB_TEXTURE_H_
#define _INCLUDED_TB_TEXTURE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_render_target.h>
#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{
    enum TbTextureFileType
    {
        TB_TEXTURE_FILE_TYPE_UNKNOWN = -1,
        TB_TEXTURE_FILE_TYPE_BEGIN , 
        TB_TEXTURE_FILE_TYPE_BMP ,
        TB_TEXTURE_FILE_TYPE_TGA ,
        TB_TEXTURE_FILE_TYPE_JPG ,
        TB_TEXTURE_FILE_TYPE_PNG ,
        TB_TEXTURE_FILE_TYPE_DDS ,
        TB_TEXTURE_FILE_TYPE_END ,
        TB_TEXTURE_FILE_TYPE_MAX = TB_TEXTURE_FILE_TYPE_END - TB_TEXTURE_FILE_TYPE_BEGIN ,
    };
    // 2Dテクスチャパラメータ
    struct TbTextureDesc
    {
        enum {
            FLAG_BIT_RENDER_TARGET = TB_BIT(0) , // レンダーターゲットとして
            FLAG_BIT_DEPTH_STENCIL = TB_BIT(1) , // 深度ステンシルバッファとして
        };
        TbTextureDesc() : width(0), height(0), levels(1), flags(0), colorFormat(TB_COLOR_FORMAT_UNKNOWN) {}
        TbUint32      width;       //!< 横幅
        TbUint32      height;      //!< 縦幅
        TbUint32      levels;      //!< ミップマップレベル
        TbUint32      flags;       //!< フラグ
        TbColorFormat colorFormat; //!< カラーフォーマット
    };
    class TbDepthStencil;
}

#include "./depend/tb_texture_depend.h"

namespace toybox
{

class TbTexture
{
    friend class TbTextureDepend;
public:
    
    typedef TbTexture Self;

public:

    // ファイル名からファイルタイプ取得
    static TbTextureFileType GetFileTypeFromFileName( const char* filename );

    // 引数のサイズを格納できるテクスチャサイズ取得
    static TbDimensionS32    GetTextureSize( const TbDimensionS32& size , TbBool isPow = TB_TRUE ) {
        TbDimensionS32 result;
        if( isPow ){
            TbUint32 value = TbMax( TbAlignPow2(size.width) , TbAlignPow2(size.height) );
            result.width = value;
            result.height = value;
        }else{
            result.width = TbAlignPow2(size.width);
            result.height = TbAlignPow2(size.height);
        }
        return result;
    }

public:

    // メモリに読み込んだファイルから生成
    TbTexture( const void* buf , const TbUint32 bufSize , const TbTextureFileType fileType );

    // 空のテクスチャ生成
    TbTexture( const TbTextureDesc& desc );
    
    // デストラクタ
    ~TbTexture();

public:

    // 利用可能
    TbBool IsReady() const;
    
    // テクスチャ書き込み
    TbResult Write( const TbUint32 level , void* buffer , TbUint32 offset , TbUint32 size  );

    // 白に書き込む
    TbResult WriteWhite(const TbUint32 level );

    // テクスチャロック
    TbResult Lock( const TbUint32 level , const TbUint32 flag , void** buffer , TbUint32& pitch , TbUint32& height );

    // テクスチャロック解除
    TbResult Unlock( const TbUint32 level );

    // 解像度(横幅)
    TbUint32 GetWidth( const TbUint32 level ) const;

    // 解像度(縦幅)
    TbUint32 GetHeight( const TbUint32 level ) const;

    // フォーマット取得
    TbColorFormat GetFormat( const TbUint32 level ) const;

    // レンダーターゲット用インタフェース取得
    TbRenderTarget* GetRenderTarget() { 
        return m_RenderTarget; 
    }

    // 深度ステンシル用インタフェース取得
    TbDepthStencil* GetDepthStencil() { 
        return m_DepthStencil; 
    }

    //! 依存コード取得
    TbTextureDepend& GetDepend() { 
        return m_Depend; 
    }

private:

    //　共通処理に利用可能
    void setup();

private:

    TbTextureDesc            m_Info;            //!< 内部情報
    TbRenderTarget*          m_RenderTarget;    //!< レンダーターゲット
    TbDepthStencil*          m_DepthStencil;    //!< 深度ステンシル
    void*                    m_LockedBuffer;    //!< ロック中
    TbTextureDepend          m_Depend;          //!< 機種依存部分

};

}

#endif