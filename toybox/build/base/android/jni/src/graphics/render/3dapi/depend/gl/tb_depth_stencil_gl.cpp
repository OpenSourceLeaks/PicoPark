/*!
 * レンダーターゲット
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"

#if (TB_3DAPI_IS_GL)

#include "graphics/render/3dapi/tb_texture.h"

namespace toybox
{

/*!
 * Rbo生成
 * @author Miyake Shunsuke
 * @since 2011.06.02
 */
void createRbo( GLuint& rboId , TbUint32 width , TbUint32 height )
{
    TbGlGenRenderbuffers( 1 , &rboId );
    TbGlBindRenderbuffer( GL_RENDERBUFFER , rboId );
#if (TB_PLATFORM_IS_WIN)
    TbGlRenderbufferStorage( GL_RENDERBUFFER , GL_DEPTH_COMPONENT, width , height );
#else
    TbGlRenderbufferStorage( GL_RENDERBUFFER , GL_DEPTH_COMPONENT16, width , height );
#endif
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbDepthStencil::TbDepthStencil( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_DEPTH_STENCIL_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    createRbo( m_Depend.rboId , width , height );
}

/*!
 * コンストラクタ( テクスチャから生成 )
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbDepthStencil::TbDepthStencil( TbTexture* texture )
    : m_Type( TB_DEPTH_STENCIL_TYPE_TEXTURE )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    if( texture ){
        m_Depend.textureId = texture->GetDepend().GetTextureId();
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * コンストラクタ( ビューから生成 )
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbDepthStencil::TbDepthStencil( TbRenderView* view )
    : m_Type( TB_DEPTH_STENCIL_TYPE_VIEW )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    if( view ){
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.05.24
 */
TbDepthStencil::~TbDepthStencil()
{
    switch( m_Type ){
    case TB_DEPTH_STENCIL_TYPE_BUFFER:
        glDeleteRenderbuffers( 1 , &m_Depend.rboId ); 
        break;
    case TB_DEPTH_STENCIL_TYPE_TEXTURE:
        break;
    case TB_DEPTH_STENCIL_TYPE_VIEW:
        break;
    }
}
    
}

#endif
