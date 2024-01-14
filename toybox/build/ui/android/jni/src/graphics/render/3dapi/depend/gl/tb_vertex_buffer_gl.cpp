/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"

#if TB_3DAPI_IS_GL

#include "graphics/render/3dapi/tb_vertex_format.h"
#include "graphics/render/3dapi/tb_render_device.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 */
TB_INLINE TbVertexBufferDepend::TbVertexBufferDepend( const TbSint32 size )
    : m_VboId( 0 )
{
    glGenBuffers(1, &m_VboId);
}

/*!
 * デストラクタ
 */
TB_INLINE TbVertexBufferDepend::~TbVertexBufferDepend()
{
    glDeleteBuffers(1, &m_VboId );
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
    , m_Depend( size )
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
        glBindBuffer( GL_ARRAY_BUFFER , m_Depend.GetVboId() );
        glBufferData( GL_ARRAY_BUFFER , 
                      size , 
                      src , 
                      GL_STATIC_DRAW);
        return TB_TRUE;
    }
    return TB_FALSE;
}

}

#endif