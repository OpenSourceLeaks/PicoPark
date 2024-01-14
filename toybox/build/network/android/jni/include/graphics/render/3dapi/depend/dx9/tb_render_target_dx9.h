/*!
 * @file
 * @brief DX9用レンダーターゲットクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_TARGET_STD_DX9_H_
#define _INCLUDED_TB_RENDER_TARGET_STD_DX9_H_

#include "./tb_lost_resource_dx9.h"

namespace toybox
{

class TbRenderDevice;
class TbRenderView;
class TbRenderTarget;
class TbTexture;

class TbRenderTargetDepend : public TbLostResourceDx9
{
public:

    TbRenderTargetDepend( TbUint32 width , TbUint32 height , TbColorFormat colorFormat );
    TbRenderTargetDepend( TbRenderView* view , TbUint32 backBufferIndex );
    TbRenderTargetDepend( TbTexture* texture );
    ~TbRenderTargetDepend();

public:

    // サーフェス取得
    IDirect3DSurface9* GetSurface() { 
        return m_Surface; 
    }

    // 横幅取得
    inline TbUint32 GetWidth() const { 
        return m_Width; 
    }

    // 縦幅取得
    inline TbUint32 GetHeight() const { 
        return m_Height; 
    }

    // フォーマット取得
    inline const TbColorFormat& GetFormat() const { 
        return m_ColorFormat; 
    }

public: // 継承関数

    // 解放
    virtual void Release();

    // 再構築
    virtual void Recover();

private:

    void setup( TbRenderTarget* target );

private:

    IDirect3DSurface9* m_Surface;
    TbRenderTarget*    m_RenderTarget;
    TbUint32 m_Width;
    TbUint32 m_Height;
    TbColorFormat m_ColorFormat;
    TbUint32 m_BackBufferIndex;
    union {
        TbTexture* m_Texture;
        TbRenderView* m_View;
        TbRenderDevice* m_Device;
    };
    
private:
    friend class TbRenderTarget;
};

}

#endif