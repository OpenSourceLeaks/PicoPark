/*!
 * 頂点バッファ(Stage3D版)
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_vertex_buffer.h"

#if TB_3DAPI_IS_S3D

#include "graphics/3dapi/tb_vertex_format.h"
#include "graphics/3dapi/tb_render_device.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbVertexBufferDepend::TbVertexBufferDepend( const TbVertexFormat* format , const TbUint32 size )
    : m_SizePerVertex(0U)
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    AS3::ui::flash::display3D::Context3D context = TbRenderDevice::GetInstance().GetDepend().GetContext();
    
    TbSizeT elementCount = format->GetDepend().GetElementCount();
    for( TbSizeT i = 0; i < elementCount; ++i ) {
        const TbVertexElementS3d* element = format->GetDepend().GetElement(i);
        if( TB_VERIFY(element && 0 == element->stream ) ) {
            m_SizePerVertex += (element->data32Count*4);
        }else{
            m_SizePerVertex = 0;
            return;
        }
    }
    size_t vertexCount = size/m_SizePerVertex;
    size_t data32Count = m_SizePerVertex/4;
    m_VertexBuffer = context->createVertexBuffer(vertexCount,data32Count);
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TB_INLINE TbVertexBufferDepend::TbVertexBufferDepend( TbUint32 vertexCount , TbUint32 sizePerVertex )
    : m_SizePerVertex(sizePerVertex)
{
    AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
    m_VertexBuffer = context->createVertexBuffer(vertexCount,sizePerVertex/4);
}

//

/*!
 * コンストラクタ
 * @param format 頂点フォーマット
 * @param size バッファサイズ
 * @parma device　所属デバイス
 * @author Miyake Shunsuke
 * @since 2011.05.04
 */
TbVertexBuffer::TbVertexBuffer( const TbVertexFormat* format , const TbSint32 size )
    : m_Size(size)
    , m_Depend( format , size )
{
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbVertexBuffer::TbVertexBuffer( TbUint32 vertexCount , TbUint32 sizePerVertex )
    : m_Size( sizePerVertex*vertexCount )
    , m_Depend( sizePerVertex , vertexCount )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbVertexBuffer::~TbVertexBuffer()
{
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbBool TbVertexBuffer::Write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    if( src ){
        m_Depend.GetVertexBuffer()->uploadFromByteArray(AS3::ui::internal::get_ram(), 
                                                        (int)src, 
                                                        0, 
                                                        size / m_Depend.m_SizePerVertex , 
                                                        const_cast<void*>(src));
        return TB_TRUE;
    }
    return TB_FALSE;
}

}

#endif