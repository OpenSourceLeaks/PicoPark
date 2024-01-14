/*!
 * @file
 * @brief バッファ書き込みクラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"

#if 0
#include "base/io/tb_stream_writer.h"
#include "base/math/tb_math_util.h"

namespace toybox
{

/*! 
 * コンストラクタ
 * @param buf 書き込み先バッファ
 * @param size バッファサイズ
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbStreamWriter::TbStreamWriter( void* buf , TbUint32 size )
    : m_BufIndex(0)
    , m_BufSize(size)
{
    m_Buf = reinterpret_cast<TbAddr>(buf);
    if(!m_Buf){
        TB_ASSERT_MSG(TB_FALSE,"m_Buf is NULL");
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbStreamWriter::~TbStreamWriter()
{
}

/*!
 * Char8型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteChar8( const TbChar8 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Uint8型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteUint8( const TbUint8 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Uint16型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteUint16( const TbUint16 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Sint16型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteSint16( const TbSint16 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Uint32型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteUint32( const TbUint32 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Sint32型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteSint32( const TbSint32 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * Float32型書き込み
 * @param value 書き込む値
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::WriteFloat32( const TbFloat32 value )
{
    return Write( &value , sizeof(value) );
}

/*!
 * 書き込む
 * @param 
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
TbResult TbStreamWriter::Write( const void* buf , const TbUint32 size )
{
    if(m_Buf && m_BufIndex+size < m_BufSize){
        void* dst = reinterpret_cast<void*>(m_Buf + m_BufIndex);
        TbMemCopy( dst , buf , size );
        Seek(size);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 進める
 * @author Miyake Shunsuke
 * @since 2011.08.27
 */
void TbStreamWriter::Seek(TbUint32 size)
{
    m_BufIndex = TbClamp(m_BufIndex + size,static_cast<TbUint32>(0U),m_BufSize);
}

}

#endif