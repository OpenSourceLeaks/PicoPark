/*!
 * 深度ステンシル(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_depth_stencil.h"

#if (TB_3DAPI_IS_S3D)

#include "graphics/3dapi/tb_texture.h"

namespace toybox
{

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
}
    
}

#endif
