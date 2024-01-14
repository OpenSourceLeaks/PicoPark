/*!
 * インデックスバッファ
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_constant_buffer.h"

#if TB_3DAPI_IS_DX9

namespace toybox
{

//

/*!
 * コンストラクタ
 * @param size バッファサイズ
 * @author teco
 */
TbConstantBuffer::TbConstantBuffer( const TbSizeT size )
    : m_Size(size)
    , m_Depend(size)
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
        TbUint8* buf = m_Depend.GetBuffer();
        TbMemCopy(&buf[offset],src,size);
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
        TbUint8* buf = m_Depend.GetBuffer();
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
}

}

#endif