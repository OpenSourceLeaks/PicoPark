/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"

#if TB_3DAPI_IS_GX2

#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/tb_render_device.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 */
TB_INLINE TbVertexBufferDepend::TbVertexBufferDepend( const TbVertexFormat* format , const TbSint32 size )
    : m_ElementSize(0)
{
    TbSizeT elementCount = format->GetDepend().GetElementCount();
    for( TbSizeT i = 0; i < elementCount; ++i ) {
        const TbVertexElementGX2* element = format->GetDepend().GetElement(i);
        if( TB_VERIFY(element && 0 == element->stream ) ) {
            m_ElementSize += (element->size);
        }else{
            m_ElementSize = 0;
            return;
        }
    }
    size_t vertexCount = size/m_ElementSize;
    GX2UTCreateVertexBuffer(&m_Buffer,m_ElementSize,vertexCount);
}

/*!
 * デストラクタ
 */
TB_INLINE TbVertexBufferDepend::~TbVertexBufferDepend()
{
    GX2RDestroyBuffer(&m_Buffer);
}

//

/*!
 * コンストラクタ
 * @param format 頂点フォーマット
 * @param size バッファサイズ
 * @parma device　所属デバイス
 */
TbVertexBuffer::TbVertexBuffer( const TbVertexFormat* format , const TbSint32 size )
    : m_Size(size)
    , m_Depend( format , size )
{
}

/*!
 * デストラクタ
 */
TbVertexBuffer::~TbVertexBuffer()
{
}

/*!
 * 書き込み
 */
TbBool TbVertexBuffer::Write( const TbUint32 size , const void* src )
{
    if( src && 0 < size ){
        void* mem = GX2RLockBuffer(&m_Depend.m_Buffer)
        TbMemCopy( mem , src , size );
        GX2RUnlockBuffer( &m_Depend.m_Buffer );
    }
    return TB_FALSE;
}

}

#endif