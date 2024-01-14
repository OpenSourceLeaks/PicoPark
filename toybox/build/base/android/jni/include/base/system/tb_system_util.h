/*!
 * システム全体で利用するマクロ、関数ヘッダー
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SYSTEM_UTIL_H_
#define _INCLUDED_TB_SYSTEM_UTIL_H_

namespace toybox
{

// 定義済みマクロ
#if defined(__FUNCTION__)
    #define TB_FUNCNAME __FUNCTION__
#else
    #define TB_FUNCNAME ""
#endif
#if defined(__FILE__)
    #define TB_FILENAME __FILE__
#else
    #define TB_FILENAME ""
#endif

#if defined(__LINE__)
    #define TB_LINE __LINE__
#else
    #define TB_LINE ""
#endif

// パス最大数
#define TB_MAX_PATH 512

// 依存コード側実装ヒントマクロ
#define TB_DEPEND

// 出力、入出力引数ヒントマクロ
#define TB_OUT 
#define TB_INOUT 

//安全なDelete
#define TB_SAFE_DELETE(p)          if(p){TB_DELETE p;(p)=NULL;}
#define TB_SAFE_DELETE_ARRAY(p)    if(p){TB_DELETE[] p;(p)=NULL;}
#define TB_SAFE_FREE(p)            if(p){TbFree(p);(p)=NULL;}
#define TB_SAFE_RELEASE(p)         if(p){(p)->Release();(p)=NULL;}
#define TB_SAFE_KILL(p)            if(p){(p)->Kill();(p)=NULL;}

//アライメントを揃える(切り上げ)
#define TB_ALIGN_ROUND_UP(value, alignment) \
    (((TbUint32)(value) + ((alignment)-1)) & ~((alignment)-1))

#define TB_ALIGN_ROUND_UP_64(value, alignment) \
    (((TbUint64)(value) + ((TbUint64)(alignment)-1)) & (~((TbUint64)(alignment)-1)))

//アライメントを揃える(切り下げ)
#define TB_ALIGN_ROUND_DOWN(value, alignment) \
    ((TbUint32)(value) & ~((alignment)-1))

#define TB_ALIGN_ROUND_DOWN_64(value, alignment) \
    ((TbUint64)(value) & (~((TbUint64)(alignment)-1)))

// 未使用変数
#define TB_UNUSE( value ) ((void)value)

//配列の長さ
#define TB_ARRAY_COUNT_OF(_array_)          (sizeof(_array_)/sizeof(_array_[0]))

// マルチバイト文字をTbUint16に格納
#define TB_MULTI_CHARA_16( a , b ) ((TbUint16)(b) << 8 | \
                                    (TbUint16)(a))

// TbUint32のチャンクタグを作成
#define TB_CHUNK_TAG( a , b , c , d ) ((TbUint32)(d) << 24 | \
                                       (TbUint32)(c) << 16 | \
                                       (TbUint32)(b) << 8 | \
                                       (TbUint32)(a))

inline TbUint32 TbChunkTag3( const char (&tag)[3] ){
    return TB_CHUNK_TAG(tag[0],tag[1],tag[2],'\0');
}
// 0終端文字列用
inline TbUint32 TbChunkTag3( const char (&tag)[4] ){
    return TB_CHUNK_TAG(tag[0],tag[1],tag[2],'\0');
}
inline TbUint32 TbChunkTag4( const char (&tag)[4] ){
    return TB_CHUNK_TAG(tag[0],tag[1],tag[2],tag[3]);
}
// 0終端文字列用
inline TbUint32 TbChunkTag4( const char (&tag)[5] ){
    return TB_CHUNK_TAG(tag[0],tag[1],tag[2],tag[3]);
}

// 指定した値を文字列変換
#define TB_TO_STRING(type) (#type)

// 指定したインデックス値からビット値へ
#define TB_BIT(index) ( 1 << index )

// ビットが立っている数取得
inline TbUint32 TbBitPopulation(TbUint32 bits) {
    bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
    bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
    bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
    bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
    return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}

// 最大有効ビット値取得
inline TbUint32 TbBitMostSignificant( TbUint32 bits ) 
{
    // todo 高速アルゴリズムに乗り換えるべし
    for( TbSint32 i = 32-1; i >= 0; --i ) {
        if( bits & TB_BIT(i) ) {
            return i;
        }
        if( 0 == i ){
            break;
        }
    }
    return 0;
}

// 最小有効ビット値取得
inline TbUint32 TbBitLeastSignificant( TbUint32 bits ) 
{
    // todo 高速アルゴリズムに乗り換えるべし
    for( TbUint32 i = 0; i < 32; ++i ) {
        if( bits & TB_BIT(i) ) {
            return i;
        }
    }
    return 0;
}

// コピーコンストラクタ、代入禁止
#define TB_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName( const TypeName& ); \
    void operator=( const TypeName& )

// 代入禁止
#define TB_DISALLOW_ASSIGN(TypeName) \
    void operator=( const TypeName& )

//インライン
#if TB_IS_DEBUG
    #define TB_INLINE 
#else
    #define TB_INLINE inline
#endif

// 強制インライン
#if (TB_PLATFORM_IS_WIN)
    #define TB_FORCE_INLINE __forceinline
#else
    #define TB_FORCE_INLINE inline
#endif

// override(使えないコンパイラの場合は、無効化)
#if !TB_HAS_OVERRIDE
#define TB_OVERRIDE
#else
#define TB_OVERRIDE override
#endif

// メンバ変数のバイトオフセットを取得
#define TB_OFFSET_OF(type, member)  \
    (reinterpret_cast<TbAddr>(  \
      &reinterpret_cast<char const volatile&>(  \
        ((type*)0)->member)))

// メンバ変数のサイズを取得
#define TB_MEMBER_SIZE_OF(type, member) sizeof(((type*)0)->member)

}

// 機種依存マクロ
#if TB_PLATFORM_IS_ANDROID
#   include "./tb_system_util_android.h"
#endif

#endif