/*!
 * インデックスバッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_constant_buffer.h"

#if TB_3DAPI_IS_GFX

namespace toybox
{

// 依存コード

/*!
 * コンストラクタ
 */
TbConstantBufferDepend::TbConstantBufferDepend( const TbSint32 size )
{
    TbGpuBufferGfx::InitParam param;
    param.cpuAccesible = TB_TRUE;
    param.bufferSize = size;
    param.accessFlag = nn::gfx::GpuAccess_ConstantBuffer;
    m_Buffer.Intialize(param);
#if TB_CONSTANT_BUFFER_ENABLE_DESCRIPTOR
    nn::gfx::GpuAddress address;
    m_Buffer.GetGpuAddress(&address);
    m_DescriptorPool.InitializeAsConstantBuffer(address,m_Buffer.GetSize());
#endif
}

/*!
 * デストラクタ
 */
TbConstantBufferDepend::~TbConstantBufferDepend()
{
    m_Buffer.Finalize();
}

//

/*!
 * コンストラクタ
 * @param size バッファサイズ
 * @author teco
 */
TbConstantBuffer::TbConstantBuffer( const TbSizeT size )
    : m_Size(size)
    , m_Depend(size )
{
}

/*!
 * デストラクタ
 */
TbConstantBuffer::~TbConstantBuffer()
{
}

/*!
 * ロック
 */
TbBool TbConstantBuffer::Write( const TbUint32 offset , const TbUint32 size , const void* src )
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
TbBool TbConstantBuffer::Lock( const TbUint32 offset , const TbUint32 size , void** dst )
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
void TbConstantBuffer::Unlock()
{
    m_Depend.GetBuffer()->Unmap();
}

}

#endif