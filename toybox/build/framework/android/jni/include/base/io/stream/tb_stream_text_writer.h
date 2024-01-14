/*!
 * @file
 * @brief ストリームテキストライタークラス
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_STREAM_TEXT_WRITER_H_
#define _INCLUDED_TB_STREAM_TEXT_WRITER_H_

#include <base/string/tb_string_encoding_all.h>

namespace toybox
{

class TbStream;

class TbStreamTextWriter
{
public:

    //コンストラクタ
    TbStreamTextWriter( TbStream* stream );

    //デストラクタ
    ~TbStreamTextWriter();

public:

    // フォーマット文字列書き込み
    TbResult Printf(const TbChar8* format , ...);

    // マルチバイト文字列書き込み
    TbResult WriteString( const TbChar8* string );
   
    // マルチバイト文字書き込み
    TbResult WriteChar8( const TbChar8 value );
    
    // 終端文字書き込み
    TbResult WriteEOFChar8() {
        return WriteChar8('\0');
    }

public:
    
    // ダブルバイト文字列書き込み
    TbResult WriteString( const TbChar16* string );
   
    // ダブルバイト文字書き込み
    TbResult WriteChar8( const TbChar16 value );

public:

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

    // Float64型書き込み
    TbResult WriteFloat64( const TbFloat64 value );

    // 任意型を文字列書き込み
    template<typename T>
    TbResult Write( T& value ) {
        value.Write(*this);
    }

private:

    TbStream*            m_Stream;       // ストリーム

};

}

#endif