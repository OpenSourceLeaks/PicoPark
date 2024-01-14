/*!
 * 描画パネルクラスの実装ファイル
 * @author teco
 * @since 2010.05.10
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_device.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_view.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_shader_manager.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"
#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/tb_index_buffer.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/tb_texture_sampler.h"
#include "graphics/window/tb_window.h"
#include "graphics/render/3dapi/depend/dx9/tb_render_util_dx9.h"
#include "base/util/tb_depend_cast.h"

namespace toybox
{

namespace
{

    static const D3DPRIMITIVETYPE PRIM_TYPE[ TB_PT_MAX  ] = 
    {
        D3DPT_POINTLIST,
        D3DPT_LINELIST,
        D3DPT_LINESTRIP,
        D3DPT_TRIANGLELIST,
        D3DPT_TRIANGLESTRIP,
        D3DPT_TRIANGLEFAN,
    };
    
    void D3DDISPLAYMODEtoTbDisplayMode( D3DDISPLAYMODE& from , TbDisplayMode& dst )
    {
        dst.format = D3DFORMATtoTbColorFormat( from.Format );
        dst.height = from.Height;
        dst.width = from.Width;
        dst.refreshRate = from.RefreshRate;
    }
}

/////////////////////////////////////////////////////////////////////
// 依存コード

/*!
 * コンストラクタ
 * @author teco
 * @since 2010.07.07
 */
TbRenderDeviceDepend::TbRenderDeviceDepend()
    : m_Direct3D9( nullptr )
    , m_DeviceDX9( nullptr )
    , m_Param()
    , m_LostResourceRoot()
{
    m_Direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2010.07.07
 */
TbRenderDeviceDepend::~TbRenderDeviceDepend()
{
    TB_SAFE_RELEASE( m_Direct3D9 );
    TB_SAFE_RELEASE( m_DeviceDX9 );
}

/*!
 * セットアップ
 * @author teco
 * @since 2010.07.07
 */
TB_INLINE TbBool TbRenderDeviceDepend::Initialize( TbRenderDevice* device , TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter )
{
    if( m_DeviceDX9 ) {
        // 生成済み
        return TB_FALSE;
    }
    if( focusWindow )
    {
        HWND windowHandle = focusWindow->GetDepend().GetWindowHandle();
        HRESULT result;
        if( !TbRenderViewParameterToD3DPRESENT_PARAMETERS( m_Direct3D9 , device , adapterIndex , focusWindow , parameter , &m_Param , &m_FullscreenParam ) ){
            TB_ASSERT(!"Failed Create Device");
            return TB_FALSE;
        }

#if 1 && TB_IS_DEBUG // スクリーンショットとりたいときの暫定対応
        m_Param.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        m_FullscreenParam.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
#endif

        D3DPRESENT_PARAMETERS* param = parameter.isFullscreen ? &m_FullscreenParam : &m_Param;
        TbPrintf("[Create Device] back=(%d,%d)\n",param->BackBufferWidth,param->BackBufferHeight);

        // ハードウェア処理で生成
        result = m_Direct3D9->CreateDevice( adapterIndex , D3DDEVTYPE_HAL , windowHandle,
                                             D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                             param,&m_DeviceDX9);
        if( SUCCEEDED(result) ){
            return TB_TRUE;
        }
        // ソフトウェア処理で生成
        result = m_Direct3D9->CreateDevice(adapterIndex,D3DDEVTYPE_HAL,windowHandle,
                                             D3DCREATE_SOFTWARE_VERTEXPROCESSING ,
                                             param,&m_DeviceDX9);
        if( SUCCEEDED(result) ){
            return TB_TRUE;
        }
    }
    TbPrintf("[failed create device]\n");
    TB_ASSERT(!"failed create device");
    return TB_FALSE;
}

/*!
 * デバイスロスト通知
 * @author teco
 * @since 2011.01.16
 */
void TbRenderDeviceDepend::NotifyDeviceLost()
{
    if( !m_BitArray.Test( FLAG_DEVICELOST ) ){
        TbPrintf( "[Device Lost]\n" );

        m_BitArray.SetBit( FLAG_DEVICELOST );
        releaseResource();
    }
}

/*!
 * リソースを解放する
 * @author teco
 * @since 2011.01.15
 */
void TbRenderDeviceDepend::releaseResource()
{
    TbLostResourceDx9* resource = m_LostResourceRoot.GetNext();
    while( resource != &m_LostResourceRoot ){
        resource->Release();
        resource = resource->GetNext();
    }
    m_CurrentVertexFormat = nullptr;

    TbRenderDevice& device = TB_DEPEND_TO_COMMON(TbRenderDevice,m_Depend);
    // 依存クラスから非依存クラスの中身を直接設定( あまり綺麗ではないが・・・ )
    TbMemClear( device.m_RenderTarget , sizeof(device.m_RenderTarget) );
    device.m_DepthStencil = nullptr;
    TbMemClear( device.m_Texture , sizeof(device.m_Texture) );
}

/*!
 * リセット
 * @author teco
 * @since 2011.01.17
 */
TbBool TbRenderDeviceDepend::reset( TbBool isFullscreen )
{
    HRESULT hr = m_DeviceDX9->Reset( isFullscreen ? &m_FullscreenParam : &m_Param );
    if( SUCCEEDED(hr) ){
        recoverResource();
        m_CurrentVertexFormat = nullptr;

        TbRenderDevice& device = TB_DEPEND_TO_COMMON(TbRenderDevice,m_Depend);
        device.m_RenderTarget[ 0 ] = device.GetView(0)->GetRenderTarget( 0 );
        device.m_DepthStencil = device.GetView(0)->GetDepthStencil();

        m_BitArray.SetBit( FLAG_DEVICELOST , TB_FALSE );
        return TB_TRUE;
    }else{
        TbDxPrintErrorCode(hr);
        TB_ASSERT(!"failed to TbRenderDeviceDepend::reset");
    }
    return TB_FALSE;
}

/*!
 * リソースを再構築
 * @author teco
 * @since 2011.01.15
 */
TB_INLINE  void TbRenderDeviceDepend::recoverResource()
{
    TbLostResourceDx9* resource = m_LostResourceRoot.GetNext();
    while( resource != &m_LostResourceRoot ){
        resource->Recover();
        resource = resource->GetNext();
    }
}

/*!
 * 現在設定されてる頂点フォーマット設定
 * @author teco
 * @since 2010.08.19
 */
void TbRenderDeviceDepend::SetCurrentVertexFormat( const TbVertexFormat* format )
{
    HRESULT hr;
    m_CurrentVertexFormat = format;
    if( nullptr != m_CurrentVertexFormat ){
        if( m_CurrentVertexFormat->GetDepend().GetVertexDeclaration() ){
            hr = m_DeviceDX9->SetVertexDeclaration( m_CurrentVertexFormat->GetDepend().GetVertexDeclaration() );
        }else{
            hr = m_DeviceDX9->SetFVF( m_CurrentVertexFormat->GetDepend().GetFVF() );
        }
    }else{
        hr = m_DeviceDX9->SetVertexDeclaration( nullptr );
        hr = m_DeviceDX9->SetFVF( 0 );
    }
}

/*!
 * フルスクリーン設定
 * @author teco
 */
TbBool TbRenderDeviceDepend::setFullscreen( TbBool isFullscreen )
{
    releaseResource();
    return reset( isFullscreen );
}

/////////////////////////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 * @since 2010.05.10
 */
TbRenderDevice::TbRenderDevice( TbSizeT viewMax )
    : m_Window( nullptr )
    , m_BitArray()
    , m_AdapterIndex( 0 )
    , m_ShaderManager()
    , m_FixedShader()
    , m_PresentType( TB_RENDER_PRESENT_DEFAULT )
    , m_IndexBuffer( nullptr )
    , m_ViewList( viewMax )
    , m_ActiveTextureCount( 0U )
    , m_DepthStencil( nullptr )
    , m_RasterizerState( nullptr )
    , m_ViewportSciessorState( nullptr )
    , m_BlendState(nullptr)
    , m_DepthStencilState(nullptr)
    , m_Depend()
{
    TbMemClear( m_Texture , sizeof( m_Texture ) );
    TbMemClear( m_TextureSampler , sizeof( m_TextureSampler ) );
    TbMemClear( m_RenderTarget , sizeof( m_RenderTarget ) );
    initAlphaTestState();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2010.05.10
 */
TbRenderDevice::~TbRenderDevice()
{
    TbRenderView* view = nullptr;
    TbForIterator( it , m_ViewList ) {
        view = *it;
        TB_SAFE_DELETE( view );
    }
}

/*!
 * セットアップ
 * @param window フォーカスウィンドウ
 * @param parameter パラメータ
 * @author teco
 * @since 2010.05.10
 */
TbBool TbRenderDevice::Initialize( TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter)
{
    if( m_Depend.Initialize( this , adapterIndex , focusWindow , flags , parameter ) ){

        m_AdapterIndex = adapterIndex;

        m_BitArray.SetBit( FLAG_FULLSCREEN , parameter.isFullscreen );

        // ビュー作成
        TbRenderView* view = TB_NEW TbRenderView( parameter , 0 );
        m_ViewList.PushBack( view );
        m_RenderTarget[ 0 ] = view->GetRenderTarget(0);

        // 自動生成されている深度ステンシルがある場合は作成
        if( parameter.enableCreateDepthStencil ){
//            m_DefaultDepthStencil = TB_NEW TbDepthStencil( this );
            m_DepthStencil = view->GetDepthStencil();
        }

        // ステート設定
        initCommon();
        initDefaultState(parameter);
        m_BitArray.SetBit(FLAG_INITIALIZE);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 性能チェック
 * @author Miyake_Shunsuke
 * @since 2014.04.03
 */
void TbRenderDevice::GetCaps( Caps& caps )
{
    TbMemClear(caps);
    D3DCAPS9 src;
    m_Depend.GetDeviceDX9()->GetDeviceCaps(&src);
    caps.shaderConstMax[TB_SHADER_TYPE_VERTEX] = src.MaxVertexShaderConst;
    caps.shaderConstMax[TB_SHADER_TYPE_PIXEL] = 224; // ps_2_0なら32
}

/*!
 * 対応しているディスプレイの現在のグラフィックスモードを取得
 * @author teco
 * @since 2010.05.05
 */
TbBool TbRenderDevice::GetCurrentDisplayMode( TbUint32 index , TbDisplayMode& outDisplayMode ) const
{
    if( 0 <= index && index < m_Depend.GetDirect3D9()->GetAdapterCount() ){
        D3DDISPLAYMODE displayMode;
        if( S_OK == m_Depend.GetDirect3D9()->GetAdapterDisplayMode( index , &displayMode ) ){
            D3DDISPLAYMODEtoTbDisplayMode( displayMode , outDisplayMode );
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 * @since 2010.05.05
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbDisplayMode& outDisplayMode ) const
{
    static const D3DFORMAT FORMAT_LIST[] = 
    {
        D3DFMT_A1R5G5B5 , 
        D3DFMT_A2R10G10B10 , 
        D3DFMT_A8R8G8B8 , 
        D3DFMT_R5G6B5 , 
        D3DFMT_X1R5G5B5 , 
        D3DFMT_X8R8G8B8 ,
    };
    if( index < 0 && index >= m_Depend.GetDirect3D9()->GetAdapterCount() ){
        return TB_FALSE;
    }
    TbSint32 num = 0;
    TbSint32 formatIndex = 0;
    for( formatIndex = 0; formatIndex < TB_ARRAY_COUNT_OF( FORMAT_LIST ); ++formatIndex ){
        TbSint32 num = m_Depend.GetDirect3D9()->GetAdapterModeCount( index , FORMAT_LIST[formatIndex] );
        if( mode < num ){
            break;
        }
        mode -= num;
    }
    if( formatIndex < TB_ARRAY_COUNT_OF( FORMAT_LIST ) ){       
        D3DDISPLAYMODE displayMode;
        if( S_OK == m_Depend.GetDirect3D9()->EnumAdapterModes( index , FORMAT_LIST[formatIndex] , mode , &displayMode ) ){
            D3DDISPLAYMODEtoTbDisplayMode( displayMode , outDisplayMode );
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 対応しているディスプレイのグラフィックスモードを取得
 * @author teco
 * @since 2010.05.05
 */
TbBool TbRenderDevice::GetDisplayMode( TbUint32 index , TbSint32 mode , TbColorFormat format , TbDisplayMode& outDisplayMode ) const
{
    if( index < 0 && index >= m_Depend.GetDirect3D9()->GetAdapterCount() ){
        return TB_FALSE;
    }
    D3DDISPLAYMODE displayMode;
    if( S_OK == m_Depend.GetDirect3D9()->EnumAdapterModes( index , TbColorFormatToD3DFORMAT( format ) , mode , &displayMode ) ){
        D3DDISPLAYMODEtoTbDisplayMode( displayMode , outDisplayMode );
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author teco
 * @since 2010.05.05
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index ) const
{
    TbSint32 num = 0;
    if( 0 <= index && index < m_Depend.GetDirect3D9()->GetAdapterCount() )
    {
        if( m_Depend.GetDirect3D9() ){
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_A1R5G5B5 );
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_A2R10G10B10 );
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_A8R8G8B8 );
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_R5G6B5 );
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_X1R5G5B5 );
            num += m_Depend.GetDirect3D9()->GetAdapterModeCount( index , D3DFMT_X8R8G8B8 );
        }
    }
    return num;
}

/*!
 * 対応しているディスプレイのグラフィックスモード数を取得
 * @author teco
 * @since 2010.05.05
 */
TbSint32 TbRenderDevice::GetDisplayModeNum( TbUint32 index , TbColorFormat format ) const
{
    TbSint32 num = 0;
    if( 0 <= index && index < m_Depend.GetDirect3D9()->GetAdapterCount() ){
        if( m_Depend.GetDirect3D9() ){
            num = m_Depend.GetDirect3D9()->GetAdapterModeCount( index , TbColorFormatToD3DFORMAT( format ) );
        }
    }
    return num;
}

/*!
 * ビュー生成
 * @author teco
 * @since 2010.12.05
 */
TbRenderView* TbRenderDevice::CreateView( const TbRenderViewParameter& parameter )
{
    TbRenderView* result = nullptr;
    result = TB_NEW TbRenderView( parameter , -1 );
    if( result->IsError() ){
        TB_SAFE_DELETE( result );
    }else{
        m_ViewList.PushBack( result );
    }
    return result;
}

/*!
 * ビューポート設定
 * @author teco
 * @since 2011.05.25
 */
TbResult TbRenderDevice::SetViewportScissorState( const TbViewportScissorState* state )
{
    if (!state) {
        return SetViewportScissorState(&m_DefaultViewportScissorState);
    }

    const TbViewportScissorState::Param& param = state->GetParam();
    D3DVIEWPORT9 viewport = 
    {
        param.viewPortRect.x , param.viewPortRect.y , param.viewPortRect.width , param.viewPortRect.height ,
        0.0f , 1.0f 
    };
    if( S_OK == m_Depend.GetDeviceDX9()->SetViewport( &viewport ) ){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

#if 0
/*!
 * ビューポート取得
 * @author teco
 * @since 2011.05.25
 */
TbResult TbRenderDevice::GetViewport( TbRectS32& rect ) const
{
     D3DVIEWPORT9 viewport;
    if( S_OK == m_Depend.GetDeviceDX9()->GetViewport( &viewport ) ){
        rect.x = viewport.X;
        rect.y = viewport.Y;
        rect.width = viewport.Width;
        rect.height = viewport.Height;
        return TB_S_OK;
    }
    return TB_E_FAIL;   
}
#endif

/*!
 * クリア
 * @author teco
 * @since 2011.05.26
 */
TbResult TbRenderDevice::Clear( TbUint32 clearTargetBit , const TbClearColor& color , TbFloat32 depth , TbUint32 stencil )
{
    DWORD flags = 0;
    if( clearTargetBit & TB_RENDER_BUFFER_TYPE_COLOR_BIT ){
        flags |= D3DCLEAR_TARGET;
    }
    if( (clearTargetBit & TB_RENDER_BUFFER_TYPE_DEPTH_BIT) && m_DepthStencil ){
        flags |= D3DCLEAR_ZBUFFER;
    }
    if( (clearTargetBit & TB_RENDER_BUFFER_TYPE_STENCIL_BIT) && m_DepthStencil ){
        flags |= D3DCLEAR_STENCIL;
    }
    
    
    D3DCOLOR d3dColor = D3DCOLOR_XRGB(color.GetRedU8(),color.GetGreenU8(),color.GetBlueU8());
    HRESULT hr = m_Depend.GetDeviceDX9()->Clear(0, nullptr, flags , d3dColor , depth , stencil);
        //Clear( 0, nullptr, flags , color.value , depth , stencil );
    if( S_OK == hr ){
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * シーン開始
 * @author teco
 * @since 2010.05.10
 */
void TbRenderDevice::BeginScene()
{
    if( m_Depend.IsDeviceLost() ){
        // フォーカスが戻ってくるまでは再生成はできない
        HRESULT hr = m_Depend.GetDeviceDX9()->TestCooperativeLevel();
        switch( hr ){
        case D3DERR_DEVICENOTRESET:
            {
                if( m_Depend.reset(IsFullscreen()) ){
                    // 状態を復帰する
                    initCommon();
                    SetDepthStencilState(m_DepthStencilState);
                    SetBlendState(m_BlendState);
                    SetRasterizerState(m_RasterizerState);
                }else{
                    return;
                }
            }
            break;
        case D3DERR_DEVICELOST:
            return;
        default:
            TbDxPrintErrorCode(hr);
            return;
        }
    }
    HRESULT hr = m_Depend.GetDeviceDX9()->BeginScene();
    TB_ASSERT(SUCCEEDED(hr));
    hr = 0;
}

/*!
 * ストリームソース設定
 * @author teco
 * @since 2010.05.10
 */
void TbRenderDevice::SetVertexBuffer( TbUint32 streamNumber , TbVertexBuffer* vertexBuffer , TbUint32 offsetInBytes , TbUint32 stride )
{
    if( m_Depend.IsDeviceLost() ){
        return;
    }
    HRESULT hr = m_Depend.GetDeviceDX9()->SetStreamSource( streamNumber , vertexBuffer->GetDepend().GetRawData() , offsetInBytes , stride );
    TB_ASSERT(SUCCEEDED(hr));
    hr = 0;
}

/*!
 * 頂点インデックスバッファ設定
 * @author teco
 * @since 2011.06.05
 */
void TbRenderDevice::SetIndexBuffer( TbIndexBuffer* indexBuffer )
{
    if( m_Depend.IsDeviceLost() ){
        return;
    }
    m_IndexBuffer = indexBuffer;
    HRESULT hr = m_Depend.GetDeviceDX9()->SetIndices( indexBuffer->GetDepend().GetRawData() );
    TB_ASSERT(SUCCEEDED(hr));
}

/*!
 * プリミティブ描画
 * @author teco
 * @since 2010.05.10
 */
TbResult TbRenderDevice::DrawPrimitive( TbPrimitiveType primitiveType , TbUint32 startIndex , TbUint32 primitiveCount )
{
    if( m_Depend.IsDeviceLost() ){
        return TB_E_FAIL;
    }
#if TB_IS_DEBUG
    TB_ASSERT_MSG( IsValidRenderTargetAndDepthStencil() , " DepthStencil or RenderTarget is Invalid " );
#endif
    DWORD res = 0;
    HRESULT hr = m_Depend.GetDeviceDX9()->DrawPrimitive( PRIM_TYPE[ primitiveType ] , startIndex , primitiveCount );
    return SUCCEEDED(hr) ? TB_S_OK : TB_E_FAIL;
}

/*!
 * プリミティブ描画
 * @author teco
 * @since 2010.05.10
 */
TbResult TbRenderDevice::DrawPrimitive( const TbPrimitiveType primitiveType , 
                                        const TbUint32 primitiveCount , 
                                        const void* vertexs , 
                                        const TbUint32 stride )
{
    if( m_Depend.IsDeviceLost() ){
        return TB_E_FAIL;
    }
#if TB_IS_DEBUG
    TB_ASSERT_MSG( IsValidRenderTargetAndDepthStencil() , " DepthStencil or RenderTarget is Invalid " );
#endif
    HRESULT hr = m_Depend.GetDeviceDX9()->DrawPrimitiveUP( PRIM_TYPE[ primitiveType ] , primitiveCount , vertexs , stride );
    return SUCCEEDED(hr) ? TB_S_OK : TB_E_FAIL;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画
 * @author teco
 * @since 2011.06.06
 */
TbResult TbRenderDevice::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                                           const TbUint32 minVertexIndex ,
                                           const TbUint32 vertexCount , 
                                           const TbUint32 startIndex , 
                                           const TbUint32 primitiveCount )
{
    if( m_Depend.IsDeviceLost() ){
        return TB_E_FAIL;
    }
#if TB_IS_DEBUG
    TB_ASSERT_MSG( IsValidRenderTargetAndDepthStencil() , " DepthStencil or RenderTarget is Invalid " );
#endif
    HRESULT hr = m_Depend.GetDeviceDX9()->DrawIndexedPrimitive( PRIM_TYPE[ primitiveType ] ,    
                                                                0 , 
                                                                minVertexIndex , 
                                                                vertexCount ,
                                                                startIndex , 
                                                                primitiveCount );
    return SUCCEEDED(hr) ? TB_S_OK : TB_E_FAIL;
}

/*!
 * 頂点インデックスを利用したプリミティブ描画( 生の頂点配列使用とインデックス配列 )
 * @author teco
 * @since 2011.06.06
 */
TbResult TbRenderDevice::DrawIndexedPrimitive( const TbPrimitiveType primitiveType , 
                           const TbUint32 minVertexIndex ,
                           const TbUint32 vertexCount , 
                           const void* indexData , 
                           const TbIndexBufferFormat indexFormat , 
                           const void* vertexs , 
                           TbUint32 stride ,
                           const TbUint32 primitiveCount )
{
    D3DFORMAT format = D3DFMT_UNKNOWN;
    if( m_Depend.IsDeviceLost() ){
        return TB_E_FAIL;
    }
    if( TB_S_OK != TbIndexBufferDepend::ConvertFormatToD3DFORMAT( format , indexFormat ) ){
        return TB_E_FAIL;
    }
#if TB_IS_DEBUG
    TB_ASSERT_MSG( IsValidRenderTargetAndDepthStencil() , " DepthStencil or RenderTarget is Invalid " );
#endif
    HRESULT hr = m_Depend.GetDeviceDX9()->DrawIndexedPrimitiveUP( PRIM_TYPE[ primitiveType ] ,    
                                                                  minVertexIndex , 
                                                                  vertexCount ,
                                                                  primitiveCount ,
                                                                  indexData , 
                                                                  format ,
                                                                  vertexs , 
                                                                  stride );
    return SUCCEEDED(hr) ? TB_S_OK : TB_E_FAIL;
}

/*!
 * シーン終了
 */
void TbRenderDevice::EndScene()
{
    if( m_Depend.IsDeviceLost() ){
        return;
    }
    HRESULT hr = m_Depend.GetDeviceDX9()->EndScene();
    TB_ASSERT(SUCCEEDED(hr));
    hr = 0;
}

/*!
 * レンダーターゲット設定
 * @author teco
 * @since 2010.12.05
 */
TbResult TbRenderDevice::SetRenderTarget( TbUint32 index , TbRenderTarget* renderTarget )
{
    if( m_Depend.IsDeviceLost() ){ // デバイスロスト中は失敗
        return TB_E_FAIL;
    }
    if( index > RENDER_TARGET_MAX ){ // indexの値が不正
        return TB_E_FAIL;
    }
    if( m_RenderTarget[ index ] == renderTarget ){ // 既に設定済みなら何もしない
        return TB_S_OK;
    }
    HRESULT hr = m_Depend.GetDeviceDX9()->SetRenderTarget( index , renderTarget->GetDepend().GetSurface() );
    if( SUCCEEDED(hr) ){
        m_RenderTarget[ index ] = renderTarget;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 深度ステンシル設定
 * @author teco
 * @since 2010.12.05
 */
TbResult TbRenderDevice::SetDepthStencil( TbDepthStencil* depthStencil )
{
    if( m_Depend.IsDeviceLost() ){ // デバイスロスト中は失敗
        return TB_E_FAIL; 
    }
    if( m_DepthStencil == depthStencil ){ // 既に設定済みなら終了
        return TB_S_OK;
    }
    HRESULT hr = m_Depend.GetDeviceDX9()->SetDepthStencilSurface( depthStencil ? depthStencil->GetDepend().GetSurface() : nullptr );
    if( SUCCEEDED(hr) ){
        m_DepthStencil = depthStencil;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 現在設定されている深度ステンシルとレンダーターゲットの組み合わせは有効か
 * @author teco
 * @since 2011.06.01
 */
TbBool TbRenderDevice::IsValidRenderTargetAndDepthStencil() const
{
    TbBool result = TB_TRUE;
    if( m_DepthStencil )
    {
        TbUint32 depthWidth = m_DepthStencil->GetWidth();
        TbUint32 depthHeight = m_DepthStencil->GetHeight();
        // TIPS:
        // DirectX9では、深度ステンシルの解像度は
        // レンダーターゲットよりも大きくなければならない
        for( TbUint32 i = 0; i < RENDER_TARGET_MAX; ++i ){
            if( m_RenderTarget[ i ] && 
                ( m_RenderTarget[ i ]->GetWidth() < depthWidth || m_RenderTarget[ i ]->GetHeight() < depthHeight ) )
            {
                result = TB_FALSE;
                break;
            }
        }
    }
    return result;
}

/*!
 * テクスチャ設定
 * @author teco
 * @since 2010.09.29
 */
TbBool TbRenderDevice::SetTexture( TbSint32 stage , TbTexture* texture , TbShaderType shaderType )
{
    if( shaderType == TB_SHADER_TYPE_PIXEL && 0 <= stage && stage < TEXTURE_STAGE_MAX && !m_Depend.IsDeviceLost() )
    {
        IDirect3DTexture9* textureDX9 = texture ? texture->GetDepend().GetTextureDx9() : nullptr;
        HRESULT result =m_Depend.GetDeviceDX9()->SetTexture( stage , textureDX9 );
        // 割り当てテクスチャ数増減
        if( texture && !m_Texture[ stage ] ){
            ++m_ActiveTextureCount;
        }else if( !texture && m_Texture[ stage ] ){
            --m_ActiveTextureCount;
        }
        // 保持しておく
        m_Texture[ stage ] = texture;
        return result == S_OK;
    }
    return TB_FALSE;
}

/*!
 * 現在利用されているテクスチャを取得
 * @author teco
 * @since 2010.10.17
 */
TbTexture*  TbRenderDevice::GetTexture( TbSint32 stage , TbShaderType shaderType )
{
    TbTexture* result = nullptr;
    if( shaderType == TB_SHADER_TYPE_PIXEL && 0 <= stage && stage < TEXTURE_STAGE_MAX )
    {
        result = m_Texture[ stage ];
    }
    return result;    
}

/*!
* ラスタライザステート設定
* @author teco
*/
TbResult TbRenderDevice::SetRasterizerState(const TbRasterizerState* state)
{
    if (!state) {
        return SetRasterizerState(&m_DefaultRasterizerState);
    }

    const TbRasterizerState::Param& param = state->GetParam();

    static const D3DFILLMODE FILL_CONVERT_TABLE[] = {
        D3DFILL_POINT ,
        D3DFILL_WIREFRAME ,
        D3DFILL_SOLID ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(FILL_CONVERT_TABLE) == TB_FILL_MODE_MAX);
    
    D3DCULL CULL_TABLE[] = {
        D3DCULL_NONE ,
        (param.isFrontFaceClockWise ? D3DCULL_CW : D3DCULL_CCW) ,
        (param.isFrontFaceClockWise ? D3DCULL_CCW : D3DCULL_CW) ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(CULL_TABLE) == TB_CULL_MODE_MAX);

    LPDIRECT3DDEVICE9 device = GetDepend().GetDeviceDX9();
        
    // fill mode
    if ( device && TB_FILL_MODE_BEGIN <= param.fillMode && param.fillMode < TB_FILL_MODE_END) {
         device->SetRenderState(D3DRS_FILLMODE, FILL_CONVERT_TABLE[param.fillMode - TB_FILL_MODE_BEGIN]);
    } else {
        return TB_E_FAIL;
    }

    // cull mode
    if (device && TB_CULL_MODE_BEGIN <= param.cullMode && param.cullMode < TB_CULL_MODE_END) {
        device->SetRenderState(D3DRS_CULLMODE, CULL_TABLE[param.cullMode - TB_CULL_MODE_BEGIN]);
    }
    else {
        return TB_E_FAIL;
    }

    m_RasterizerState = state;
}

/*!
 * テクスチャサンプラー設定
 * @author teco
 */
TbResult TbRenderDevice::SetTextureSampler( TbUint32 slot , TbTextureSampler* sampler , TbShaderType shaderType )
{
    LPDIRECT3DDEVICE9 device = GetDepend().GetDeviceDX9();
    D3DTEXTUREFILTERTYPE minFilterType;
    D3DTEXTUREFILTERTYPE magFilterType;
    D3DTEXTUREADDRESS addressModeU;
    D3DTEXTUREADDRESS addressModeV;

    const TbTextureSamplerParam& param = sampler->GetParam();

    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return TB_E_FAIL;
    }
    // nullptrの場合はここで終了
    if( !sampler ){
        m_TextureSampler[ slot ] = nullptr;
        return TB_S_OK;
    }
    if( !device ||
        !TbTextureFilterModeToD3DTEXTUREFILTERTYPE( minFilterType , param.minFilterMode) ||
        !TbTextureFilterModeToD3DTEXTUREFILTERTYPE( magFilterType , param.magFilterMode) ||
        !TbTextureAddressModeToD3DTEXTUREADDRESS( addressModeU , param.addressModeU ) ||
        !TbTextureAddressModeToD3DTEXTUREADDRESS( addressModeV , param.addressModeV ) )
    {
        return TB_E_FAIL;
    }
    m_TextureSampler[ slot ] = sampler;
    device->SetSamplerState( slot , D3DSAMP_MINFILTER , minFilterType );
    device->SetSamplerState( slot , D3DSAMP_MAGFILTER , magFilterType );
    device->SetSamplerState( slot , D3DSAMP_ADDRESSU , addressModeU );
    device->SetSamplerState( slot , D3DSAMP_ADDRESSV , addressModeV );
    return TB_S_OK;
}

/*!
 * テクスチャサンプラー取得
 * @author teco
 */
TbTextureSampler* TbRenderDevice::GetTextureSampler( TbUint32 slot , TbShaderType shaderType )
{
    // 引数不正チェック
    if( slot >= TEXTURE_SAMPLER_SLOT_MAX || 
        TB_SHADER_TYPE_PIXEL != shaderType )
    {
        return nullptr;
    }
    return m_TextureSampler[ slot ];
}

/*!
 * ブレンドステート設定
 * @author teco
 */
TbResult TbRenderDevice::SetBlendState(const TbBlendState* state)
{
    if (!state) {
        return SetBlendState(&m_DefaultBlendState);
    }

    const TbBlendState::Param& param = state->GetParam();

    // enable blend
    {
        m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_ALPHABLENDENABLE, param.isEnableBlend);
    }

    // blend op 
    {
        D3DBLENDOP blendOpD3D;
        if (TbBlendOpToD3DBLENDOP(blendOpD3D, param.blendOp))
        {
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_BLENDOP, blendOpD3D);
        }
        else
        {
            return TB_E_FAIL;
        }
    }

    // blend factor
    {
        D3DBLEND srcFactorD3D;
        D3DBLEND dstFactorD3D;
        if (TbBlendFactorToD3DBLEND(srcFactorD3D, param.blendSrcFactor) &&
            TbBlendFactorToD3DBLEND(dstFactorD3D, param.blendDstFactor))
        {
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_SRCBLEND, srcFactorD3D);
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_DESTBLEND, dstFactorD3D);
        }
        else
        {
            return TB_E_FAIL;
        }
    }

    // color mask
    {
        DWORD flags = 0;
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_RED) {
            flags |= D3DCOLORWRITEENABLE_RED;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_GREEN) {
            flags |= D3DCOLORWRITEENABLE_GREEN;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_BLUE) {
            flags |= D3DCOLORWRITEENABLE_BLUE;
        }
        if (param.colorMaskBit & TB_COLOR_MASK_BIT_ALPHA) {
            flags |= D3DCOLORWRITEENABLE_ALPHA;
        }
        m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_COLORWRITEENABLE, flags);
    }

    m_BlendState = state;
    return TB_S_OK;
}

/*!
 * アルファテスト有効設定
 * @author teco
 * @since 2011.06.11
 */
TbResult TbRenderDevice::SetEnableAlphaTest( TbBool isEnable )
{
    m_Depend.GetDeviceDX9()->SetRenderState( D3DRS_ALPHATESTENABLE , isEnable );
    m_AlphaTestState.isEnableAlphaTest = isEnable;
    return TB_S_OK;
}

/*!
 * アルファテスト有効設定
 * @author teco
 * @since 2011.06.11
 */
TbResult TbRenderDevice::SetAlphaTestFunc( TbCmpFunc func )
{
    D3DCMPFUNC funcD3D;
    if( TbCmpFuncToD3DCMPFUNC( funcD3D , func ) == TB_S_OK ){
        m_AlphaTestState.alphaTestFunc = func;
        m_Depend.GetDeviceDX9()->SetRenderState( D3DRS_ALPHAFUNC , funcD3D );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * アルファ参照値
 * @author teco
 * @since 2011.06.11
 */
TbResult TbRenderDevice::SetAlphaTestRef( TbUint8 ref )
{
    m_Depend.GetDeviceDX9()->SetRenderState( D3DRS_ALPHAREF , ref );
    m_AlphaTestState.alphaTestRef = ref;
    return TB_S_OK;
}

/*!
 * Depth Stencil State設定
 * @author teco
 * @since 2011.06.11
 */
TbResult TbRenderDevice::SetDepthStencilState(const TbDepthStencilState* state)
{
    if (!state) {
        return SetDepthStencilState(&m_DefaultDepthStencilState);
    }
    const TbDepthStencilState::Param& param = state->GetParam();

    // enable depth test
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_ZENABLE, param.isEnableDepthTest);

    // depth write mask
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_ZWRITEENABLE, param.depthWriteMask == TB_DEPTH_MASK_ALL);

    // depth test func
    {
        D3DCMPFUNC funcD3D;
        if (TbCmpFuncToD3DCMPFUNC(funcD3D, param.depthTestFunc)) {
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_ZFUNC, funcD3D);
        }else{
            return TB_E_FAIL;
        }
    }

    // enable stencil test
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILENABLE, param.isEnableStencilTest);

    // stencil read mask
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILMASK, param.stencilReadMask);

    // stencil write mask
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILWRITEMASK, param.stencilWriteMask);

    // stencil ref
    m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILREF, param.stencilTestRef);
    
    // stencil func
    {
        D3DCMPFUNC funcD3D;
        if (TbCmpFuncToD3DCMPFUNC(funcD3D, param.stencilTestFunc) == TB_S_OK) {
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILFUNC, funcD3D);
        }
        else {
            return TB_E_FAIL;
        }
    }

    // stencil op
    {
        D3DSTENCILOP failOpD3D;
        D3DSTENCILOP zfailOpD3D;
        D3DSTENCILOP passOpD3D;
        if (TbStencilOpToD3DSTENCILOP(failOpD3D, param.stencilOp.failOp) == TB_S_OK &&
            TbStencilOpToD3DSTENCILOP(zfailOpD3D, param.stencilOp.zfailOp) == TB_S_OK &&
            TbStencilOpToD3DSTENCILOP(passOpD3D, param.stencilOp.passOp) == TB_S_OK)
        {
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILFAIL, failOpD3D);
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILZFAIL, zfailOpD3D);
            m_Depend.GetDeviceDX9()->SetRenderState(D3DRS_STENCILPASS, passOpD3D);
        } else {
            return TB_E_FAIL;
        }
    }
    m_DepthStencilState = state;
    return TB_S_OK;
}

/*!
 * フルスクリーン設定
 * @author teco
 */
TbBool TbRenderDevice::SetFullscreen( TbBool isFullscreen )
{
    if( TB_BOOL_NOT_EQUAL( isFullscreen , m_BitArray.Test(FLAG_FULLSCREEN) ) ) 
    {
        m_Depend.setFullscreen(isFullscreen);
        initCommon();
        SetDepthStencilState(m_DepthStencilState);
        SetBlendState(m_BlendState);
        SetRasterizerState(m_RasterizerState);
        m_BitArray.SetBit(FLAG_FULLSCREEN,isFullscreen);
    }
    return TB_TRUE;
}

// 固定機能シェーダ
TbBool TbRenderDevice::isActiveFixedShader() const
{
    return nullptr == m_ShaderManager.GetCurrentProgram();
}

/*!
 * 状態をダンプ
 * @author teco
 * @since 2013.03.14
 */
void TbRenderDevice::Dump()
{
    
}

}

#endif