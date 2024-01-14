/*!
 * メモリ上のバッファをストリーム化
 * @author teco
 */

#ifndef _INCLUDED_TB_MEMORY_STREAM_H_
#define _INCLUDED_TB_MEMORY_STREAM_H_

#include <base/io/stream/tb_stream.h>

namespace toybox
{

class TbMemoryStream : public TbStream
{
public:
    typedef TbStream Super;
    typedef TbMemoryStream Self;
public:
    
    // コンストラクタ
    TbMemoryStream();

    // コンストラクタ
    TbMemoryStream( void* buf , TbUint32 bufSize );
    
    // デストラクタ
    ~TbMemoryStream();

public:

    // 初期化
    void Initialize(void* buf, TbUint32 bufSize);
    
public:
    
    // 読み込み
    virtual TbResult Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count);
    
    // 書き込み
    virtual TbResult Write(const void* buf,TbUint32 count);
    
    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type );
    
private:
    
    TbUint8*    m_Buffer;
    
};

}

#endif
