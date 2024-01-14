/*!
 * @file
 * @brief DX9用深度ステンシルクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_STD_DX9_H_
#define _INCLUDED_TB_DEPTH_STENCIL_STD_DX9_H_

#include "./tb_lost_resource_dx9.h"

namespace toybox
{

class TbDepthStencil;
class TbRenderDevice;
class TbRenderView;
class TbTexture;

class TbDepthStencilDepend : public TbLostResourceDx9
{
public:

    // コンストラクタ
    TbDepthStencilDepend( TbRenderView* view );
    // コンストラクタ
    TbDepthStencilDepend( TbUint32 width , TbUint32 height , TbColorFormat colorFormat );
    // コンストラクタ
    TbDepthStencilDepend( TbTexture* texture );
    // デストラクタ
    ~TbDepthStencilDepend();

public:

    // サーフェス取得
    IDirect3DSurface9* GetSurface() {
        return m_Surface; 
    }

    // 横幅取得
    TbUint32 GetWidth() const { 
        return m_Width; 
    }

    // 縦幅取得
    TbUint32 GetHeight() const { 
        return m_Height; 
    }

public: // 継承関数

    // 解放
    virtual void Release();

    // 再構築
    virtual void Recover();

private:

    void setup( TbDepthStencil* depthStencil );

private:
    IDirect3DSurface9* m_Surface;
    TbDepthStencil* m_DepthStencil;
    TbUint32 m_Width;
    TbUint32 m_Height;
    TbColorFormat m_ColorFormat;
    union {
        TbTexture* m_Texture;
        TbRenderDevice* m_Device;
        TbRenderView* m_View;
    };
    
private:
    friend class TbDepthStencil;
};

}

#endif