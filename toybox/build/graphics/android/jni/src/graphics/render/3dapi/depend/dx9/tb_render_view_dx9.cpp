/*!
 * @file
 * @brief レンダービュークラス（ウィンドウハンドル別）
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

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
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderViewDepend::TbRenderViewDepend( const TbRenderViewParameter& param , TbSint32 index)
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_Index( index )
    , m_Param()
    , m_BitArray( 0 )
    , m_SwapChain( nullptr )
    , m_RenderTargetList( nullptr )
    , m_RenderTargetCount( 0U )
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT result = S_OK;
    if( 0 <= m_Index ){
        result = device.GetDepend().GetDeviceDX9()->GetSwapChain( static_cast<TbUint32>(m_Index) , &m_SwapChain );
        m_RenderTargetCount = 1;
    }else{
        result = TbRenderViewParameterToD3DPRESENT_PARAMETERS( device.GetDepend().GetDirect3D9() , &device , device.GetAdapterIndex() , nullptr , param , &m_Param , nullptr );
        result = device.GetDepend().GetDeviceDX9()->CreateAdditionalSwapChain( &m_Param , &m_SwapChain );
        if( FAILED(result) ){
            TbPrintf("CreateAdditionalSwapChain error (%d)\n",result);
        }
        m_RenderTargetCount = m_Param.BackBufferCount;
    }
    if( S_OK != result ){
        m_RenderTargetCount = 0;
        m_SwapChain = nullptr;
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderViewDepend::~TbRenderViewDepend()
{
    Release();
    TB_SAFE_DELETE_ARRAY( m_RenderTargetList );
}

/*!
 * 解放
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbRenderViewDepend::Release()
{
    for( TbUint32 i = 0; i < m_RenderTargetCount; ++i ){
        TB_SAFE_DELETE( m_RenderTargetList[ i ] );
    }
    TbSint32 refCount = m_SwapChain->Release();
    m_SwapChain = nullptr;
}

/*!
 * 再構築
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbRenderViewDepend::Recover()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT result = S_OK;
    if( m_Index >= 0 ){
        result = device.GetDepend().GetDeviceDX9()->GetSwapChain( static_cast<TbUint32>(m_Index) , &m_SwapChain );
    }else{
        result = device.GetDepend().GetDeviceDX9()->CreateAdditionalSwapChain( &m_Param , &m_SwapChain );
    }
    setup( m_Common );
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
void TbRenderViewDepend::setup( TbRenderView* view )
{
    m_Common = view;
    if( !m_RenderTargetList ){
        m_RenderTargetList = TB_NEW TbRenderTarget*[ m_RenderTargetCount ];
    }
    for( TbUint32 i = 0; i < m_RenderTargetCount; ++i ){
        m_RenderTargetList[i] = TB_NEW TbRenderTarget( view , i );
    }
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::TbRenderView( const TbRenderViewParameter& param , TbSint32 index )
    : m_BitArray()
    , m_DepthStencil( nullptr )
    , m_Depend( param , index )
    , m_FrontBufferFormat( TB_COLOR_FORMAT_X8R8G8B8 )
{
    if( !m_Depend.GetSwapChain() ){
        m_BitArray.SetBit( FLAG_ERROR );
    }else{
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        m_Depend.setup( this );
        D3DPRESENT_PARAMETERS d3dParam;
        m_Depend.GetSwapChain()->GetPresentParameters( &d3dParam );
        D3DPRESENT_PARAMETERSToTbRenderViewParameter( m_Param , d3dParam );
        if( m_Param.enableCreateDepthStencil ){
            // 0番目のビューをコピー(DirectX9は、SwapChain毎ではなく自動生成されるのは一つのデバイスに一つのため)
            m_DepthStencil = device.GetView(0) ? device.GetView(0)->GetDepthStencil() : TB_NEW TbDepthStencil( this );
            m_Depend.m_BitArray.SetBit( TbRenderViewDepend::FLAG_CREATE_DEPTH_STENCIL );
        }
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderView::~TbRenderView()
{
    if( m_Depend.m_BitArray.Test( TbRenderViewDepend::FLAG_CREATE_DEPTH_STENCIL ) ){
        TB_SAFE_DELETE( m_DepthStencil );
    }
}

/*!
 * 画面に表示
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */
void TbRenderView::Present()
{
    IDirect3DSwapChain9* swapChain = m_Depend.GetSwapChain();
    if( swapChain ){
        TbRenderDevice& device = TbRenderDevice::GetInstance();
        if( device.GetDepend().IsDeviceLost() ){
            // デバイスロスト中は何もしない
            return;
        }
        const TbWindow* window = m_Param.window ? m_Param.window : device.GetWindow();
        const TbRectS32 rect = TbRenderDevice::GetPresentRect(device.GetPresentType(),device.GetRenderTarget(0),window);
        RECT  targetRect = {rect.x,rect.y,rect.x+rect.width,rect.y+rect.height};
        // &targetRect
        HRESULT hr = S_OK;
        if( device.IsFullscreen() ){
            //TB_ASSERT(!"unsupported");
            m_PresentRegion.x = 0;
            m_PresentRegion.y = 0;
            m_PresentRegion.width = window->GetWidth();
            m_PresentRegion.height = window->GetHeight();
//            TbPrintf("Fullscreen:%d,%d\n",m_PresentRegion.width,m_PresentRegion.height);
            hr = m_Depend.GetSwapChain()->Present( nullptr , nullptr , nullptr , nullptr , 0 );
        }else{
            m_PresentRegion.x = targetRect.left;
            m_PresentRegion.y = targetRect.top;
            m_PresentRegion.width = targetRect.right - targetRect.left;
            m_PresentRegion.height = targetRect.bottom - targetRect.top;
             hr = m_Depend.GetSwapChain()->Present( nullptr , &targetRect , nullptr , nullptr , 0 );
        }
        if( FAILED(hr) ){
            TbPrintf("Present error (%d)(Region=%d,%d,%d,%d)\n",hr,m_PresentRegion.x,m_PresentRegion.y,m_PresentRegion.width,m_PresentRegion.height);
        }
        if( hr ==  D3DERR_DEVICELOST ) {
            device.GetDepend().NotifyDeviceLost();
        }else{
//            TbDxPrintErrorCode(hr);
        }
    }
}

/*!
 * レンダーターゲット取得
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderTarget* TbRenderView::GetRenderTarget( TbUint32 backBufferIndex )
{
    return m_Depend.GetRenderTarget( backBufferIndex );
}

/*!
 * フロントバッファの横幅取得
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbUint32 TbRenderView::GetFrontBufferWidth()
{
    TbUint32 result = 0;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    if( device.IsFullscreen() ){
        D3DDISPLAYMODE displayMode;
        if( SUCCEEDED( m_Depend.GetSwapChain()->GetDisplayMode( &displayMode ) ) ){
            result = displayMode.Width;
        }
    }else{
        result = GetWindow()->GetWidth();
    }
    return result;
}

/*!
 * フロントバッファのピッチ幅取得
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbUint32 TbRenderView::GetFrontBufferPitch()
{
    return GetFrontBufferWidth() * TbGetColorFormatByteSize(m_FrontBufferFormat);
}

/*!
 * フロントバッファの縦幅取得
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbUint32 TbRenderView::GetFrontBufferHeight()
{
    TbUint32 result = 0;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    if( device.IsFullscreen() ){
        D3DDISPLAYMODE displayMode;
        if( SUCCEEDED( m_Depend.GetSwapChain()->GetDisplayMode( &displayMode ) ) ){
            result = displayMode.Height;
        }
    }else{
        result = GetWindow()->GetHeight();
    }
    return result;
}

/*!
 * フロントバッファのフォーマット取得
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbColorFormat TbRenderView::GetFrontBufferFormat()
{
    return m_FrontBufferFormat;
}

/*!
 * フロントバッファ読み込み
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbResult TbRenderView::ReadFrontBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect , TbBool* isDownToUp )
{
    TB_UNUSE( readRect );

    if( !readBuffer ){
        return TB_E_INVALIDARG;
    }

    IDirect3DSurface9* surface = nullptr;
    D3DDISPLAYMODE displayMode;
    if( FAILED( m_Depend.GetSwapChain()->GetDisplayMode( &displayMode ) ) ){
        return TB_E_FAIL;
    }

    TbSint32 width = GetFrontBufferWidth();
    TbSint32 pitch = GetFrontBufferPitch();
    TbSint32 height = GetFrontBufferHeight();

    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT hr = device.GetDepend().GetDeviceDX9()->CreateOffscreenPlainSurface( displayMode.Width , 
                                                             displayMode.Height ,
                                                             D3DFMT_A8R8G8B8 ,
                                                             D3DPOOL_SYSTEMMEM ,
                                                             &surface ,
                                                             nullptr );
    hr = m_Depend.GetSwapChain()->GetFrontBufferData( surface );

    if( FAILED( hr ) ){
        TbPrintf("Failed\n");
        return TB_E_FAIL;
    }

    // ロック
    D3DLOCKED_RECT lockedRect;

    if( device.IsFullscreen() ){
        TbPrintf("Fullscreen\n");
        hr = surface->LockRect( &lockedRect , nullptr , D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK );
    }else{
        RECT rect;
        const TbRectS32& windowRect = GetWindow()->GetRect();
        rect.left = windowRect.x;
        rect.top = windowRect.y;
        rect.right = windowRect.x + width;
        rect.bottom = windowRect.y + height;
        hr = surface->LockRect( &lockedRect , &rect , D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK);
    }

    if( FAILED( hr ) ){
        return TB_E_FAIL;
    }

    // 読み取りバッファサイズチェック
    TbUint32 writeBufferSize = pitch * height;
    if( readBufferSize < writeBufferSize ){
        surface->UnlockRect();
        return TB_E_FAIL;
    }
    if( isDownToUp ){
        *isDownToUp = TB_FALSE;
    }

    // コピー
    TbUint8* dstBuf = reinterpret_cast<TbUint8*>(readBuffer);
    TbUint8* srcBuf = reinterpret_cast<TbUint8*>(lockedRect.pBits);
    TbUint32 dstByteSize = pitch / width;
    TbUint32 srcByteSize = lockedRect.Pitch / width;
    for( TbSint32 i = 0; i < height; ++i ){

        TbMemCopy( dstBuf , srcBuf , pitch );
        dstBuf += pitch;
        srcBuf += lockedRect.Pitch;
    }

    // ロック解除
    surface->UnlockRect();
    TB_SAFE_RELEASE(surface);

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
    IDirect3DSurface9* surface = nullptr;
    HRESULT hr = m_Depend.GetSwapChain()->GetBackBuffer( 0 , D3DBACKBUFFER_TYPE_MONO ,&surface );

 // ロック
    D3DLOCKED_RECT lockedRect;
    hr = surface->LockRect( &lockedRect , nullptr , D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK);

    if( FAILED( hr ) ){
        return TB_E_FAIL;
    }

    // 読み取りバッファサイズチェック
    TbSint32 width = GetBackBufferWidth();
    TbSint32 pitch = GetBackBufferPitch();
    TbSint32 height = GetBackBufferHeight();

    TbUint32 writeBufferSize = pitch * height;
    if( readBufferSize < writeBufferSize ){
        surface->UnlockRect();
        return TB_E_FAIL;
    }

    if( isDownToUp ){
        *isDownToUp = TB_FALSE;
    }

    // コピー
    TbUint8* dstBuf = reinterpret_cast<TbUint8*>(readBuffer);
    TbUint8* srcBuf = reinterpret_cast<TbUint8*>(lockedRect.pBits);
    TbUint32 dstByteSize = pitch / width;
    TbUint32 srcByteSize = lockedRect.Pitch / width;
    for( TbSint32 i = 0; i < height; ++i ){
        TbMemCopy( dstBuf , srcBuf , pitch );
        dstBuf += pitch;
        srcBuf += lockedRect.Pitch;
    }

    // ロック解除
    surface->UnlockRect();
    surface->Release();

    return TB_S_OK;
}

}

#endif
