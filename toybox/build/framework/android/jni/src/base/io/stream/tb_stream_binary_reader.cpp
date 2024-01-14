/*!
 * @file
 * @brief ストリームリーダー
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/system/tb_assert.h"
#include "base/math/tb_math_util.h"
#include "base/io/stream/tb_stream_binary_reader.h"
#include "base/io/stream/tb_stream.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.09
 */
TbStreamBinaryReader::TbStreamBinaryReader( TbStream* stream )
    : m_Stream(stream)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.09
 */
TbStreamBinaryReader::~TbStreamBinaryReader()
{
}

/*!
 * char8型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadChar8( TbChar8* result )
{
    TbChar8 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * 基本型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbChar8 TbStreamBinaryReader::ReadChar8()
{
    TbChar8 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}


/*!
 * uint8型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadUint8( TbUint8* result )
{
    TbUint8 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * uint8型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbUint8    TbStreamBinaryReader::ReadUint8()
{
    TbUint8 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * uint8型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadSint8( TbSint8* result )
{
    TbSint8 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * sint8型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbSint8    TbStreamBinaryReader::ReadSint8()
{
    TbSint8 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * uint8型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadUint16( TbUint16* result )
{
    TbUint16 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * uint16型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbUint16   TbStreamBinaryReader::ReadUint16()
{
    TbUint16 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * Sint8型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadSint16( TbSint16* result )
{
    TbSint16 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * sint16型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbSint16   TbStreamBinaryReader::ReadSint16()
{
    TbSint16 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * uint32型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadUint32( TbUint32* result )
{
    TbUint32 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * uint32型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbUint32   TbStreamBinaryReader::ReadUint32()
{
    TbUint32 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * sint32型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadSint32( TbSint32* result )
{
    TbSint32 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * sint32型読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbSint32   TbStreamBinaryReader::ReadSint32()
{
    TbSint32 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * Float32型読み込み試す
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbBool TbStreamBinaryReader::TryReadFloat32( TbFloat32* result )
{
    TbFloat32 out = 0;
    if( m_Stream && TB_SUCCEEDED(m_Stream->Read(&out,0,sizeof(out))) ){
        if( result ){
            *result = out;
            return TB_TRUE;
        }
    }
    return TB_FALSE;
}

/*!
 * float読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbFloat32  TbStreamBinaryReader::ReadFloat32()
{
    TbFloat32 result = 0;
    if( m_Stream ){
        m_Stream->Read(&result,0,sizeof(result));
    }
    return result;
}

/*!
 * 任意サイズ読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.10
 */
TbResult TbStreamBinaryReader::Read(TB_OUT void* buf,TbSint32 offset,TbSint32 count)
{
    if( m_Stream ){
        return m_Stream->Read(buf,offset,count);
    }
    return TB_E_FAIL;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.13
 */
TbResult TbStreamBinaryReader::Seek( TbSint32 size , TbIOSeekType type )
{
    if( m_Stream ){
        return m_Stream->Seek(size,type);
    }
    return TB_E_FAIL;
}

}
