/*!
 * レンダービュークラス (Android版は複数ビューには非対応)
 * @author Miyake Shunsuke
 * @since 2012.07.05
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_view.h"

#if TB_3DAPI_IS_GL

#include "graphics/render/3dapi/tb_render_target.h"
#include "graphics/window/tb_window.h"

namespace toybox
{

/*!
 * レンダーターゲット取得
 * @author Miyake Shunsuke
 * @since 2010.11.12
 */
TbRenderTarget* TbRenderView::GetRenderTarget( TbUint32 backBufferIndex )
{
    TB_ASSERT(backBufferIndex==0); // GLではビュー用レンダリングターゲットは一つだけ
    return m_Depend.m_RenderTarget;
}

/*!
 * フロントバッファの横幅取得
 * @author Miyake Shunsuke
 * @since 2011.11.01
 */
TbUint32 TbRenderView::GetFrontBufferWidth()
{
   return GetWindow()->GetWidth();
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
    return GetWindow()->GetHeight();
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
    TB_UNUSE(readRect);

    if( !readBuffer ){
        return TB_E_FAIL;
    }

    TbUint32 pitch = GetFrontBufferPitch();
    TbUint32 width = GetFrontBufferWidth();
    TbUint32 height = GetFrontBufferHeight();

    TbUint32 writeBufferSize = pitch * height;
    if( readBufferSize < writeBufferSize ){
        return TB_E_FAIL;
    }
    if( isDownToUp ){
        *isDownToUp = TB_TRUE;
    }

    // バックバッファから取得
    glReadPixels( 0 , 0 , width , height, GL_RGBA , GL_UNSIGNED_BYTE , readBuffer );
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
    return TB_E_FAIL;
}

}

#endif
