/*!
 * @file
 * @brief バッファ書き込みクラス
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#if 1
#include "base/io/stream/tb_stream_text_writer.h"
#include "base/math/tb_math_util.h"
#include "base/io/stream/tb_stream.h"

namespace toybox
{

/*! 
 * コンストラクタ
 * @param stream ストリーム
 * @author teco
 */
TbStreamTextWriter::TbStreamTextWriter( TbStream* stream  )
    : m_Stream(stream)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbStreamTextWriter::~TbStreamTextWriter()
{
}

// フォーマット文字列書き込み
TbResult TbStreamTextWriter::Printf(const TbChar8* format, ...)
{
    if (m_Stream) {
        static const TbUint32 BUF_SIZE = 512;
        va_list ap;
        TbChar8    buf[BUF_SIZE];
        va_start(ap, format);
        TbVStrPrintf(buf, BUF_SIZE, format, ap);
        va_end(ap);

        TbSint32 size = TbStrCharCount(buf);
        return m_Stream->Write(buf, size);
    }
    return TB_E_FAIL;
}

/*!
 * 文字列書き込み
 * @author teco
 */
TbResult TbStreamTextWriter::WriteString( const TbChar8* string )
{
    if( m_Stream && string ){
        TbSint32 size = TbStrCharCount(string);
        return m_Stream->Write( string , size );
    }
    return TB_E_FAIL;
}

/*!
 * 文字書き込み
 * @author teco
 */
TbResult TbStreamTextWriter::WriteChar8( const TbChar8 value )
{
    if( m_Stream ){
        return m_Stream->Write( &value , sizeof(value) );
    }
    return TB_E_FAIL;
}

}

#endif