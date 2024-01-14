/*!
 * レンダーターゲット
 * 設定は全てここに収める
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_H_
#define _INCLUDED_TB_RENDER_TARGET_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_geometry.h>

namespace toybox
{
    enum TbRenderTargetType
    {
        TB_RENDER_TARGET_TYPE_BUFFER , // 単なるバッファ
        TB_RENDER_TARGET_TYPE_TEXTURE , // テクスチャ
        TB_RENDER_TARGET_TYPE_VIEW , // 画面に表示可能なビュー
    };
};

#include "./depend/tb_render_target_depend.h"

namespace toybox
{

class TbRenderDevice;
class TbRenderView;
class TbTexture;

class TbRenderTarget
{
public:

    // コンストラクタ(バッファ)
    TbRenderTarget( TbUint32 width , TbUint32 height , TbColorFormat colorFormat );

    // コンストラクタ(Viewから生成)
    TbRenderTarget( TbRenderView* view , TbUint32 backBufferIndex );

    // コンストラクタ( テクスチャから生成 )
    explicit TbRenderTarget( TbTexture* texture );

public:

    // デストラクタ
    ~TbRenderTarget();

public:

    // 有効なレンダーターゲットか
    TbBool IsValid() const { 
        return m_BitArray.Test( FLAG_VALID ); 
    }

    // タイプ取得
    TbRenderTargetType GetType() const {
        return m_Type; 
    }

    // 解像度横幅取得
    TbUint32 GetWidth() const;

    // 解像度縦幅取得
    TbUint32 GetHeight() const;

    // ピッチ幅取得
    TbUint32 GetPitch();

    // フォーマット取得
    TbColorFormat GetFormat() const;

public:

    // 依存部分取得
    TbRenderTargetDepend& GetDepend() { 
        return m_Depend; 
    }

    // 依存部分取得
    const TbRenderTargetDepend& GetDepend() const { 
        return m_Depend; 
    }
    
private:

    enum {
        FLAG_VALID , 
    };

private:

    // 複数コンストラクタ共通処理用
    void setup();
    
private:
    
    TbBitArray32 m_BitArray;
    TbRenderTargetType m_Type;
    TbRenderTargetDepend m_Depend;

};

}

#endif