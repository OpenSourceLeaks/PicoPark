/*!
 * 文字エンコーディングクラス(UTF16)
 * @author Miyake Shunsuke
 * @since @date
 */

#ifndef _INCLUDED_TB_STRING_ENCODING_UTF16_H_
#define _INCLUDED_TB_STRING_ENCODING_UTF16_H_

#include <base/string/encoding/tb_string_encoding.h>
#include <base/util/tb_singleton.h>

namespace toybox
{

class TbStringEncodingUTF16
{
public:

    // コンストラクタ
    TbStringEncodingUTF16() {}

    // デストラクタ
    ~TbStringEncodingUTF16() {}

public: // マルチバイト文字用
    
    // index文字目のバイト数を取得
    virtual TbUint32 GetCharByteCount( TbUint32 index , const TbChar8* string ) const;

    // バイト数取得
    virtual TbUint32 GetByteCount( const TbChar8* string ) const;
    
    // 文字数取得
    virtual TbUint32 GetCharCount( const TbChar8* string ) const;
    
public: // 2バイト文字用 ( 非対応 )
    
    // index文字目のバイト数を取得
    virtual TbUint32 GetCharByteCount( TbUint32 index , const TbChar16* string ) const { return 0U; }

    // バイト数取得
    virtual TbUint32 GetByteCount( const TbChar16* string ) const { return 0U; }
    
    // 文字数
    virtual TbUint32 GetCharCount( const TbChar16* string ) const { return 0U; }
    
public:
    
    // Sin32 -> 全角文字列
    virtual TbResult ConvertSint32ToString( TbStaticString64& string , TbSint32 src )const;
    
    // Uint32 -> 全角文字列
    virtual TbResult ConvertUint32ToString( TbStaticString64& string , TbUint32 src )const;

    // Float32 -> 全角文字列
    virtual TbResult ConvertFloat32ToString( TbStaticString64& string , TbFloat32 src )const;

    // Bool -> 全角文字列
    virtual TbResult ConvertBoolToString( TbStaticString8& string , TbBool src )const;

    // 全角文字列 -> Sin32
    virtual TbSint32 ConvertStringToSint32( const TbChar8* string )const;
    
    // 全角文字列 -> Uint32
    virtual TbUint32 ConvertStringToUint32( const TbChar8* string )const;

    // 全角文字列 -> Float32
    virtual TbFloat32 ConvertStringToFloat32( const TbChar8* string )const;

    // 全角文字列 -> Bool
    virtual TbBool ConvertStringToBool( const TbChar8* string )const;

    // 全角文字列 -> Sin32
    virtual TbBool TryConvertStringToSint32( const TbChar8* string , TbSint32& result );
    
    // 全角文字列 -> Uint32
    virtual TbBool TryConvertStringToUint32( const TbChar8* string , TbUint32& result );

    // 全角文字列 -> Float32
    virtual TbBool TryConvertStringToFloat32( const TbChar8* string , TbFloat32& result );

    // 全角文字列 -> Bool
    virtual TbBool TryConvertStringToBool( const TbChar8* string , TbBool& result );

};

}

#endif
