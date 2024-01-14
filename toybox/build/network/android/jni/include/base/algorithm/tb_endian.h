/*!
 * エンディアン
 * @author Miyake_Shunsuke
 * @since 2012.04.30
 */

#ifndef _INCLUDED_TB_ENDIAN_H_
#define _INCLUDED_TB_ENDIAN_H_

namespace toybox
{
    enum EndianType
    {
        ENDIAN_TYPE_LITTLE , // リトルエンディアン
        ENDIAN_TYPE_BIG ,    // ビッグエンディアン
    };

    //! u16エンディアン変換
    TbUint16 TbSwapEndian( const TbUint16& value ) {
        TbUint16 result = (( value & 0xFF00 ) >> 8 ) |
                          (( value & 0x00FF ) << 8 );
        return result;
    }

    //! s16エンディアン変換
    TbSint16 TbSwapEndian( const TbSint16& value ) {
        TbUint16* valU16 = reinterpret_cast<TbUint16*>(&value);
        TbUint16  result = TbSwapEndian( *valU16 );
        return *reinterpret_cast<TbSint16*>(&result);
    }

    //! u32エンディアン変換
    TbUint32 TbSwapEndian( const TbUint32& value ) {
        TbUint32 result = (( value & 0xFF000000 ) >> 24 ) |
                          (( value & 0x00FF0000 ) >> 8 )  |
                          (( value & 0x0000FF00 ) << 8 )  | 
                          (( value & 0x000000FF ) << 24 );
        return result;
    }

    //! s32エンディアン変換
    TbSint32 TbSwapEndian( const TbSint32& value ) {
        TbUint32* valU32 = reinterpret_cast<TbUint32*>(&value);
        TbUint32  result = TbSwapEndian( *valU32 );
        return *reinterpret_cast<TbSint32*>(&result);
    }

    //! f32エンディアン変換
    TbFloat32 TbSwapEndian( const TbFloat32& value ) {
        TbUint32* valU32 = reinterpret_cast<TbUint32*>(&value);
        TbUint32  result = TbSwapEndian( *valU32 );
        return *reinterpret_cast<TbFloat32*>(&result);
    }

}

#endif
