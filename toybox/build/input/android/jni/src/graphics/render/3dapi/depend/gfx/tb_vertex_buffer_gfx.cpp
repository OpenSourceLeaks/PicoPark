/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"

#if TB_3DAPI_IS_GFX

#include "graphics/render/3dapi/tb_vertex_format.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 */
TbVertexBufferDepend::TbVertexBufferDepend( const TbVertexFormat* format , const TbSint32 size )
{
    TbGpuBufferGfx::InitParam param;
    param.cpuAccesible = TB_TRUE;
    param.bufferSize = size;
    param.accessFlag = nn::gfx::GpuAccess_VertexBuffer;
    m_Buffer.Intialize(param);
}

/*!
 * デストラクタ
 */
TbVertexBufferDepend::~TbVertexBufferDepend()
{
    m_Buffer.Finalize();
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
        m_Depend.GetBuffer()->Write(src, 0, size);
    }
    return TB_FALSE;
}

}

#endif