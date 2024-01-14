/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GFX_H_
#define _INCLUDED_TB_RENDER_VIEW_GFX_H_

#include <graphics/render/3dapi/depend/gfx/tb_memory_pool_gfx.h>

namespace toybox
{

class TbRenderDevice;
class TbRenderTarget;
class TbRenderView;
class TbTexture;

class TbRenderViewDepend
{
public:

    // コンストラクタ
    TbRenderViewDepend( const TbRenderViewParameter& param );

    //デストラクタ
    ~TbRenderViewDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    // SwapChain取得
    nn::gfx::SwapChain* GetSwapChain() {
        return &m_SwapChain;
    }

    // SwapChain取得
    const nn::gfx::SwapChain* GetSwapChain() const {
        return &m_SwapChain;
    }

    // ScanBuffer取得
    nn::gfx::ColorTargetView* GetScanBuffer(TbSint32 index)
    {
        if (index < TB_ARRAY_COUNT_OF(m_ScanBuffers)) {
            return m_ScanBuffers[index];
        }
        return nullptr;
    }

public:

    void Sync();

public:

    static const TbSizeT SCAN_BUFFER_MAX = 2;

private:

    nn::gfx::SwapChain  m_SwapChain;
    nn::gfx::Semaphore  m_DisplaySemaphore;
    nn::gfx::ColorTargetView* m_ScanBuffers[SCAN_BUFFER_MAX];
    TbMemoryPoolGfx     m_MemoryPool;
    TbRenderTarget*     m_RenderTarget[SCAN_BUFFER_MAX];
    TbTexture*          m_DepthStencil;
    TbUint32            m_CurrentRenderTarget;

private:

    friend class TbRenderView;
};

}

#endif