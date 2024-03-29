﻿/*!
 * メモリ上のバッファをストリーム化
 * @author Miyake Shunsuke
 * @since 2012.03.01
 */

#include "tb_fwd.h"
#include "base/io/stream/tb_const_memory_stream.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @param buf バッファ
 * @param bufSize バッファサイズ
 * @author Miyake Shunsuke
 * @since 2012.03.05
 */
TbConstMemoryStream::TbConstMemoryStream( const void* buf , TbUint32 bufSize )
    : Super()
    , m_Buffer( reinterpret_cast<const TbUint8*>(buf) )
{
    SetSize(bufSize);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.05
 */
TbConstMemoryStream::~TbConstMemoryStream()
{
}

/*!
 * 読み込み
 * @param buf 読み込んだデータ格納先バッファ
 * @param offset オフセット値
 * @param count サイズ
 * @author Miyake Shunsuke
 * @since 2012.03.05
 */
TbResult TbConstMemoryStream::Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count)
{
    TbUint32 start = GetSeek() + offset;
    TbUint32 end = start + count;
    
    if( end > GetSize() ||
        !buf || 
        0 == count )
    {
        // 読み込みできない
        return TB_E_FAIL;
    }

    // バッファに読み込ませる
    TbMemCopy(buf,m_Buffer+start,count);
    
    Seek( static_cast<TbSint32>(offset+count) , TB_IO_SEEK_TYPE_CURRENT );
    return TB_S_OK;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.05
 */
TbResult TbConstMemoryStream::Seek( TbSint32 size , TbIOSeekType type )
{
    TbSint32 pos = 0;
    switch( type ){
    case TB_IO_SEEK_TYPE_BEGIN:
        break;
    case TB_IO_SEEK_TYPE_CURRENT:
        pos = GetSeek();
        break;
    case TB_IO_SEEK_TYPE_END:
        pos = GetSize();
        break;
    default:
        TB_ASSERT(0);
        break;
    }
    pos += size;
    pos = TbClamp<TbSint32>(pos,0,GetSize());
    SetSeek( static_cast<TbUint32>(pos) );
    return TB_S_OK;
}

}