/*!
 * @file
 * @brief ストリームリーダークラス
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_STREAM_BINARY_READER_H_
#define _INCLUDED_TB_STREAM_BINARY_READER_H_

#include "base/io/tb_io_types.h"

namespace toybox
{

class TbStream;

/*!
 * @class
 * @brief ストリームリーダークラス
 * author Miyake Shunsuke
 */
class TbStreamBinaryReader
{
public:

    //コンストラクタ
    TbStreamBinaryReader( TbStream* stream );

    //デストラクタ
    ~TbStreamBinaryReader();

public:

    //char8型読み込み試す
    TbBool    TryReadChar8( TbChar8* result=NULL );

    //char8型読み込み
    TbChar8   ReadChar8();
    
    //uint8型読み込み試す
    TbBool    TryReadUint8( TbUint8* result=NULL );

    //uint8型読み込み
    TbUint8    ReadUint8();

    //sint8型読み込み試す
    TbBool    TryReadSint8( TbSint8* result=NULL );

    //sint8型読み込み
    TbSint8    ReadSint8();

    //uint18型読み込み試す
    TbBool    TryReadUint16( TbUint16* result=NULL );

    //uint16型読み込み
    TbUint16   ReadUint16();

    //sint16型読み込み試す
    TbBool    TryReadSint16( TbSint16* result=NULL );

    //sint16型読み込み
    TbSint16   ReadSint16();

    //uint32型読み込みを試す
    TbBool    TryReadUint32( TbUint32* result=NULL );

    //uint32型読み込み
    TbUint32   ReadUint32();

    //sint32型読み込みを試す
    TbBool    TryReadSint32( TbSint32* result=NULL );

    //sint32型読み込み
    TbSint32   ReadSint32();

    //float32型読み込みを試す
    TbBool    TryReadFloat32( TbFloat32* result=NULL );

    //float32読み込み
    TbFloat32  ReadFloat32();

    // 任意型で読み込み
    template<typename T>
    TbResult Read( T& value ) {
        return Read(&value,0,sizeof(value));
    }

    // 任意サイズ読み込み
    TbResult Read(TB_OUT void* buf,TbSint32 offset,TbSint32 count);

    // シーク
    TbResult Seek( TbSint32 offset , TbIOSeekType type );

public:

    // ストリーム取得
    TbStream* GetStream() {
        return m_Stream;
    }

private:

    TbStream*   m_Stream;   // ストリーム

};

}

#endif