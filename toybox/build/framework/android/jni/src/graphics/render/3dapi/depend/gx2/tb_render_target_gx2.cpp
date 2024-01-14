/*!
 * レンダーターゲット
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_render_target.h"

#if TB_3DAPI_IS_GX2

#include "graphics/window/tb_window.h"
#include "graphics/render/3dapi/tb_render_view.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "graphics/render/3dapi/depend/gl/tb_render_util_gl.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbRenderTarget::TbRenderTarget( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_RENDER_TARGET_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend()
{
    m_Depend.width = width;
    m_Depend.height = height;
    
}

/*!
 * コンストラクタ(Viewから生成)
 * @author Miyake Shunsuke
 */
TbRenderTarget::TbRenderTarget( TbRenderView* view , TbUint32 backBufferIndex )
    : m_Type( TB_RENDER_TARGET_TYPE_VIEW )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    m_Depend.view = view;
    m_Depend.width = view->GetWindow()->GetWidth();
    m_Depend.height = view->GetWindow()->GetHeight();
    if( view ){
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * コンストラクタ( テクスチャから生成 )
 * @author Miyake Shunsuke
 */
TbRenderTarget::TbRenderTarget( TbTexture* texture )
    : m_Type( TB_RENDER_TARGET_TYPE_TEXTURE )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    m_Depend.texture = texture;
    m_Depend.width = texture->GetWidth(0);
    m_Depend.height = texture->GetHeight(0);
    if( texture ){
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbRenderTarget::~TbRenderTarget()
{
    switch( m_Type ){
    case TB_RENDER_TARGET_TYPE_BUFFER:
        break;
    case TB_RENDER_TARGET_TYPE_VIEW:
        break;
    case TB_RENDER_TARGET_TYPE_TEXTURE:
        break;
    }
}

/*!
 * 解像度横幅取得
 * @author Miyake Shunsuke
 */
TbUint32 TbRenderTarget::GetWidth() const
{
    return m_Depend.width;
}

/*!
 * 解像度縦幅取得
 * @author Miyake Shunsuke
 * @since 2011.06.03
 */
TbUint32 TbRenderTarget::GetHeight() const
{
    return m_Depend.height;
}

/*!
 * ピッチ幅取得
 * @author Miyake Shunsuke
 * @since 2011.10.30
 */
TbUint32 TbRenderTarget::GetPitch()
{
    TbUint32 result = 0U;
    return result;
}

/*!
 * フォーマット取得
 * @author Miyake Shunsuke
 */
TbColorFormat TbRenderTarget::GetFormat() const
{
    TbColorFormat result = TB_COLOR_FORMAT_UNKNOWN;
    switch( m_Type ){
    case TB_RENDER_TARGET_TYPE_BUFFER:
        break;
    case TB_RENDER_TARGET_TYPE_VIEW:
        result = TB_COLOR_FORMAT_A8R8G8B8;
        break;
    case TB_RENDER_TARGET_TYPE_TEXTURE:
        result = m_Depend.texture->GetFormat(0);
        break;
    }
    return result;
}

}

#endif
