/*!
 * ストリームテキスト形式リーダークラス
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_STREAM_TEXT_READER_H_
#define _INCLUDED_TB_STREAM_TEXT_READER_H_

#include <base/string/tb_string_encoding_types.h>

namespace toybox
{

class TbStream;

class TbStreamTextReader
{
public:

    //コンストラクタ
    TbStreamTextReader( TbStream* stream , TbStringEncodingType type );

    //デストラクタ
    ~TbStreamTextReader();

public: // マルチバイト文字用

    // 1行読み込み
    TbResult ReadLine( TbChar8** buffer , TbUint32 bufSize );

    // 1文字読み込み
    TbResult ReadChar( TbChar8** buffer , TbUint32 bufSize );

    // 区切り文字まで読み込み
    TbResult ReadString( TbChar8** buffer , TbUint32 bufSize , TbChar8 separate );
    
public: // 2バイト文字用

    // 1行読み込み
    TbResult ReadLine( TbCha16** buffer , TbUint32 bufSize );

    // 1文字読み込み
    TbResult ReadChar( TbChar16** buffer , TbUint32 bufSize );

    // 区切り文字まで読み込み
    TbResult ReadString( TbChar16** buffer , TbUint32 bufSize , TbChar16 separate );

private:

    TbStream*       m_Stream;       // ストリーム
    TbEncodingType  m_EncodingType; // エンコーディングタイプ

};

}

#endif