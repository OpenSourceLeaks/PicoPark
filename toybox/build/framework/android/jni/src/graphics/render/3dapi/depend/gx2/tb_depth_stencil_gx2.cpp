/*!
 * レンダーターゲット
 * @author teco
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_depth_stencil.h"

#if (TB_3DAPI_IS_GX2)

#include "graphics/render/3dapi/tb_texture.h"

namespace toybox
{

/*!
 * Rbo生成
 * @author teco
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
 * @author teco
 */
TbDepthStencil::TbDepthStencil( TbUint32 width , TbUint32 height , TbColorFormat colorFormat )
    : m_Type( TB_DEPTH_STENCIL_TYPE_BUFFER )
    , m_BitArray()
    , m_Depend()
{
    GX2SurfaceFormat gx2format;
    if( TbColorFormatToGX2SurfaceFormat(&gx2format,colorFormat) ){
        GX2InitDepthBuffer(&m_Depend.depthBuffer,width,height,gx2format,GX2_AA_MODE_1X );
    }
}

/*!
 * コンストラクタ( テクスチャから生成 )
 * @author teco
 */
TbDepthStencil::TbDepthStencil( TbTexture* texture )
    : m_Type( TB_DEPTH_STENCIL_TYPE_TEXTURE )
    , m_BitArray()
    , m_Depend()
{
    TbMemClear( &m_Depend , sizeof( m_Depend ) );
    if( texture ){
        GX2SurfaceFormat gx2format;
        if( TbColorFormatToGX2SurfaceFormat(&gx2format,colorFormat) ){
            GX2InitDepthBuffer(&m_Depend.depthBuffer,
                               width,
                               height,
                               gx2format,
                               GX2_AA_MODE_1X );
        }
        m_BitArray.SetBit( FLAG_VALID );
    }
}

/*!
 * コンストラクタ( ビューから生成 )
 * @author teco
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
 * @author teco
 */
TbDepthStencil::~TbDepthStencil()
{
}
    
}

#endif
