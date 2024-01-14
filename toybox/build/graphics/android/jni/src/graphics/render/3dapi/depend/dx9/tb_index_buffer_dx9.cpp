/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_index_buffer.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_render_view.h"

namespace toybox
{

// 依存コード

/*!
 * フォーマットをD3DFORMATへ変換
 * @param format フォーマット
 * @author Miyake Shunsuke
 * @since 2011.06.06
 */
TbResult TbIndexBufferDepend::ConvertFormatToD3DFORMAT( D3DFORMAT& dst , const TbIndexBufferFormat format )
{
   static const D3DFORMAT TABLE[] = 
    {
        D3DFMT_INDEX16 ,
        D3DFMT_INDEX32 ,
    };
    if( 0 <= format && format < TB_INDEX_BUFFER_FORMAT_MAX )
    {
        dst = TABLE[ format ];
        return TB_S_OK;
    }
    return D3DFMT_UNKNOWN;
 }

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
TB_INLINE TbIndexBufferDepend::TbIndexBufferDepend( const TbIndexBufferFormat format , const TbSint32 size )
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_IndexBufferDx9( nullptr )
    , m_Format( D3DFMT_UNKNOWN )
    , m_Size( size )
    , m_BackupBuffer( nullptr )
{
    TB_ASSERT( 0 <= format && format < TB_INDEX_BUFFER_FORMAT_MAX );
    ConvertFormatToD3DFORMAT( m_Format , format );
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.GetDepend().GetDeviceDX9()->CreateIndexBuffer( m_Size , 0 , m_Format ,
                                                             D3DPOOL_DEFAULT , &m_IndexBufferDx9 , nullptr );
    m_BackupBuffer = TB_NEW TbUint8[ m_Size ];
    TbMemClear( m_BackupBuffer , m_Size );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
TB_INLINE TbIndexBufferDepend::~TbIndexBufferDepend()
{
    TB_SAFE_RELEASE( m_IndexBufferDx9 );
    TB_SAFE_DELETE_ARRAY( m_BackupBuffer );
}

/*!
 * 解放
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbIndexBufferDepend::Release()
{
    TB_SAFE_RELEASE( m_IndexBufferDx9 );
}

/*!
 * 再構築
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbIndexBufferDepend::Recover()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT hr = device.GetDepend().GetDeviceDX9()->CreateIndexBuffer( m_Size , 0 , m_Format ,
                                                                       D3DPOOL_DEFAULT , &m_IndexBufferDx9 , nullptr );
    write( 0 , m_Size , m_BackupBuffer );
    TB_UNUSE( hr );
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
void TbIndexBufferDepend::write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    void* tempBuffer;
    m_IndexBufferDx9->Lock( offset , size , &tempBuffer , 0);
    TbMemCopy( tempBuffer , src , size );
    // バックアップバッファにも書き込む
    if( m_BackupBuffer != src ){
        TbMemCopy( m_BackupBuffer + offset , src , size );
    }
    m_IndexBufferDx9->Unlock();
}

//

/*!
 * コンストラクタ
 */
TbIndexBuffer::TbIndexBuffer( const TbIndexBufferFormat format , const TbSint32 size )
    : m_Size(size)
    , m_Format( format )
    , m_Depend( format , size )
{
}

/*!
 * デストラクタ
 */
TbIndexBuffer::~TbIndexBuffer()
{
}

/*!
 *　書き込み
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
TbBool TbIndexBuffer::Write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    m_Depend.write( offset , size , src );
    return TB_TRUE;
}

/*!
 * ロック
 */
TbBool TbIndexBuffer::Lock( const TbUint32 offset , const TbUint32 size , void** dst )
{
    m_Depend.GetRawData()->Lock( offset , size , dst , 0);
    return TB_TRUE;
}

/*!
 * ロック解除
 */
void TbIndexBuffer::Unlock()
{
    m_Depend.GetRawData()->Unlock();
}

}

#endif