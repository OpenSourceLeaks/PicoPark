/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_index_buffer.h"

#if TB_3DAPI_IS_GL

#include "graphics/render/3dapi/tb_render_device.h"

namespace toybox
{

// 依存コード

/*!
 * フォーマットをGLenumへ変換
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */
TbResult TbIndexBufferDepend::ConvertFormatToGLenum( GLenum& dst , const TbIndexBufferFormat format )
{
    static const GLenum TABLE[] = 
    {
        GL_UNSIGNED_SHORT ,
        GL_UNSIGNED_INT ,
    };
    if( 0 <= format && format < TB_INDEX_BUFFER_FORMAT_MAX ){
        dst = TABLE[ format ];
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * コンストラクタ
 */
TB_INLINE TbIndexBufferDepend::TbIndexBufferDepend( const TbSint32 size )
    : m_VboId( 0 )
    , m_Buffer( nullptr )
    , m_LockBufferSize( 0U )
{
    m_Buffer = TB_NEW TbUint8[ size ];
    glGenBuffers(1, &m_VboId);
}

/*!
 * デストラクタ
 */
TB_INLINE TbIndexBufferDepend::~TbIndexBufferDepend()
{
    glDeleteBuffers(1, &m_VboId );
    TB_SAFE_DELETE_ARRAY( m_Buffer );
}

//

/*!
 * コンストラクタ
 * @param format 頂点フォーマット
 * @param size バッファサイズ
 * @parma device　所属デバイスインデックス
 * @author Miyake Shunsuke
 * @since 2011.05.04
 */
TbIndexBuffer::TbIndexBuffer( const TbIndexBufferFormat format , const TbSint32 size )
    : m_Size(size)
    , m_Format( format )
    , m_Depend( size )
{
    TB_UNUSE( format );
}

/*!
 * デストラクタ
 */
TbIndexBuffer::~TbIndexBuffer()
{
}

/*!
 * ロック
 */
TbBool TbIndexBuffer::Write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    if( src ){
        void* dst = nullptr;
        Lock( offset , size , &dst );
        TbMemCopy( dst , src , size ); 
        Unlock();
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ロック
 */
TbBool TbIndexBuffer::Lock( const TbUint32 offset , const TbUint32 size , void** dst )
{
    if( dst ){
        *dst = &( m_Depend.GetBuffer()[ offset ] );
        m_Depend.m_LockBufferSize = size;
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ロック解除
 */
void TbIndexBuffer::Unlock()
{
    if( m_Depend.m_LockBufferSize > 0 ){
        // DrawIndexedPrimitiveの時に、必要なバインドはされるので現在何がバインドされているかを気にする必要はない
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , m_Depend.GetVboId() );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER , m_Depend.m_LockBufferSize , m_Depend.GetBuffer() , GL_STATIC_DRAW);
        m_Depend.m_LockBufferSize = 0U;
    }
}

}

#endif