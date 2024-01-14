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
 * フォーマットからバイト数取得
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */
TbResult TbIndexBufferDepend::GetByteCountFromFormat( TbUint32& dst , const TbIndexBufferFormat format )
{
    static const TbUint32 TABLE[] = 
    {
        2 ,
        4 ,
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
TB_INLINE TbIndexBufferDepend::TbIndexBufferDepend( TbIndexBufferFormat format , const TbSint32 size )
{
    TbUint32 byteCount = 0;
    GetByteCountFromFormat(byteCount,format);
    GX2UTCreateIndexBuffer(&m_Buffer,byteCount,size/byteCount);
}

/*!
 * デストラクタ
 */
TB_INLINE TbIndexBufferDepend::~TbIndexBufferDepend()
{
    GX2RDestroyBuffer(&m_Buffer);
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
        TbUint8* buf = reinterpret_cast<TbUint8*>(GX2RLockBuffer(&m_Depend.m_Buffer));
        *dst = buf;
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
        GX2RUnlockBuffer(&m_Depend.m_Buffer);
    }
}

}

#endif