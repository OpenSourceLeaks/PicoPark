/*!
 * メモリ上のバッファをストリーム化
 * @author Miyake Shunsuke
 * @since 2012.03.01
 */

#ifndef _INCLUDED_TB_CONST_MEMORY_STREAM_H_
#define _INCLUDED_TB_CONST_MEMORY_STREAM_H_

#include <base/io/stream/tb_stream.h>

namespace toybox
{

class TbConstMemoryStream : public TbStream
{
public:
    typedef TbStream Super;
    typedef TbConstMemoryStream Self;
public:
    
    // コンストラクタ
    TbConstMemoryStream( const void* buf , TbUint32 bufSize );
    
    // デストラクタ
    ~TbConstMemoryStream();
    
public:
    
    // 読み込み
    virtual TbResult Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count);

    // 書き込み
    virtual TbResult Write(const void* buf,TbUint32 count) { return TB_E_FAIL; }
    
    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type );

    
private:
    
    const TbUint8* m_Buffer;
    
};

}

#endif
