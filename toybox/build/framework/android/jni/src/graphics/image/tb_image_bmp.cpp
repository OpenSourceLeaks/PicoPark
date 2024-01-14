/*!
 * @file
 * @brief BMP画像パーサークラスの実装ファイル
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */

#include "tb_fwd.h"
#include "graphics/image/tb_image_bmp.h"
#include "base/io/stream/tb_stream_binary_reader.h"
#include "base/io/stream/tb_const_memory_stream.h"

namespace toybox
{

namespace{
    
    //! BMPファイルのヘッダー構造
    struct BITMAPFILEHEADER
    {
        TbUint16 type;      //BMP画像かどうかの識別子
        TbUint32 size;      //ファイルのサイズ
        TbUint16 reserved1; //予約領域
        TbUint16 reserved2; //予約領域2
        TbUint32 offBits;    //ビットマップのオフセット値
    };
    static TbUint32 BITMAPFILEHEADER_SIZE = 14;

    //! BMPファイルの情報ヘッダー構造
    struct BITMAPINFOHEADER
    {
        TbUint32    size;           //情報ヘッダーのサイズ
        TbSint32    width;          //画像の横幅
        TbSint32    height;         //画像の縦幅
        TbUint16    planes;         //デバイス面数
        TbUint16    bitCount;       //1ピクセル毎のビット数
        TbUint32    compression;    //圧縮タイプ
        TbUint32    sizeImage;      //イメージデータのサイズ
        TbSint32    xPelsPerMeter;  //水平解像度1メートルあたり画素数
        TbSint32    yPelsPerMeter;  //垂直解像度1メートルあたり画素数
        TbUint32    colorUsed;      //使われているカラー数
        TbUint32    colorImportant; //重要な色の数
    };

    // パレット情報
    struct RGBQUAD
    {
        TbUint8 blue;   //青
        TbUint8 green;  //緑
        TbUint8 red;    //赤
        TbUint8 reserved;//予約
    };

    /**
    * ビットマップ情報
    */ 
    struct BITMAPINFO
    {
        BITMAPINFOHEADER header;
        RGBQUAD          colors[256];
    };
}
    
/*!
 * ファイルからピクセル配列を生成
 * とりあえず24bit/32bitカラーに対応
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */
TbBool TbImageBmp::Import( TB_OUT TbImageImportResult& result , 
                            const TbImageImportInfo& info , 
                            const TbImageAllocFunc allocFunc )
{
    const TbImageAllocFunc localAlloc = allocFunc ? allocFunc : TbImageAllocatorDefault::Alloc;
    if( !info.fileBuf || !info.fileBufSize ){
        return TB_E_INVALIDARG;
    }

#if 0
    // 指定しているカラーフォーマットに対応しているか
    if( TB_COLOR_FORMAT_UNKNOWN != info.colorFormat ) {
        if( TB_COLOR_FORMAT_B8G8R8A8 != info.colorFormat ){
            return TB_E_INVALIDARG;
        }
    }
#endif

    TbConstMemoryStream stream( reinterpret_cast<const TbUint8*>(info.fileBuf) , info.fileBufSize );
    TbStreamBinaryReader reader( &stream );

    TbUint32 infoSize = 0U;
    BITMAPFILEHEADER header;
    BITMAPINFO      bitmapInfo;
    TbUint8* writeBuffer = nullptr;
    TbUint32 imageSize = 0;

    //パディングを回避するために2バイトのみ先に取得
    TbUint16 headerUint16;
    if( !reader.TryReadUint16(&headerUint16) ){
        return TB_FALSE;
    }
    TbMemCopy( &header , &headerUint16 , sizeof(TbUint16) );

    if( !TB_SUCCEEDED( reader.Read( &header.size , 
                                   0 , 
                                   BITMAPFILEHEADER_SIZE-sizeof(TbUint16) ) ) ){
        return TB_FALSE;
    }

    //MBという記号がなければこれはBMP画像ではない
    if( header.type != TB_MULTI_CHARA_16('B','M') ){
        return TB_FALSE;
    }

    //情報部の情報取得
    infoSize = header.offBits - BITMAPFILEHEADER_SIZE;
    if( !TB_SUCCEEDED(reader.Read( &bitmapInfo , 0 , infoSize ) ) ){
        return TB_FALSE;
    }

    // 24bit/32bitにのみ対応
    switch( bitmapInfo.header.bitCount )
    {
    case 8:
    case 24:
    case 32:
        break;
    default:
        return TB_FALSE;
    }

    if( info.colorFormat == TB_COLOR_FORMAT_UNKNOWN ) {
        result.colorFormat = TB_COLOR_FORMAT_R8G8B8;
    }else{
        result.colorFormat = info.colorFormat;
    }

    //画像の縦幅と横幅取得
    result.width = bitmapInfo.header.width;
    result.height = bitmapInfo.header.height;
    imageSize = result.width * result.height;
    result.bufSize = result.width * result.height * TbGetColorFormatByteSize(result.colorFormat);

    // 出力バッファのサイズは足りているか
    if( !( result.buffer = localAlloc( result.bufSize ) ) ) {
        return TB_FALSE;
    }
    writeBuffer = reinterpret_cast<TbUint8*>(result.buffer);

    TbUint32 offsetR = 0;
    TbUint32 offsetG = 0;
    TbUint32 offsetB = 0;
    TbUint32 offsetA = 0;
    switch( result.colorFormat )
    {
    case TB_COLOR_FORMAT_R8G8B8:
        offsetR = 2;
        offsetG = 1;
        offsetB = 0;
        break;
    case TB_COLOR_FORMAT_A8R8G8B8:
        offsetA = 3;
        offsetR = 2;
        offsetG = 1;
        offsetB = 0;
        break;
    case TB_COLOR_FORMAT_A8B8G8R8:
        offsetA = 3;
        offsetR = 0;
        offsetG = 1;
        offsetB = 2;
        break;
    case TB_COLOR_FORMAT_B8G8R8A8:
        offsetA = 0;
        offsetR = 1;
        offsetG = 2;
        offsetB = 3;
        break;
    default:
        TB_ASSERT(!"unsupported");
        break;
    }

    // バイナリにはBGR順に格納されてあるので、反転
    // 上から格納する
    TbUint32 index = 0;
    if( 8 == bitmapInfo.header.bitCount ){
        TbUint8 paletteIndex = 0;
        switch( result.colorFormat )
        {
        case TB_COLOR_FORMAT_R8G8B8:
            for(TbUint32 i=0;i < (imageSize) ;i++){
                index = imageSize - i - 1;
                paletteIndex = reader.ReadUint8();
                writeBuffer[ index*3 + offsetB ] = bitmapInfo.colors[ paletteIndex ].blue;
                writeBuffer[ index*3 + offsetG ] = bitmapInfo.colors[ paletteIndex ].green;
                writeBuffer[ index*3 + offsetR ] = bitmapInfo.colors[ paletteIndex ].red;
            }
            break;
        case TB_COLOR_FORMAT_B8G8R8A8:
        case TB_COLOR_FORMAT_A8R8G8B8:
        case TB_COLOR_FORMAT_A8B8G8R8:
            for(TbUint32 i=0;i < (imageSize) ;i++){
                index = imageSize - i - 1;
                paletteIndex = reader.ReadUint8();
                writeBuffer[ index*4 + offsetA ] = 0xFF;
                writeBuffer[ index*4 + offsetR ] = bitmapInfo.colors[ paletteIndex ].red;
                writeBuffer[ index*4 + offsetG ] = bitmapInfo.colors[ paletteIndex ].green;
                writeBuffer[ index*4 + offsetB ] = bitmapInfo.colors[ paletteIndex ].blue;
            }
        default:
            TB_ASSERT(!"unsupported");
            break;
        }
    }else{
        switch( result.colorFormat )
        {
        case TB_COLOR_FORMAT_R8G8B8:
            for(TbUint32 i=0;i < (imageSize) ;i++){
                index = imageSize - i - 1;
                writeBuffer[ index*3 + offsetB ] = reader.ReadUint8();
                writeBuffer[ index*3 + offsetG ] = reader.ReadUint8();
                writeBuffer[ index*3 + offsetR] = reader.ReadUint8();
                if( 32 == bitmapInfo.header.bitCount ){
                    reader.ReadUint8();
                }
            }
            break;
        case TB_COLOR_FORMAT_B8G8R8A8:
        case TB_COLOR_FORMAT_A8R8G8B8:
        case TB_COLOR_FORMAT_A8B8G8R8:
            for(TbUint32 i=0;i < (imageSize) ;i++){
                index = imageSize - i - 1;
                writeBuffer[ index*4 + offsetB  ] = reader.ReadUint8();
                writeBuffer[ index*4 + offsetG ] = reader.ReadUint8();
                writeBuffer[ index*4 + offsetR ] = reader.ReadUint8();
                if( 32 == bitmapInfo.header.bitCount ){
                    writeBuffer[ index*4 + offsetA ] = reader.ReadUint8();
                }else{
                    writeBuffer[ index*4 + offsetA ] = 0xFF;
                }
            }
            break;
        default:
            TB_ASSERT(!"unsupported");
            break;
        }
    }

    return TB_TRUE;
}

/*!
 * ピクセル配列からファイル出力用バッファ
 * とりあえず24bit/32bitカラーに対応
 * 出力は現状24bitのみ
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */
TbBool TbImageBmp::Export( TB_OUT TbImageExportResult& result , 
                            const TbImageExportInfo& info , 
                            const TbImageAllocFunc allocFunc )
{
    const TbImageAllocFunc localAlloc = allocFunc ? allocFunc : TbImageAllocatorDefault::Alloc;
    TbColorFormat outputFormat = TB_COLOR_FORMAT_R8G8B8;

    if( info.colorFormat != TB_COLOR_FORMAT_A8R8G8B8 &&
        info.colorFormat != TB_COLOR_FORMAT_X8R8G8B8 &&
        info.colorFormat != TB_COLOR_FORMAT_R8G8B8 )
    {
        // 未対応
        return TB_FALSE;
    }

    // ヘッダー
    BITMAPFILEHEADER header = 
    {
        TB_MULTI_CHARA_16('M','B') , 
        0 , 
        0 , 
        0 , 
        static_cast<TbUint32>(BITMAPFILEHEADER_SIZE + sizeof(BITMAPINFOHEADER))
    };

    // BMPファイルの情報ヘッダー
    BITMAPINFOHEADER infoHeader = 
    {
        sizeof(BITMAPINFOHEADER) , // 情報ヘッダーのサイズ
        static_cast<TbSint32>(info.width) , // 画像の横幅
        static_cast<TbSint32>(info.height) , // 画像の縦幅
        1 , // デバイス面数
        TbGetColorFormatBitSize(outputFormat) , //1ピクセル毎のビット数
        0 , //圧縮タイプ
        info.width * info.height * TbGetColorFormatBitSize(outputFormat)/8 ,  //イメージデータのサイズ
        0 ,  //水平解像度1メートルあたり画素数
        0 ,  //垂直解像度1メートルあたり画素数
        0 ,  //使われているカラー数
        0 //重要な色の数
    };
    
    TbUint32 pitch = info.width * TbGetColorFormatBitSize(TB_COLOR_FORMAT_A8R8G8B8)/8;
    header.size = BITMAPFILEHEADER_SIZE + sizeof(BITMAPINFOHEADER) + ( pitch * info.height );

    // これでファイルバッファ
    TbUint8* fileBuf = reinterpret_cast<TbUint8*>(localAlloc( header.size ));

    TbMemCopy( fileBuf , &header , sizeof(TbUint16) );
    TbMemCopy( &fileBuf[sizeof(TbUint16)] , &header.size , BITMAPFILEHEADER_SIZE-sizeof(TbUint16) );
    TbMemCopy( &fileBuf[BITMAPFILEHEADER_SIZE] , &infoHeader , sizeof(BITMAPINFOHEADER) );

    TbUint8* colorBuf = &fileBuf[ header.offBits ];
    TbUint32 color = 0;
    if( info.colorFormat == TB_COLOR_FORMAT_A8R8G8B8 ||
        info.colorFormat == TB_COLOR_FORMAT_X8R8G8B8 )
    {
        // 32bit
        for( TbUint32 y = 0; y < info.height; ++y ){
            for( TbUint32 x = 0; x < info.width; ++x ){
                color = reinterpret_cast<const TbUint32*>(info.buffer)[(y*info.width)+x];
                TbUint32 writeIndex = x;
                if( info.flag & TbImageExportInfo::FLAG_BIT_RIGHT_TO_LEFT ){
                    writeIndex = info.width - 1 - x;
                }
                if( info.flag & TbImageExportInfo::FLAG_BIT_DOWN_TO_UP ){
                    writeIndex += y * info.width;
                }else{
                    writeIndex += ( info.height - 1 - y ) * info.width;
                }
                colorBuf[ writeIndex * 3 ] = TB_COLOR_A8R8G8B8_GET_B( color );
                colorBuf[ writeIndex * 3 + 1 ] = TB_COLOR_A8R8G8B8_GET_G( color );
                colorBuf[ writeIndex * 3 + 2 ] = TB_COLOR_A8R8G8B8_GET_R( color );
            }
        }
    }else if( info.colorFormat == TB_COLOR_FORMAT_R8G8B8 ){
        // 24bit
        const TbUint8* bufferU8 = reinterpret_cast<const TbUint8*>(info.buffer);
        for( TbUint32 y = 0; y < info.height; ++y ){
            for( TbUint32 x = 0; x < info.width; ++x ){
                color = *reinterpret_cast<const TbUint32*>(&bufferU8[(y*info.width+x)*3]);
                TbUint32 writeIndex = x;
                if( info.flag & TbImageExportInfo::FLAG_BIT_RIGHT_TO_LEFT ){
                    writeIndex = info.width - 1 - x;
                }
                if( info.flag & TbImageExportInfo::FLAG_BIT_DOWN_TO_UP ){
                    writeIndex += y * info.width;
                }else{
                    writeIndex += ( info.height - 1 - y ) * info.width;
                }
                colorBuf[ writeIndex * 3 ] = TB_COLOR_R8G8B8_GET_B( color );
                colorBuf[ writeIndex * 3 + 1 ] = TB_COLOR_R8G8B8_GET_G( color );
                colorBuf[ writeIndex * 3 + 2 ] = TB_COLOR_R8G8B8_GET_R( color );
            }
        }
    }else{
        TB_ASSERT(!"Unknown Error");
    }

    result.fileBuf = fileBuf;
    result.fileBufSize = header.size;
    return TB_TRUE;
}

}