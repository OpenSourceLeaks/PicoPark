/*!
 * レンダーデバイスの共通処理
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_view.h"
#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

/*!
 * 初期化(簡易版)
 * @author teco
 */
TbBool TbRenderDevice::Initialize( TbWindow* focusWindow , bool isEnableDepthStencil , bool isFullscreen ) 
{
    TbRenderViewParameter param;
    TbMemClear( &param , sizeof( param ) );
    param.backBufferHeight = 0;
    param.backBufferWidth = 0;
    param.backBufferFormat = TB_COLOR_FORMAT_UNKNOWN;
    param.backBufferNum = 0;
    param.enableCreateDepthStencil = isEnableDepthStencil;
    param.autoDepthStencilsFormat = TB_COLOR_FORMAT_D24S8;
    param.isFullscreen = isFullscreen;
    param.multiSampleType = TB_MULTISAMPLE_NONE;
    return Initialize( 0 , focusWindow , 0 , param );
}

/*!
 * ビュー取得
 * @param index インデックス
 * @author teco
 */
TbRenderView* TbRenderDevice::GetView( TbSint32 index )
{
    if( 0 <= index && index < m_ViewList.GetCount() ){
        return m_ViewList[ index ];
    }
    return nullptr;
}

/*!
 * 共通初期化
 * @param index インデックス
 * @author teco
 */
void TbRenderDevice::initCommon()
{
#if TB_IS_ENABLE_FIXED_SHADER
    GetFixedShader().SetTextureAlphaOp( 0 , TB_TEXTURE_OP_MODULATE );
#endif
}

// 
/*!
 * ステート系初期化
 * @param index インデックス
 * @author teco
 */
void TbRenderDevice::initDefaultState(const TbRenderViewParameter& viewParam)
{
    TB_RESQUE(m_RenderTarget[0]);
    m_RenderStatePreset.Initialize();

    // 深度ステンシルステート
    {
        TbDepthStencilState::Param param;
        if (viewParam.enableCreateDepthStencil) {
            param.isEnableDepthTest = TB_TRUE;
            param.depthWriteMask = TB_DEPTH_MASK_ALL;
        }
        m_DefaultDepthStencilState.Initialize(param);
        SetDepthStencilState(&m_DefaultDepthStencilState);
    }
    // ブレンドステート
    {
        TbBlendState::Param param;
        m_DefaultBlendState.Initialize(param);
        SetBlendState(&m_DefaultBlendState);
    }

    // ビューポートシザーステート
    {
        TbViewportScissorState::Param param;
        param.viewPortRect.width = m_RenderTarget[0]->GetWidth();
        param.viewPortRect.height = m_RenderTarget[0]->GetHeight();
        m_DefaultViewportScissorState.Initialize(param);
        SetViewportScissorState(&m_DefaultViewportScissorState);
    }

    // ラスタライザステート
    {
        TbRasterizerState::Param param;
        m_DefaultRasterizerState.Initialize(param);
        SetRasterizerState(&m_DefaultRasterizerState);
    }
    
    //
    {
        TbTextureSamplerParam param;
        m_DefaultTextureSampler.Initialize(param);
    }
}

#if 0
/*!
 * 深度ステンシルステートの初期値を設定
 * @author Miyake Shunsuke
 */
void TbRenderDevice::initDepthStencilState()
{
    m_DepthStencilState.isEnableDepthTest = TB_FALSE;
    m_DepthStencilState.depthWriteMask = TB_DEPTH_MASK_ALL;
    m_DepthStencilState.depthTestFunc = TB_CMP_FUNC_LESS;
    m_DepthStencilState.isEnableStencilTest = TB_FALSE;
    m_DepthStencilState.stencilTestFunc = TB_CMP_FUNC_ALWAYS;
    m_DepthStencilState.stencilReadMask = 0xFF;
    m_DepthStencilState.stencilWriteMask = 0xFF;
    m_DepthStencilState.stencilTestRef = 0U;
    m_DepthStencilState.isEnableDepthTest = TB_FALSE;
#if 0
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_FRONT ].failOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_FRONT ].zfailOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_FRONT ].passOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_BACK ].failOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_BACK ].zfailOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp[ TB_RENDER_FACE_BACK ].passOp = TB_STENCIL_OP_KEEP;
#else
    m_DepthStencilState.stencilOp.failOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp.zfailOp = TB_STENCIL_OP_KEEP;
    m_DepthStencilState.stencilOp.passOp = TB_STENCIL_OP_KEEP;
#endif
}
#endif

/*!
 * アルファテストステートの初期化
 * @author Miyake Shunsuke
 * @since 2011.06.10
 */
void TbRenderDevice::initAlphaTestState()
{
    m_AlphaTestState.isEnableAlphaTest = TB_FALSE;
    m_AlphaTestState.alphaTestFunc= TB_CMP_FUNC_GREATER;
    m_AlphaTestState.alphaTestRef = 0;
//    SetEnableAlphaTest( m_AlphaTestState.isEnableAlphaTest );
//    SetAlphaTestFunc( m_AlphaTestState.alphaTestFunc );
//    SetAlphaTestRef( m_AlphaTestState.alphaTestRef );
}

/*!
 * 全ビューを表示
 * @author teco
 */
void TbRenderDevice::PresentAll()
{
    TbUint32 viewCount = m_ViewList.GetCount();
    for( TbUint32 i = 0; i < viewCount; ++i ) {
        m_ViewList[i]->Present();
    }
}

/*!
 * 表示用矩形領域取得
 * @author teco
 */
TbRectS32 TbRenderDevice::GetPresentRect( TbRenderPresentType type , const TbRenderTarget* rt , const TbWindow* window )
{
    TbRectS32 result;
    switch( type )
    {
    case TB_RENDER_PRESENT_BUFFER:
        if( TB_VERIFY(rt) ){
            result.width = rt->GetWidth();
            result.height = rt->GetHeight();
        }
        break;
    case TB_RENDER_PRESENT_WINDOW:
        {
            if( TB_VERIFY(window) ){
                result.width = window->GetWidth();
                result.height = window->GetHeight();
            }
        }
        break;
    case TB_RENDER_PRESENT_WINDOW_KEEP_ASPECT:
        if( TB_VERIFY(rt && window) ){
            TbFloat32 rtWidth = static_cast<TbFloat32>(rt->GetWidth());
            TbFloat32 rtHeight = static_cast<TbFloat32>(rt->GetHeight());
            TbFloat32 rtAspect = rtWidth / rtHeight;
                
            TbFloat32 wndWidth = static_cast<TbFloat32>(window->GetWidth());
            TbFloat32 wndHeight = static_cast<TbFloat32>(window->GetHeight());
            TbFloat32 wndAspect = wndWidth / wndHeight;
            if( wndAspect > rtAspect ) {
                // 高さをウィンドウサイズに合わせる
                TbFloat32 scale = wndHeight / rtHeight;
                result.width = static_cast<TbSint32>(scale * rtWidth);
                result.height = window->GetHeight();
                result.x = static_cast<TbSint32>( wndWidth - result.width ) / 2;
            }else{
                TbFloat32 scale = wndWidth / rtWidth;
                result.height = static_cast<TbSint32>(scale * rtHeight);
                result.width = window->GetWidth();
                result.y = static_cast<TbSint32>( wndHeight - result.height ) / 2;
            }
        }
        break;
    }
    return result;
}

}
