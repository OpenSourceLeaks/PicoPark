/*!
 * 深度ステンシル
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_H_
#define _INCLUDED_TB_DEPTH_STENCIL_H_

#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{
    enum TbDepthStencilType
    {
        TB_DEPTH_STENCIL_TYPE_BUFFER , // 単なるバッファ
        TB_DEPTH_STENCIL_TYPE_TEXTURE , // テクスチャ
        TB_DEPTH_STENCIL_TYPE_VIEW , // ビューによって自動生成
    };
};

#include "./depend/tb_depth_stencil_depend.h"

namespace toybox
{

class TbTexture;
class TbRenderView;

class TbDepthStencil
{
public:

    // コンストラクタ(ビューに自動生成されたタイプ)
    explicit TbDepthStencil( TbRenderView* view );

    // コンストラクタ(バッファ)
    TbDepthStencil( TbUint32 width , TbUint32 height , TbColorFormat colorFormat );

    // コンストラクタ( テクスチャから生成 )
    explicit TbDepthStencil( TbTexture* texture );

public:

    // デストラクタ
    ~TbDepthStencil();

public:

    // 有効な深度ステンシルバッファか
    TbBool IsValid() const { 
        return m_BitArray.Test( FLAG_VALID ); 
    }

    // タイプ取得
    TbDepthStencilType GetType() const { 
        return m_Type; 
    }
    
    // 解像度横幅取得
    TbUint32 GetWidth() const;

    // 解像度縦幅取得
    TbUint32 GetHeight() const;
    
public: // 依存クラス

    TbDepthStencilDepend& GetDepend() { 
        return m_Depend; 
    }
    
    const TbDepthStencilDepend& GetDepend() const { 
        return m_Depend; 
    }
    
private:

    enum {
        FLAG_VALID , 
    };

private:

    void setup();
    
private:
    
    TbBitArray32 m_BitArray;
    TbDepthStencilType m_Type;
    TbDepthStencilDepend m_Depend;

};

}

#endif