/*!
 * @file
 * @brief 色情報構造体のヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_COLOR_H_
#define _INCLUDED_TB_COLOR_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <base/math/tb_math_util.h>

#include "./depend/tb_color_depend.h"

namespace toybox
{

enum TbColorFormat
{
    TB_COLOR_FORMAT_BEGIN,
    TB_COLOR_FORMAT_UNKNOWN = TB_COLOR_FORMAT_BEGIN,
    // 16bit カラーフォーマット
    TB_COLOR_FORMAT_A4G4B4R4,
    TB_COLOR_FORMAT_R5G6B5,
    // 24bit カラーフォーマット
    TB_COLOR_FORMAT_R8G8B8,
    TB_COLOR_FORMAT_B8G8R8,
    // 32bit カラーフォーマット
    TB_COLOR_FORMAT_A8R8G8B8,
    TB_COLOR_FORMAT_B8G8R8A8,
    TB_COLOR_FORMAT_A8B8G8R8,
    TB_COLOR_FORMAT_X8R8G8B8,
    // 128bit カラーフォーマット
    TB_COLOR_FORMAT_A32R32G32B32,
    // 深度ステンシル用カラーフォーマット
    TB_COLOR_FORMAT_D16,      // 深度バッファ16bit
    TB_COLOR_FORMAT_D32,      // 深度バッファ32bit
    TB_COLOR_FORMAT_D24S8,    // 深度バッファ24bit ステンシルバッファ8bit
    TB_COLOR_FORMAT_END,

    TB_COLOR_FORMAT_NUM = TB_COLOR_FORMAT_END - TB_COLOR_FORMAT_BEGIN,
};

// カラーフォーマットのビット数取得
TbUint8 TbGetColorFormatBitSize( const TbColorFormat colorFormat );
// カラーフォーマットのバイト数取得
inline TbUint8 TbGetColorFormatByteSize( const TbColorFormat colorFormat ){
    return TbGetColorFormatBitSize( colorFormat ) / 8;
}
// カラーフォーマットのビット数取得(深度)
TbUint8 TbGetColorFormatDepthBitSize( const TbColorFormat colorFormat );
// カラーフォーマットのビット数取得(ステンシル)
TbUint8 TbGetColorFormatStencilBitSize( const TbColorFormat colorFormat );

// 32bitカラー用マクロ
#if (TB_COLOR_MAP == TB_COLOR_MAP_ARGB)
    #define TB_COLOR_A8R8G8B8( a , r , g , b ) \
        (( (a) << 24 ) | ( (r) << 16 ) | ( (g) << 8 ) | (b))
    #define TB_COLOR_ARGB_F32_TO_U8( a , r , g , b ) \
        (static_cast<TbUint32>(TbClamp(a,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 24) | \
        (static_cast<TbUint32>(TbClamp(r,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 16) | \
        (static_cast<TbUint32>(TbClamp(g,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 8) | \
        (static_cast<TbUint32>(TbClamp(b,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)));
    #define TB_COLOR_A8R8G8B8_GET_R( value ) ( ( value >> 16 ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_G( value ) ( ( value >> 8 ) & 0xFF ) 
    #define TB_COLOR_A8R8G8B8_GET_B( value ) ( ( value ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_A( value ) ( ( value >> 24 ) & 0xFF )

    #define TB_COLOR_R8G8B8( r , g , b ) \
        (( (r) << 16 ) | ( (g) << 8 ) | ( (b) ))
    #define TB_COLOR_R8G8B8_GET_R( value ) ( ( value >> 16 ) & 0xFF )
    #define TB_COLOR_R8G8B8_GET_G( value ) ( ( value >> 8 ) & 0xFF ) 
    #define TB_COLOR_R8G8B8_GET_B( value ) ( ( value ) & 0xFF )
#elif (TB_COLOR_MAP == TB_COLOR_MAP_RGBA)
    #define TB_COLOR_A8R8G8B8A8( a , r , g , b ) \
        (( (r) << 24 ) | ( (g) << 16 ) | ( (b) << 8 ) | (a))
    #define TB_COLOR_ARGB_F32_TO_U8( a , r , g , b ) \
        (static_cast<TbUint32>(TbClamp(r,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 24) | \
        (static_cast<TbUint32>(TbClamp(g,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 16) | \
        (static_cast<TbUint32>(TbClamp(b,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 8) | \
        (static_cast<TbUint32>(TbClamp(a,0.0f,1.0f) * static_cast<TbFloat32>(0xFF))); 
    #define TB_COLOR_A8R8G8B8_GET_R( value ) ( ( value >> 24 ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_G( value ) ( ( value >> 16 ) & 0xFF ) 
    #define TB_COLOR_A8R8G8B8_GET_B( value ) ( ( value >> 8 ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_A( value ) ( ( value ) & 0xFF )

    #define TB_COLOR_R8G8B8( r , g , b ) \
        (( (r) << 16 ) | ( (g) << 8 ) | ( (b) ))
    #define TB_COLOR_R8G8B8_GET_R( value ) ( ( value >> 16 ) & 0xFF )
    #define TB_COLOR_R8G8B8_GET_G( value ) ( ( value >> 8 ) & 0xFF ) 
    #define TB_COLOR_R8G8B8_GET_B( value ) ( ( value ) & 0xFF )
#elif (TB_COLOR_MAP == TB_COLOR_MAP_ABGR)
    #define TB_COLOR_A8R8G8B8( a , r , g , b ) \
        (( (r) ) | ( (g) << 8 ) | ( (b) << 16 ) | (a << 24))
    #define TB_COLOR_ARGB_F32_TO_U8( a , r , g , b ) \
        (static_cast<TbUint32>(TbClamp(r,0.0f,1.0f) * static_cast<TbFloat32>(0xFF))) | \
        (static_cast<TbUint32>(TbClamp(g,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 8) | \
        (static_cast<TbUint32>(TbClamp(b,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 16) | \
        (static_cast<TbUint32>(TbClamp(a,0.0f,1.0f) * static_cast<TbFloat32>(0xFF)) << 24); 
    #define TB_COLOR_A8R8G8B8_GET_R( value ) ( ( value ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_G( value ) ( ( value >> 8 ) & 0xFF ) 
    #define TB_COLOR_A8R8G8B8_GET_B( value ) ( ( value >> 16 ) & 0xFF )
    #define TB_COLOR_A8R8G8B8_GET_A( value ) ( ( value >> 24 ) & 0xFF )

    #define TB_COLOR_R8G8B8( r , g , b ) \
        (( (r) ) | ( (g) << 8 ) | ( (b) << 16 ))
    #define TB_COLOR_R8G8B8_GET_R( value ) ( ( value ) & 0xFF )
    #define TB_COLOR_R8G8B8_GET_G( value ) ( ( value >> 8 ) & 0xFF ) 
    #define TB_COLOR_R8G8B8_GET_B( value ) ( ( value >> 16 ) & 0xFF )
#endif

/*!
 * uint32型色構造体
 * 直接valueに値を入れる場合は、argbの順かrgbaの順なのかは機種依存なので必ずTB_COLOR_U32_****マクロを利用して代入してください
 */
struct TbColorU32
{
    // 白
    static TbColorU32 White() {
        return FromRgbaU8(0xFF,0xFF,0xFF,0xFF);
    }
    // 黒
    static TbColorU32 Black() {
        return FromRgbaU8(0,0,0,0xFF);
    }
    // 赤
    static TbColorU32 Red()  {
        return FromRgbaU8(0xFF,0,0,0xFF);
    }
    // 緑
    static TbColorU32 Green() {
        return FromRgbaU8(0x0,0xFF,0,0xFF);
    }
    // 青
    static TbColorU32 Blue() {
        return FromRgbaU8(0x0,0x0,0xFF,0xFF);
    }
    // マゼンタ
    static TbColorU32 Magenta() {
        return FromRgbaU8(0xFF,0x0,0xFF,0xFF);
    }
    // シアン
    static TbColorU32 Cyan() {
        return FromRgbaU8(0x0,0xFF,0xFF,0xFF);
    }
    // 黄色
    static TbColorU32 Yellow() {
        return FromRgbaU8(0xFF,0xFF,0x0,0xFF);
    }

    // RGBA値から作成
    static TbColorU32 FromRgbaF32(TbFloat32 r , TbFloat32 g , TbFloat32 b ,TbFloat32 a)
    {
        TbColorU32 result;
        result.SetColorF32(r,g,b,a);
        return result;
    }

    // RGBA値から作成
    static TbColorU32 FromRgbaU8(TbUint8 r , TbUint8 g , TbUint8 b ,TbUint8 a)
    {
        TbColorU32 result;
        result.SetColorU8(r,g,b,a);
        return result;
    }

    //! コンストラクタ
    TbColorU32()
        : value(0) {}

    //! コンストラクタ    
    TbColorU32( TbUint32 color )
        : value( color )
    {
    }

    //! コンストラクタ
    TbColorU32( const TbColorU32& color )
        : value( color.value )
    {
    }

    //! ==比較演算子
    TbBool operator==(const TbColorU32& color ) const{
        return value == color.value;
    }

    //! !=比較演算子
    TbBool operator!=(const TbColorU32& color ) const{
        return value != color.value;
    }

    // pow関数(alphaはしない)
    TbColorU32 Pow(TbFloat32 power) const
    {
        TbFloat32 r = TbPow(GetRedF32(), power);
        TbFloat32 g = TbPow(GetGreenF32(), power);
        TbFloat32 b = TbPow(GetBlueF32(), power);
        TbColorU32 result = TbColorU32::FromRgbaF32(r,g,b,GetAlphaF32());
        return result;
    }

    //! 設定
    void SetColorF32( TbFloat32 r,TbFloat32 g,TbFloat32 b,TbFloat32 a )
    {
        value = TB_COLOR_ARGB_F32_TO_U8( a , r , g , b );
    }

    //! 設定
    void SetColorF32( TbFloat32 r,TbFloat32 g,TbFloat32 b )
    {
        value = TB_COLOR_ARGB_F32_TO_U8( GetAlphaF32() , r , g , b );
    }

    //! 設定
    void SetRedF32( TbFloat32 r )
    {
        value = TB_COLOR_ARGB_F32_TO_U8( GetAlphaF32() , r , GetGreenF32() , GetBlueF32() );
    }

    //! 設定
    void SetGreenF32( TbFloat32 g )
    {
        value = TB_COLOR_ARGB_F32_TO_U8( GetAlphaF32() , GetRedF32() , g , GetBlueF32() );
    }

    //! 設定
    void SetBlueF32( TbFloat32 b )
    {
        value = TB_COLOR_ARGB_F32_TO_U8( GetAlphaF32() , GetRedF32() , GetGreenF32() , b );
    }

    //! 設定
    void SetAlphaF32( TbFloat32 a )
    {
        value &= TB_COLOR_A8R8G8B8(0x0,0xFF,0xFF,0xFF);
        value |= TB_COLOR_ARGB_F32_TO_U8(a,0.0f,0.0f,0.0f);
    }

    //! 設定
    void SetColorU8( TbUint8 r , TbUint8 g , TbUint8 b , TbUint8 a )
    {
        value = TB_COLOR_A8R8G8B8( a , r , g , b );
    }

    //! 設定
    void SetColorU8( TbUint8 r , TbUint8 g , TbUint8 b )
    {
        value = TB_COLOR_A8R8G8B8( GetAlphaU8() , r , g , b );
    }

    //! 設定
    void SetRedU8( TbUint8 r )
    {
        value = TB_COLOR_A8R8G8B8( GetAlphaU8() , r , GetGreenU8() , GetBlueU8() );
    }

    //! 設定
    void SetGreenU8( TbUint8 g )
    {
        value = TB_COLOR_A8R8G8B8( GetAlphaU8() , GetRedU8() , g , GetBlueU8() );
    }

    //! 設定
    void SetBlueU8( TbUint8 b )
    {
        value = TB_COLOR_A8R8G8B8( GetAlphaU8() , GetRedU8() , GetGreenU8() , b );
    }

    //! 設定
    void SetAlphaU8( TbUint8 a )
    {
        value &= TB_COLOR_A8R8G8B8(0x0,0xFF,0xFF,0xFF);
        value |= TB_COLOR_A8R8G8B8(a,0,0,0);
    }

    //! 設定
    void SetColorU32( TbUint32 value )
    {
        this->value = value;
    }

    // 取得
    TbUint32 GetColorU32() const {
        return this->value;
    }

    //! u8型の赤成分取得
    TbUint8 GetRedU8() const 
    {
        return static_cast<TbUint8>(TB_COLOR_A8R8G8B8_GET_R(value));
    }

    //! u8型の緑成分取得
    TbUint8 GetGreenU8() const 
    {
        return static_cast<TbUint8>(TB_COLOR_A8R8G8B8_GET_G(value));
    }

    //! u8型の青成分取得
    TbUint8 GetBlueU8() const 
    {
        return static_cast<TbUint8>(TB_COLOR_A8R8G8B8_GET_B(value));
    }

    //! u8型のアルファ成分取得
    TbUint8 GetAlphaU8() const 
    {
        return static_cast<TbUint8>(TB_COLOR_A8R8G8B8_GET_A(value));
    }

    //! f32型の赤成分取得
    TbFloat32 GetRedF32() const 
    {
        return static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_R(value)) / 255.0f;
    }

    //! f32型の緑成分取得    
    TbFloat32 GetGreenF32() const 
    {
        return static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_G(value)) / 255.0f;
    }

    //! f32型の青成分取得
    TbFloat32 GetBlueF32() const 
    {
        return static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_B(value)) / 255.0f;
    }

    //! f32型のアルファ成分取得
    TbFloat32 GetAlphaF32() const 
    {
        return static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_A(value)) / 255.0f;
    }

    typedef TbUint32 ValueType;
    ValueType value;
};

/*!
 * float型色構造体
 * u32Value形式で値を入れる場合は、argbの順かrgbaの順なのかは機種依存なので必ずTB_COLOR_U32_****マクロを利用して代入してください
 */
struct TbColorF128
{
    // 白
    static TbColorF128 White() {
        return FromRgbaF32(1.0f,1.0f,1.0f,1.0f);
    }
    // 黒
    static TbColorF128 Black() {
        return FromRgbaF32(0,0,0,1.0f);
    }
    // 赤
    static TbColorF128 Red()  {
        return FromRgbaF32(1.0f,0,0,1.0f);
    }
    // 緑
    static TbColorF128 Green() {
        return FromRgbaF32(0,1.0f,0,1.0f);
    }
    // 青
    static TbColorF128 Blue() {
        return FromRgbaF32(0,0,1.0f,1.0f);
    }
    // マゼンタ
    static TbColorF128 Magenta() {
        return FromRgbaF32(0xFF,0,1.0f,1.0f);
    }
    // シアン
    static TbColorF128 Cyan() {
        return FromRgbaF32(0,1.0f,1.0f,1.0f);
    }
    // 黄色
    static TbColorF128 Yellow() {
        return FromRgbaF32(1.0f,1.0f,0,1.0f);
    }
    // RGBA値から作成
    static TbColorF128 FromRgbaF32(TbFloat32 r , TbFloat32 g , TbFloat32 b ,TbFloat32 a)
    {
        TbColorF128 result;
        result.SetColorF32(r,g,b,a);
        return result;
    }
    // RGBA値から作成
    static TbColorF128 FromRgbaU8(TbUint8 r , TbUint8 g , TbUint8 b ,TbUint8 a)
    {
        TbColorF128 result;
        result.SetColorU8(r,g,b,a);
        return result;
    }

    //! コンストラクタ
    TbColorF128()
        : r( 0.0f ) , g( 0.0f ) , b( 0.0f ) , a( 0.0f )
    {
    }

    //! コンストラクタ( R8G8B8A8 )
    TbColorF128( TbUint32 color ) { 
        SetColorU32( color );
    }

    //! コンストラクタ( R32G32B32A32 )
    TbColorF128( const TbColorF128& color ) {
        SetColorF32( color.r , color.g , color.b , color.a );
    }

    //! ==比較演算子
    TbBool operator==(const TbColorF128& color ) const{
        return r == color.r && g == color.g && b == color.b && a == color.a;
    }

    //! !=比較演算子
    TbBool operator!=(const TbColorF128& color ) const{
        return r != color.r || g != color.g || b != color.b || a != color.a;
    }

    // pow関数
    TbColorF128 Pow( TbFloat32 power ) const
    {
        TbColorF128 result = TbColorF128::FromRgbaF32( 
                                 TbPow(r,power),
                                 TbPow(g,power),
                                 TbPow(b,power),
                                 a);
        return result;
    }

    //! 設定
    void SetColorF32(TbFloat32 inR,TbFloat32 inG,TbFloat32 inB,TbFloat32 inA)
    {
        r = TbClamp<TbFloat32>( inR , 0.0f , 1.0f );
        g = TbClamp<TbFloat32>( inG , 0.0f , 1.0f );
        b = TbClamp<TbFloat32>( inB , 0.0f , 1.0f );
        a = TbClamp<TbFloat32>( inA , 0.0f , 1.0f );
    }

    //! 設定
    void SetColorF32(TbFloat32 inR,TbFloat32 inG,TbFloat32 inB)
    {
        r = TbClamp<TbFloat32>( inR , 0.0f , 1.0f );
        g = TbClamp<TbFloat32>( inG , 0.0f , 1.0f );
        b = TbClamp<TbFloat32>( inB , 0.0f , 1.0f );
    }

    //! 設定
    void SetRedF32( TbFloat32 inR )
    {
        r = TbClamp<TbFloat32>( inR , 0.0f , 1.0f );
    }

    //! 設定
    void SetGreenF32( TbFloat32 inG )
    {
        g = TbClamp<TbFloat32>( inG , 0.0f , 1.0f );
    }

    //! 設定
    void SetBlueF32( TbFloat32 inB )
    {
        b = TbClamp<TbFloat32>( inB , 0.0f , 1.0f );
    }

    //! 設定
    void SetAlphaF32(TbFloat32 inA)
    {
        a = TbClamp<TbFloat32>( inA , 0.0f , 1.0f );
    }

    //! 設定
    void SetColorU8( TbUint8 inR , TbUint8 inG , TbUint8 inB , TbUint8 inA )
    {
        r = static_cast<TbFloat32>(inR) / static_cast<TbFloat32>(0xFF);
        g = static_cast<TbFloat32>(inG) / static_cast<TbFloat32>(0xFF);
        b = static_cast<TbFloat32>(inB) / static_cast<TbFloat32>(0xFF);
        a = static_cast<TbFloat32>(inA) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetColorU8( TbUint8 inR , TbUint8 inG , TbUint8 inB )
    {
        r = static_cast<TbFloat32>(inR) / static_cast<TbFloat32>(0xFF);
        g = static_cast<TbFloat32>(inG) / static_cast<TbFloat32>(0xFF);
        b = static_cast<TbFloat32>(inB) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetColorU32( TbUint32 value )
    {
        a = static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_A(value)) / static_cast<TbFloat32>(0xFF);
        r = static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_R(value)) / static_cast<TbFloat32>(0xFF);
        g = static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_G(value)) / static_cast<TbFloat32>(0xFF);
        b = static_cast<TbFloat32>(TB_COLOR_A8R8G8B8_GET_B(value)) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetRedU8( TbUint8 inR )
    {
        r = static_cast<TbFloat32>(inR) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetGreenU8( TbUint8 inG )
    {
        g = static_cast<TbFloat32>(inG) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetBlueU8( TbUint8 inB )
    {
        b = static_cast<TbFloat32>(inB) / static_cast<TbFloat32>(0xFF);
    }

    //! 設定
    void SetAlphaU8( TbUint8 inA )
    {
        a = static_cast<TbFloat32>(inA) / static_cast<TbFloat32>(0xFF);
    }

    // 取得
    TbUint32 GetColorU32() const {
        TbUint32 value = TB_COLOR_A8R8G8B8( GetAlphaU8(), 
                                            GetRedU8(), 
                                            GetGreenU8(),
                                            GetBlueU8());
        return value;
    }

    //! u8型の赤成分取得
    TbUint8 GetRedU8() const 
    {
        return static_cast<TbUint8>(r * 255.0f);
    }

    //! u8型の緑成分取得
    TbUint8 GetGreenU8() const 
    {
        return static_cast<TbUint8>(g * 255.0f);
    }

    //! u8型の青成分取得
    TbUint8 GetBlueU8() const 
    {
        return static_cast<TbUint8>(b * 255.0f);
    }

    //! u8型のアルファ成分取得
    TbUint8 GetAlphaU8() const 
    {
        return static_cast<TbUint8>(a * 255.0f);
    }

    //! f32型の赤成分取得
    TbFloat32 GetRedF32() const 
    {
        return r;
    }

    //! f32型の緑成分取得    
    TbFloat32 GetGreenF32() const 
    {
        return g;
    }

    //! f32型の青成分取得
    TbFloat32 GetBlueF32() const 
    {
        return b;
    }

    //! f32型のアルファ成分取得
    TbFloat32 GetAlphaF32() const 
    {
        return a;
    }

    struct ValueType{
        TbFloat32 r;    //!< R成分( 0.0f - 1.0f )
        TbFloat32 g;    //!< G成分( 0.0f - 1.0f )
        TbFloat32 b;    //!< B成分( 0.0f - 1.0f )
        TbFloat32 a;    //!< A成分( 0.0f - 1.0f )
    };
    union{
        ValueType value;
        struct{
            TbFloat32 r;    //!< R成分( 0.0f - 1.0f )
            TbFloat32 g;    //!< G成分( 0.0f - 1.0f )
            TbFloat32 b;    //!< B成分( 0.0f - 1.0f )
            TbFloat32 a;    //!< A成分( 0.0f - 1.0f )
        };
        
    };
};

/*!
 * 乗算
 * @author Miyake Shunsuke
 * @since 2012.02.20
 */
inline TbColorU32 operator*(const TbColorU32& left , const TbColorU32& right )
{
    TbColorU32 result = TbColorU32::FromRgbaF32( left.GetRedF32() * right.GetRedF32() , 
                                                 left.GetGreenF32() * right.GetGreenF32() , 
                                                 left.GetBlueF32() * right.GetBlueF32() , 
                                                 left.GetAlphaF32() * right.GetAlphaF32() );
    return result;
}

/*!
* 乗算
* @author Miyake Shunsuke
* @since 2012.02.20
*/
inline TbColorU32 operator*(const TbColorU32& left, const TbColorF128& right)
{
    TbColorU32 result = TbColorU32::FromRgbaF32(left.GetRedF32() * right.GetRedF32(),
        left.GetGreenF32() * right.GetGreenF32(),
        left.GetBlueF32() * right.GetBlueF32(),
        left.GetAlphaF32() * right.GetAlphaF32());
    return result;
}

/*!
 * 乗算
 * @author Miyake Shunsuke
 * @since 2012.02.20
 */
inline TbColorF128 operator*(const TbColorF128& left , const TbColorF128& right )
{
    TbColorF128 result = TbColorF128::FromRgbaF32( left.GetRedF32() * right.GetRedF32() , 
                                                   left.GetGreenF32() * right.GetGreenF32() , 
                                                   left.GetBlueF32() * right.GetBlueF32() , 
                                                   left.GetAlphaF32() * right.GetAlphaF32() );
    return result;
}

/*!
* 乗算
* @author Miyake Shunsuke
* @since 2012.02.20
*/
inline TbColorF128 operator*(const TbColorF128& left, const TbColorU32& right)
{
    TbColorF128 result = TbColorF128::FromRgbaF32(left.GetRedF32() * right.GetRedF32(),
        left.GetGreenF32() * right.GetGreenF32(),
        left.GetBlueF32() * right.GetBlueF32(),
        left.GetAlphaF32() * right.GetAlphaF32());
    return result;
}

#if (TB_COLOR_TYPE == TB_COLOR_TYPE_U8)
    typedef TbColorU32 TbColor;
#elif (TB_COLOR_TYPE == TB_COLOR_TYPE_F32)
    typedef TbColorF128 TbColor;
#endif

}

#include "./tb_color_op.h"

#endif