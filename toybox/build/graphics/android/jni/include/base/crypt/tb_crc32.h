/*!
 * CRC32ハッシュクラス
 * @author Miyake Shunsuke
 * @since 2011.07.11
 */

#ifndef _INCLUDED_TB_CRC32_H_
#define _INCLUDED_TB_CRC32_H_

namespace toybox 
{

struct TbCrc32
{
public:
    static const TbUint32 INVALID = 0;

    // 特定の値を別の値として扱う
    struct Alias
    {
        TbUint8 src;
        TbUint8 dst;
    };

public:
    // ハッシュ値取得
    static TbUint32 GetHash( const TbChar8* data , TbSint32 len ){
        return GetHash( reinterpret_cast<const TbUint8*>(data) , len );
    }
    // ハッシュ値取得
    static TbUint32 GetHash( const TbChar8* data , TbSint32 len , const Alias& alias ){
        return GetHash( reinterpret_cast<const TbUint8*>(data) , len , alias );
    }
    // ハッシュ値取得
    static TbUint32 GetHash( const TbChar8* string ){
        return GetHash( reinterpret_cast<const TbUint8*>(string) , TbStrCharCount(string) );
    }
    // ハッシュ値取得
    static TbUint32 GetHash( const TbChar8* string , const Alias& alias ){
        return GetHash( reinterpret_cast<const TbUint8*>(string) , TbStrCharCount(string) , alias );
    }
    // ハッシュ値取得
    static TbUint32 GetHash( const TbUint8* data , TbSint32 len );
    // ハッシュ値取得
    static TbUint32 GetHash( const TbUint8* data , TbSint32 len , const Alias& alias );
public:
    // 関数オブジェクトとして利用用
    TbUint32 operator()( const TbChar8* data , TbSint32 len ) {
        return TbCrc32::GetHash( data , len );
    }
};

}

#endif