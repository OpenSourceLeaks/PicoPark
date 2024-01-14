/*!
 * 頂点バッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_buffer.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_vertex_format.h"

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
TB_INLINE TbVertexBufferDepend::TbVertexBufferDepend( const TbVertexFormat* format , TbSint32 size )
    : TbLostResourceDx9( &TbRenderDevice::GetInstance().GetDepend().GetLostResourceRoot() )
    , m_VertexBuffer( nullptr )
    , m_Size( size )
    , m_FVF( format ? format->GetDepend().GetFVF() : 0 )
    , m_BackupBuffer( nullptr )
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT result = device.GetDepend().GetDeviceDX9()->CreateVertexBuffer( m_Size , D3DUSAGE_WRITEONLY , m_FVF ,
                                                         D3DPOOL_DEFAULT , &m_VertexBuffer , nullptr );
    if( FAILED(result) ) {
        TbPrintf("CreateVertexBuffer error (%d)\n",result);
    }
    m_BackupBuffer = TB_NEW TbUint8[ m_Size ];
    TbMemClear( m_BackupBuffer , m_Size );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
TB_INLINE TbVertexBufferDepend::~TbVertexBufferDepend()
{
    TB_SAFE_RELEASE( m_VertexBuffer );
    TB_SAFE_DELETE_ARRAY( m_BackupBuffer );
}

/*!
 * 解放
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbVertexBufferDepend::Release()
{
    TbSint32 refCount = m_VertexBuffer->Release();
    if( refCount > 0 ){
        // ありえない
        int a = 0;
        TB_ASSERT(0);
    }
    m_VertexBuffer = nullptr;
}

/*!
 * 再構築
 * @author Miyake Shunsuke
 * @since 2011.01.15
 */
void TbVertexBufferDepend::Recover()
{
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    HRESULT hr = device.GetDepend().GetDeviceDX9()->CreateVertexBuffer( m_Size , D3DUSAGE_WRITEONLY , m_FVF ,
                                                             D3DPOOL_DEFAULT , &m_VertexBuffer , nullptr );
    if( FAILED(hr) ) {
        TbPrintf("CreateVertexBuffer(Recover) error (%d)\n",hr);
    }
    write( 0 , m_Size , m_BackupBuffer );
    hr = 0;
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
void TbVertexBufferDepend::write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    if( m_VertexBuffer ){
        void* tempBuffer;
        m_VertexBuffer->Lock( offset , size , &tempBuffer , 0);
        TbMemCopy( tempBuffer , src , size );
        if( m_BackupBuffer != src ){
            TbMemCopy( m_BackupBuffer + offset , src , size );
        }
        m_VertexBuffer->Unlock();
    }
}

//

/*!
 * コンストラクタ
 */
TbVertexBuffer::TbVertexBuffer( const TbVertexFormat* format , const TbSint32 size )
    : m_Size(size)
    , m_Depend( format , size )
{
}
    
/*!
 * コンストラクタ
 */
TbVertexBuffer::TbVertexBuffer( TbUint32 vertexCount , TbUint32 sizePerVertex )
    : m_Size( sizePerVertex*vertexCount )
    , m_Depend( nullptr , sizePerVertex*vertexCount )
{
}

/*!
 * デストラクタ
 */
TbVertexBuffer::~TbVertexBuffer()
{
}

/*!
 *　書き込み
 * @author Miyake Shunsuke
 * @since 2011.01.29
 */
TbBool TbVertexBuffer::Write( const TbUint32 size , const void* src )
{
    m_Depend.write( 0 , size , src );
    return TB_TRUE;
}

}

#endif