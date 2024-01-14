/*!
 * インデックスバッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_index_buffer.h"

#if TB_3DAPI_IS_GFX

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 */
TbIndexBufferDepend::TbIndexBufferDepend( TbIndexBufferFormat format , const TbSint32 size )
    : m_Format(nn::gfx::IndexFormat_Uint16)
{
    nn::gfx::IndexFormat FORMAT_TABLE[] = 
    {
        nn::gfx::IndexFormat_Uint16 ,
        nn::gfx::IndexFormat_Uint32 ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(FORMAT_TABLE)==TB_INDEX_BUFFER_FORMAT_MAX);
    m_Format = FORMAT_TABLE[format];

    TbGpuBufferGfx::InitParam param;
    param.accessFlag = TB_TRUE;
    param.bufferSize = size;
    param.accessFlag = nn::gfx::GpuAccess_IndexBuffer;
    m_Buffer.Intialize(param);
}

/*!
 * デストラクタ
 */
TbIndexBufferDepend::~TbIndexBufferDepend()
{
    m_Buffer.Finalize();
}

//

/*!
 * コンストラクタ
 * @param format 頂点フォーマット
 * @param size バッファサイズ
 * @parma device　所属デバイスインデックス
 * @author teco
 */
TbIndexBuffer::TbIndexBuffer( const TbIndexBufferFormat format , const TbSint32 size )
    : m_Size(size)
    , m_Format( format )
    , m_Depend(format , size )
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
        m_Depend.GetBuffer()->Write(src,offset,size);
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
        TbUint8* buf = reinterpret_cast<TbUint8*>(m_Depend.GetBuffer()->Map());
        *dst = &buf[offset];
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * ロック解除
 */
void TbIndexBuffer::Unlock()
{
    m_Depend.GetBuffer()->Unmap();
}

}

#endif