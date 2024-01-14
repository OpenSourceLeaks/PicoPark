/*!
 * @file
 * @brief ストリームライタークラス
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_STREAM_WRITER_H_
#define _INCLUDED_TB_STREAM_WRITER_H_

namespace toybox
{

class TbStream;

/*!
 * @class
 * @brief ストリームライタークラス
 * author Miyake Shunsuke
 */
class TbStreamWriter
{
public:

    //コンストラクタ
    TbStreamWriter( TbStream* stream );

    //デストラクタ
    ~TbStreamWriter();

public:

    // 現在のインデックス
    TbUint32 GetCurrentIndex() const { return m_BufIndex; }

    // 書き込みバッファサイズ
    TbUint32 GetSize() const { return m_BufSize; }

    // Char8型書き込み
    TbResult WriteChar8( const TbChar8 value );

    // Uint8型書き込み
    TbResult WriteUint8( const TbUint8 value );

    // Uint16型書き込み
    TbResult WriteUint16( const TbUint16 value );

    // Sint16型書き込み
    TbResult WriteSint16( const TbSint16 value );

    // Uint32型書き込み
    TbResult WriteUint32( const TbUint32 value );

    // Sint32型書き込み
    TbResult WriteSint32( const TbSint32 value );

    // Float32型書き込み
    TbResult WriteFloat32( const TbFloat32 value );

    // 任意型で読み込み
    template<typename T>
    TbResult Write( T& value ) {
        return Write(&value,sizeof(value));
    }

    // 書き込む
    TbResult Write( const void* buf , const TbUint32 size);

    // 進める
    void Seek(TbUint32 size);

private:

    TbStream* m_Stream;

};

}

#endif