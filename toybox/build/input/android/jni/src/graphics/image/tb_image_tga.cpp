/*!
 * @file
 * @brief BMP画像パーサークラスの実装ファイル
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */

#include "tb_fwd.h"
#include "graphics/image/tb_image_tga.h"
#include "base/io/stream/tb_stream_binary_reader.h"
#include "base/io/stream/tb_const_memory_stream.h"

namespace toybox
{

namespace{
    
    struct TgaFileHeader
    {
        TbUint8  idFieldLength;  // IDフィールド長さ
        TbUint8  colorMapType;   // カラーマップの有無 0=無し 1=有り
        TbUint8  imageType;      // 画像形式
        TbUint16 colorMapIndex;  // カラーマップ開始エントリー
        TbUint16 colorMapLength; // カラーマップエントリーサイズ
        TbUint8  colorMapSize;   // カラーマップのビットサイズ
        TbUint16 imageOriginX;   //　
        TbUint16 imageOriginY;   // 

        TbUint16 imageWidth;
        TbUint16 imageHeight;
        TbUint8  bitPerPixel;
        TbUint8  discripter;
    };
    static TbUint32 TGA_FILE_HEADER_SIZE = 18;

    enum TgaImageType
    {
        TGA_IMAGE_TYPE_NONE = 0 ,
        TGA_IMAGE_TYPE_INDEXED_COLOR = 1 ,
        TGA_IMAGE_TYPE_FULL_COLOR = 2 , 
        TGA_IMAGE_TYPE_GRAY = 3 , 
        TGA_IMAGE_TYPE_INDEXED_COLOR_RLE = 9 ,
        TGA_IMAGE_TYPE_FULL_COLOR_RLE = 10, 
        TGA_IMAGE_TYPE_GRAY_RLE = 11 , 
    };
    
    enum TgaAttributeBit
    {
        TGA_ATTRIBUTE_RIGHT_TO_LEFT = TB_BIT(4) , // 右から左に格納されている
        TGA_ATTRIBUTE_UP_TO_DOWN  = TB_BIT(5) , // 上から下に格納されている
    };

}

struct TgaFileParam
{
    TgaFileHeader header;
    TbUint8 bytePerPixel;
    TbUint8 writeBytePerPixel;
    TbColorFormat colorFormat;
    TbColorFormat writeColorFormat;
    TbUint32 imageSize;
    TbUint8* colorMap;
};

// カラーセット関数型
typedef void (*SetColorFunc)( TB_OUT TbUint8* writeBuffer ,
                              TB_INOUT TbStreamBinaryReader& reader ,
                              const TbUint32& count ,
                              const TbUint32 startIndex , 
                              const TgaFileParam& param );

inline void writeColor( TbUint8* buf , TbUint8 r , TbUint8 g , TbUint8 b , TbUint8 a , const TgaFileParam& param )
{
    TbBool canWriteAlpha = (4 == param.writeBytePerPixel);
    TbUint32 offsetR = 0;
    TbUint32 offsetG = 0;
    TbUint32 offsetB = 0;
    TbUint32 offsetA = 0;
    switch( param.writeColorFormat )
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
        break;
    }
    if( a > 0 ) {
        int a = 0;
    }
    buf[offsetR] = r;
    buf[offsetG] = g;
    buf[offsetB] = b;
    if( canWriteAlpha ) {
        buf[offsetA] = a;
    }
}

// 書き込むインデックス取得
inline TbUint32 getWriteIndex( const TgaFileParam& param , const TbUint32& startIndex )
{
    TbUint32 indexX = ( (startIndex) % param.header.imageWidth );
    // 左右反転
    if( param.header.discripter & TGA_ATTRIBUTE_RIGHT_TO_LEFT ){
        indexX = ( param.header.imageWidth - 1 - indexX );
    }
    TbUint32 indexY = ( (startIndex) / param.header.imageWidth );
    // 上下反転
    if( !( param.header.discripter & TGA_ATTRIBUTE_UP_TO_DOWN ) ){
        indexY = ( param.header.imageHeight - 1 - indexY );
    }
    return ( indexY * param.header.imageWidth + indexX ) * param.writeBytePerPixel;
}

// インデックスカラー( 非圧縮 )
inline void setIndexColor( TB_OUT TbUint8* writeBuffer ,
                           TB_INOUT TbStreamBinaryReader& reader ,
                           const TbUint32& count ,
                           const TbUint32 startIndex , 
                           const TgaFileParam& param )
{
    TbBool hasAlpha = ( 32 == param.header.colorMapSize );
    TbUint8 paletteIndex = 0;
    TbUint8 blue = 0;
    TbUint8 green = 0;
    TbUint8 red = 0;
    TbUint8 alpha = 0;
    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        paletteIndex = reader.ReadUint8();
        paletteIndex = paletteIndex * ( param.header.colorMapSize / 8 );
        blue = param.colorMap[ paletteIndex ];
        green = param.colorMap[ paletteIndex+1 ];
        red = param.colorMap[ paletteIndex+2 ];
        alpha = hasAlpha ? param.colorMap[ paletteIndex+3 ] : 0xFF;
        index = getWriteIndex( param , startIndex+i );
        writeColor( &writeBuffer[index] , red , green , blue , alpha , param );
    }
}

// インデックスカラー( RLE圧縮 )
inline void setIndexColorRLE( TB_OUT TbUint8* writeBuffer ,
                             TB_INOUT TbStreamBinaryReader& reader ,
                             const TbUint32& count ,
                             const TbUint32 startIndex , 
                             const TgaFileParam& param )
{
    TbBool hasAlpha = ( 32 == param.header.colorMapSize );
    TbUint8 paletteIndex = reader.ReadUint8();
    paletteIndex = paletteIndex * ( param.header.colorMapSize / 8 );
    TbUint8 blue = param.colorMap[ paletteIndex ];
    TbUint8 green = param.colorMap[ paletteIndex+1 ];
    TbUint8 red = param.colorMap[ paletteIndex+2 ];
    TbUint8 alpha = hasAlpha ? param.colorMap[ paletteIndex+3 ] : 0xFF;

    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        index = getWriteIndex( param , startIndex+i );
        writeColor( &writeBuffer[index] , red , green , blue , alpha , param );
    }
}

// フルカラー( 非圧縮 )
inline void setFullColor( TB_OUT TbUint8* writeBuffer ,
                          TB_INOUT TbStreamBinaryReader& reader ,
                          const TbUint32& count ,
                          const TbUint32 startIndex , 
                          const TgaFileParam& param )
{
    TbBool hasAlpha = ( 4 == param.bytePerPixel );
    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        index = getWriteIndex( param , startIndex+i );
        TbUint8 blue   = reader.ReadUint8();
        TbUint8 green = reader.ReadUint8();
        TbUint8 red = reader.ReadUint8();
        TbUint8 alpha = 0xFF;
        if( hasAlpha ){
            alpha = reader.ReadUint8();
        }
        writeColor( &writeBuffer[index] , red , green , blue , alpha , param );
    }
}

// フルカラー( RLE圧縮 )
inline void setFullColorRLE( TB_OUT TbUint8* writeBuffer ,
                             TB_INOUT TbStreamBinaryReader& reader ,
                             const TbUint32& count ,
                             const TbUint32 startIndex , 
                             const TgaFileParam& param )
{
    TbBool hasAlpha = ( 4 == param.bytePerPixel );
    TbUint8 blue = reader.ReadUint8();
    TbUint8 green = reader.ReadUint8();
    TbUint8 red = reader.ReadUint8();
    TbUint8 alpha = hasAlpha ? reader.ReadUint8() : 0xFF;
    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        index = getWriteIndex( param , startIndex+i );
        writeColor( &writeBuffer[index] , red , green , blue , alpha , param );
    }
}

// グレーカラー( 非圧縮 )
inline void setGrayColor( TB_OUT TbUint8* writeBuffer ,
                          TB_INOUT TbStreamBinaryReader& reader ,
                          const TbUint32& count ,
                          const TbUint32 startIndex , 
                          const TgaFileParam& param )
{
    TbUint8 colorValue = 0;
    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        index = getWriteIndex( param , startIndex+i );
        colorValue = reader.ReadUint8();
        writeColor( &writeBuffer[index] , colorValue , colorValue , colorValue , 0xFF , param );
    }
}

// グレーカラー( RLE圧縮 )
inline void setGrayColorRLE( TB_OUT TbUint8* writeBuffer ,
                             TB_INOUT TbStreamBinaryReader& reader ,
                             const TbUint32& count ,
                             const TbUint32 startIndex ,
                             const TgaFileParam& param )
{
    TbUint8 colorValue = reader.ReadUint8();
    TbUint32 index = 0;
    for( TbUint32 i =0; i < count; ++i ){
        index = getWriteIndex( param , startIndex+i );
        writeColor( &writeBuffer[index] , colorValue , colorValue , colorValue , 0xFF , param );
    }
}

/*!
 * RLE圧縮による色設定
 * @author Miyake Shunsuke
 * @since 2011.05.09
 */
void setColorRLE( TB_OUT TbUint8* writeBuffer ,
                  TB_INOUT TbStreamBinaryReader& reader ,  
                  const TgaFileParam& param , 
                  SetColorFunc setColor , 
                  SetColorFunc setColorRLE )
{
    TbUint8 packet = 0;
    TbUint8 count = 0;
    TbUint32 writeIndex = 0;
    while ( writeIndex < param.imageSize )
    {
        packet = reader.ReadUint8();
        if( packet & 0x80 ){ // RLE Data
            count = packet - 127;
            setColorRLE( writeBuffer , 
                         reader , 
                         count , 
                         writeIndex ,
                         param );
        }else{ // Raw Data
            count = packet + 1;
            setColor( writeBuffer , 
                      reader , 
                      count , 
                      writeIndex ,
                      param );            
        }
        writeIndex += count;
    }
}
   
/*!
 * ファイルからピクセル配列を生成
 * とりあえず24bit/32bitカラーに対応
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */
TbBool TbImageTga::Import( TB_OUT TbImageImportResult& result , 
                           const TbImageImportInfo& info , 
                           const TbImageAllocFunc allocFunc )
{
    const TbImageAllocFunc localAlloc = allocFunc ? allocFunc : TbImageAllocatorDefault::Alloc;
    if( !info.fileBuf || !info.fileBufSize ){
        return TB_FALSE;
    }

    TbConstMemoryStream stream( reinterpret_cast<const TbUint8*>(info.fileBuf) , info.fileBufSize );
    TbStreamBinaryReader reader( &stream );

    const void* readBuf = nullptr;
    TbUint8* writeBuffer = nullptr;
    TbUint32 colorMapBytePerPixel = 0;
    TgaFileParam param;
    TbMemClear( &param , sizeof(param) );

    //パディングを回避するために2バイトのみ先に取得
    param.header.idFieldLength = reader.ReadUint8();
    param.header.colorMapType = reader.ReadUint8();
    param.header.imageType = reader.ReadUint8();
    param.header.colorMapIndex = reader.ReadUint16();
    param.header.colorMapLength = reader.ReadUint16();
    param.header.colorMapSize = reader.ReadUint8();
    param.header.imageOriginX = reader.ReadUint16();
    param.header.imageOriginY = reader.ReadUint16();
    param.header.imageWidth = reader.ReadUint16();
    param.header.imageHeight = reader.ReadUint16();
    param.header.bitPerPixel = reader.ReadUint8();
    param.header.discripter = reader.ReadUint8();
    
    // IDフィールドは無視
    if( param.header.idFieldLength > 0 ){
        reader.Read( param.header.idFieldLength );
    }

    // カラーマップ
    if( param.header.colorMapType ){
        if( 15 == param.header.colorMapSize ){ // 15bitサイズのカラーマップは非対応
            return TB_FALSE;
        }
        colorMapBytePerPixel = param.header.colorMapSize / 8;
        TbUint32 colorMapBufSize = param.header.colorMapLength * colorMapBytePerPixel;
        param.colorMap = TB_NEW TbUint8[ colorMapBufSize ];
        reader.Read( param.colorMap , 0 , colorMapBufSize );
    }

    // 24/32bitのみ対応
    switch( param.header.bitPerPixel )
    {
    case 8:
        {
            if( 32 == param.header.colorMapSize ){
                param.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
            }else{
                param.colorFormat = TB_COLOR_FORMAT_R8G8B8;
            }
        }
        break;
    case 24:
        param.colorFormat = TB_COLOR_FORMAT_R8G8B8;
        break;
    case 32:
        param.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
        break;
    default:
        // 未サポート
        return TB_FALSE;
        break;
    }

    param.bytePerPixel = TbGetColorFormatByteSize(param.colorFormat);
    if( info.colorFormat != TB_COLOR_FORMAT_UNKNOWN )
    {
        // BGRAのみ対応
        if( info.colorFormat != TB_COLOR_FORMAT_B8G8R8A8 &&
            info.colorFormat != TB_COLOR_FORMAT_A8B8G8R8 &&
            info.colorFormat != TB_COLOR_FORMAT_A8R8G8B8 )
        {
            return TB_FALSE;
        }
        param.writeColorFormat = info.colorFormat;
        param.writeBytePerPixel = TbGetColorFormatByteSize(info.colorFormat);
    }
    else
    {
        // ファイルのフォーマットそのまま利用
        param.writeColorFormat = param.colorFormat;
        param.writeBytePerPixel = param.bytePerPixel;
    }
    result.colorFormat = param.writeColorFormat;

    //画像の縦幅と横幅取得
    result.width = param.header.imageWidth;
    result.height = param.header.imageHeight;
    param.imageSize = result.width * result.height;
    result.bufSize = result.width * result.height * param.writeBytePerPixel;

    // 出力バッファのサイズは足りているか
    if( !( result.buffer = localAlloc( result.bufSize ) ) ) {
        TB_SAFE_DELETE_ARRAY( param.colorMap );
        return TB_FALSE;
    }
    writeBuffer = reinterpret_cast<TbUint8*>(result.buffer);

    switch( param.header.imageType )
    {
    case TGA_IMAGE_TYPE_INDEXED_COLOR:
        {
            setIndexColor( writeBuffer , reader , param.imageSize , 0U , param );
        }
        break;
    case TGA_IMAGE_TYPE_FULL_COLOR:
        {
            setFullColor( writeBuffer , reader , param.imageSize , 0U ,  param );
        }
        break;
    case TGA_IMAGE_TYPE_GRAY:
        {
            setGrayColor( writeBuffer , reader , param.imageSize , 0U , param );
        }
        break;
    case TGA_IMAGE_TYPE_INDEXED_COLOR_RLE:
        {
            setColorRLE( writeBuffer , 
                         reader , 
                         param , 
                         setIndexColor , 
                         setIndexColorRLE );
        }
        break;
    case TGA_IMAGE_TYPE_FULL_COLOR_RLE:
        {
            setColorRLE( writeBuffer , 
                         reader , 
                         param , 
                         setFullColor , 
                         setFullColorRLE );
        }
        break;
    case TGA_IMAGE_TYPE_GRAY_RLE:
        {
            setColorRLE( writeBuffer , 
                         reader , 
                         param , 
                         setGrayColor , 
                         setGrayColorRLE );
        }
        break;
    };
    TB_SAFE_DELETE_ARRAY( param.colorMap );
    return TB_TRUE;
}

/*!
 * ピクセル配列から画像ファイル出力
 * @author Miyake Shunsuke
 * @since 2011.08.25
 */
TbBool TbImageTga::Export( TB_OUT TbImageExportResult& result , const TbImageExportInfo& info , const TbImageAllocFunc allocFunc )
{
    const TbImageAllocFunc localAlloc = allocFunc ? allocFunc : TbImageAllocatorDefault::Alloc;
    TbColorFormat exportFormat = TB_COLOR_FORMAT_A8R8G8B8;

    if( info.colorFormat != TB_COLOR_FORMAT_A8R8G8B8 &&
        info.colorFormat != TB_COLOR_FORMAT_X8R8G8B8 )
    {
        // 未対応
        return TB_FALSE;
    }

    TgaFileHeader header;
    header.idFieldLength = 0;
    header.colorMapType = 0;
    header.imageType = TGA_IMAGE_TYPE_FULL_COLOR;
    header.colorMapIndex = 0;
    header.colorMapLength = 0;
    header.colorMapSize = 0;
    header.imageOriginX = 0;
    header.imageOriginY = 0;
    header.imageWidth = static_cast<TbUint16>(info.width);
    header.imageHeight = static_cast<TbUint16>(info.height);
    header.bitPerPixel = TbGetColorFormatBitSize( exportFormat );
    header.discripter = 8;
    if( !(info.flag & TbImageExportInfo::FLAG_BIT_DOWN_TO_UP) ){
        header.discripter |= TGA_ATTRIBUTE_UP_TO_DOWN;
    }
    if( info.flag & TbImageExportInfo::FLAG_BIT_RIGHT_TO_LEFT ){
        header.discripter |= TGA_ATTRIBUTE_RIGHT_TO_LEFT;
    }

    result.fileBufSize = TGA_FILE_HEADER_SIZE + ( header.imageWidth * header.imageHeight * header.bitPerPixel / 8 );
    TbUint8* fileBuf = reinterpret_cast<TbUint8*>(localAlloc( result.fileBufSize ));

    // todo:Writerクラスを作って使うこと
    TbUint8* currentBuf = fileBuf;
    TbMemCopy( currentBuf , &header.idFieldLength , sizeof(header.idFieldLength) ); currentBuf += sizeof(header.idFieldLength);
    TbMemCopy( currentBuf , &header.colorMapType , sizeof(header.colorMapType) ); currentBuf += sizeof(header.colorMapType);
    TbMemCopy( currentBuf , &header.imageType , sizeof(header.imageType) ); currentBuf += sizeof(header.imageType);
    TbMemCopy( currentBuf , &header.colorMapIndex , sizeof(header.colorMapIndex) ); currentBuf += sizeof(header.colorMapIndex);
    TbMemCopy( currentBuf , &header.colorMapLength , sizeof(header.colorMapLength) ); currentBuf += sizeof(header.colorMapLength);
    TbMemCopy( currentBuf , &header.colorMapSize , sizeof(header.colorMapSize) ); currentBuf += sizeof(header.colorMapSize);
    TbMemCopy( currentBuf , &header.imageOriginX , sizeof(header.imageOriginX) ); currentBuf += sizeof(header.imageOriginX);
    TbMemCopy( currentBuf , &header.imageOriginY , sizeof(header.imageOriginY) ); currentBuf += sizeof(header.imageOriginY);
    TbMemCopy( currentBuf , &header.imageWidth , sizeof(header.imageWidth) ); currentBuf += sizeof(header.imageWidth);
    TbMemCopy( currentBuf , &header.imageHeight , sizeof(header.imageHeight) ); currentBuf += sizeof(header.imageHeight);
    TbMemCopy( currentBuf , &header.bitPerPixel , sizeof(header.bitPerPixel) ); currentBuf += sizeof(header.bitPerPixel);
    TbMemCopy( currentBuf , &header.discripter , sizeof(header.discripter) ); currentBuf += sizeof(header.discripter);
    TbUint8* colorBuf = currentBuf;

    TbUint32 color = 0;
    switch( header.imageType )
    {
    case TGA_IMAGE_TYPE_INDEXED_COLOR:
        break;
    case TGA_IMAGE_TYPE_FULL_COLOR:
        {
            if( info.colorFormat == TB_COLOR_FORMAT_A8R8G8B8 ||
                info.colorFormat == TB_COLOR_FORMAT_X8R8G8B8 )
            {
                // 32bit
                for( TbUint32 y = 0; y < info.height; ++y ){
                    for( TbUint32 x = 0; x < info.width; ++x ){
                        color = reinterpret_cast<const TbUint32*>(info.buffer)[(y*info.width)+x];
                        TbUint32 writeIndex = y * info.width + x;
                        colorBuf[ writeIndex * 4 ] = TB_COLOR_A8R8G8B8_GET_B( color );
                        colorBuf[ writeIndex * 4 + 1 ] = TB_COLOR_A8R8G8B8_GET_G( color );
                        colorBuf[ writeIndex * 4 + 2 ] = TB_COLOR_A8R8G8B8_GET_R( color );
                        if( info.colorFormat == TB_COLOR_FORMAT_X8R8G8B8 ){
                            colorBuf[ writeIndex * 4 + 3 ] = 0xFF;
                        }else{
                            colorBuf[ writeIndex * 4 + 3 ] = TB_COLOR_A8R8G8B8_GET_A( color );
                        }
                    }
                }
            }
        }
        break;
    case TGA_IMAGE_TYPE_GRAY:
    case TGA_IMAGE_TYPE_INDEXED_COLOR_RLE:
    case TGA_IMAGE_TYPE_FULL_COLOR_RLE:
    case TGA_IMAGE_TYPE_GRAY_RLE:
        break;
    };

    result.fileBuf = fileBuf;
    return TB_TRUE;
}

}