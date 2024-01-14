/*!
 * 文字エンコーディングクラス
 * @author Miyake Shunsuke
 * @since @date
 */

#ifndef _INCLUDED_TB_STRING_ENCODING_H_
#define _INCLUDED_TB_STRING_ENCODING_H_

#include <base/string/tb_static_string.h>

namespace toybox
{

class TbStringEncoding
{    
public: // マルチバイト文字用
    
    // index文字目のバイト数を取得
    virtual TbUint32 GetCharByteCount( TbUint32 index , const TbChar8* string ) const = 0;

    // バイト数取得
    virtual TbUint32 GetByteCount( const TbChar8* string ) const = 0;
    
    // 文字数取得
    virtual TbUint32 GetCharCount( const TbChar8* string ) const = 0;
    
public: // 2バイト文字用
    
    // index文字目のバイト数を取得
    virtual TbUint32 GetCharByteCount( TbUint32 index , const TbChar16* string ) const = 0;

    // バイト数取得
    virtual TbUint32 GetByteCount( const TbChar16* string ) const = 0;
    
    // 文字数
    virtual TbUint32 GetCharCount( const TbChar16* string ) const = 0;
    
public:

    // Sin32 -> 全角文字列
    virtual TbResult ConvertSint32ToString( TbStaticString64& string , TbSint32 src )const=0;
    
    // Uint32 -> 全角文字列
    virtual TbResult ConvertUint32ToString( TbStaticString64& string , TbUint32 src )const=0;

    // Float32 -> 全角文字列
    virtual TbResult ConvertFloat32ToString( TbStaticString64& string , TbFloat32 src )const=0;

    // Bool -> 全角文字列
    virtual TbResult ConvertBoolToString( TbStaticString8& string , TbBool src )const=0;

    // 全角文字列 -> Sin32
    virtual TbSint32 ConvertStringToSint32( const TbChar8* string )const=0;
    
    // 全角文字列 -> Uint32
    virtual TbUint32 ConvertStringToUint32( const TbChar8* string )const=0;

    // 全角文字列 -> Float32
    virtual TbFloat32 ConvertStringToFloat32( const TbChar8* string )const=0;

    // 全角文字列 -> Bool
    virtual TbBool ConvertStringToBool( const TbChar8* string )const=0;

    // 全角文字列 -> Sin32
    virtual TbBool TryConvertStringToSint32( const TbChar8* string , TbSint32& result )const=0;
    
    // 全角文字列 -> Uint32
    virtual TbBool TryConvertStringToUint32( const TbChar8* string , TbUint32& result )const=0;

    // 全角文字列 -> Float32
    virtual TbBool TryConvertStringToFloat32( const TbChar8* string , TbFloat32& result )const=0;

    // 全角文字列 -> Bool
    virtual TbBool TryConvertStringToBool( const TbChar8* string , TbBool& result )const=0;

public: // ASCII変換は基底クラスでサポート

    // ASCII文字列 -> Sin32
    static TbSint32 ConvertASCIIToSint32( const TbChar8* string );
    
    // ASCII文字列 -> Uint32
    static TbUint32 ConvertASCIIToUint32( const TbChar8* string );

    // ASCII文字列 -> Float32
    static TbFloat32 ConvertASCIIToFloat32( const TbChar8* string );

    // ASCII文字列 -> Bool
    static TbBool ConvertASCIIToBool( const TbChar8* string );

    // ASCII文字列 -> Sin32
    static TbBool TryConvertASCIIToSint32( const TbChar8* string , TbSint32& result );
    
    // ASCII文字列 -> Uint32
    static TbBool TryConvertASCIIToUint32( const TbChar8* string , TbUint32& result );

    // ASCII文字列 -> Float32
    static TbBool TryConvertASCIIToFloat32( const TbChar8* string , TbFloat32& result );

    // ASCII文字列 -> Bool
    static TbBool TryConvertASCIIToBool( const TbChar8* string , TbBool& result );

public:

    // Sin32 -> ASCII文字列
    static TbResult ConvertSint32ToASCII( TbStaticString64& string , TbSint32 src );
    
    // Uint32 -> ASCII文字列
    static TbResult ConvertUint32ToASCII( TbStaticString64& string , TbUint32 src );

    // Float32 -> ASCII文字列
    static TbResult ConvertFloat32ToASCII( TbStaticString64& string , TbFloat32 src );

    // Bool -> ASCII文字列
    static TbResult ConvertBoolToASCII( TbStaticString8& string , TbBool src );

private:
    
    // コンストラクタ
    TbStringEncoding();

    // デストラクタ
    ~TbStringEncoding();
    
};

}

#endif
