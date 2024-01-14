/*!
 * @file
 * @brief レンダービュークラス（ウィンドウハンドル別）
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"

#if TB_3DAPI_IS\TB_3DAPI_GX2

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @parma device 対応デバイス
 * @param param 生成パラメータ
 * @param index 既に生成済みのSwapChainがあれば、そのインデックス
 * @author teco
 */
TbRenderViewDepend::TbRenderViewDepend( const TbRenderViewParameter& param , TbSint32 index)
    : m_RenderTarget(nullptr)
{
    GX2InitColorBufferFTV(&m_ColorBuffer,
                          TB_GX2_WINDOW_WIDTH, TB_GX2_WINDOW_HEIGHT,
                          GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM,
                          GX2_AA_MODE_1X);
    void* surfacePtr = TbGX2AllocMem1(m_ColorBuffer.surface.imageSize,
                                      m_ColorBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_CPU, surfacePtr, m_ColorBuffer.surface.imageSize);
    GX2InitColorBufferPtr(&m_ColorBuffer, surfacePtr);
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    Release();
}

/*!
 * 解放
 * @author teco
 */
void TbRenderViewDepend::Release()
{
}

/*!
 * 再構築
 * @author teco
 */
void TbRenderViewDepend::Recover()
{
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
TbRenderView::TbRenderView( const TbRenderViewParameter& param , TbSint32 index )
    : m_BitArray()
    , m_DepthStencil( nullptr )
    , m_Depend( param , index )
    , m_FrontBufferFormat( TB_COLOR_FORMAT_X8R8G8B8 )
{
    m_Depend.m_RenderTarget = TB_NEW TbRenderTarget( this );
    m_Param = param;
    m_Param.backBufferWidth = TB_GX2_WINDOW_WIDTH;
    m_Param.backBufferHeight= TB_GX2_WINDOW_HEIGHT;
    if( m_Param.enableCreateDepthStencil ){
        // 0番目のビューをコピー(DirectX9は、SwapChain毎ではなく自動生成されるのは一つのデバイスに一つのため)
        m_DepthStencil = TB_NEW TbDepthStencil( this );
    }
}

/*!
 * デストラクタ
 * @author teco
 */
TbRenderView::~TbRenderView()
{
    if( m_Depend.m_BitArray.Test( TbRenderViewDepend::FLAG_CREATE_DEPTH_STENCIL ) ){
        TB_SAFE_DELETE( m_DepthStencil );
    }
}

/*!
 * 画面に表示
 * @author teco
 */
void TbRenderView::Present()
{
    GX2Flush();
    GX2SwapBuffers(&DEMOColorBuffer);
    GX2Flush();
    
    u32 swapCount=0;
    u32 flipCount = 0;
    u32 waitCount=0;
    
    OSTime lastFlipTime;
    OSTime lastVsyncTime;
    OSTime nowTime;
    
    const OSTime t60  = (OSTime)OSSecondsToTicks(1.0f/59.94f);
    OSTime period;
    
    u32    swapInt = GX2GetSwapInterval();
    if (swapInt != 0)
    {
        while (1)
        {
            GX2GetSwapStatus(&swapCount, &flipCount, &lastFlipTime, &lastVsyncTime);
            if (swapCount <= flipCount) {
                break;
            }
            GX2WaitForVsync();
        }
    }
}

/*!
 * レンダーターゲット取得
 * @author teco
 */
TbRenderTarget* TbRenderView::GetRenderTarget( TbUint32 backBufferIndex )
{
    return m_Depend.GetRenderTarget( backBufferIndex );
}

/*!
 * フロントバッファの横幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferWidth()
{
    return TB_GX2_WINDOW_WIDTH;
}

/*!
 * フロントバッファのピッチ幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferPitch()
{
    return TB_GX2_WINDOW_WIDTH * TbGetColorFormatByteSize(m_FrontBufferFormat);
}

/*!
 * フロントバッファの縦幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetFrontBufferHeight()
{
    return TB_GX2_WINDOW_HEIGHT;
}

/*!
 * フロントバッファのフォーマット取得
 * @author teco
 */
TbColorFormat TbRenderView::GetFrontBufferFormat()
{
    return m_FrontBufferFormat;
}

/*!
 * フロントバッファ読み込み
 * @author teco
 */
TbResult TbRenderView::ReadFrontBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect , TbBool* isDownToUp )
{
    TB_ARRAY(!"unsupported");
    return TB_S_OK;
}

/*!
 * バックバッファ読み込み
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferWidth()
{
    TbUint32 result = m_Param.backBufferWidth;
    return result;
}

/*!
 * バックバッファのピッチ幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferPitch()
{
    TbUint32 result = m_Param.backBufferWidth * TbGetColorFormatByteSize(m_Param.backBufferFormat);
    return result;
}

/*!
 * バックバッファの縦幅取得
 * @author teco
 */
TbUint32 TbRenderView::GetBackBufferHeight()
{
    TbUint32 result = m_Param.backBufferHeight;
    return result;
}

/*!
 * バックバッファのフォーマット取得
 * @author teco
 */
TbColorFormat TbRenderView::GetBackBufferFormat()
{
    return m_Param.backBufferFormat;
}
    
/*!
 * バックバッファの読み込み
 * @author teco
 */
TbResult TbRenderView::ReadBackBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect , TbBool* isDownToUp )
{
    TB_ARRAY(!"unsupported");
    return TB_S_OK;
}

}

#endif
