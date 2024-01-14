/*!
 * @file
 * @brief BMP画像パーサークラスの実装ファイル
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */

#include "tb_fwd.h"
#include "graphics/font/tb_font.h"

#include "base/crypt/tb_crc32.h"
#include "base/algorithm/tb_search.h"
#include "base/algorithm/tb_sort.h"
#include "base/string/tb_string_code_page.h"
#include "base/util/tb_adapter_iterator.h"
#include "./tb_font_types_ascii.hpp"
#include "./tb_font_types_sjis.hpp"

namespace toybox
{

namespace
{

const TbFontCreateParam DEFAULT_PARAM_COMMON_ASCII = { FONT_ASCII_TILE_SIZE , 
                                          TbDimensionF32(0.062500f,0.062500f) , 
                                          TB_ARRAY_COUNT_OF(FONT_ASCII_INFO) 
                                        };
const TbFontCreateParamAscii DEFAULT_PARAM_ASCII( DEFAULT_PARAM_COMMON_ASCII , FONT_ASCII_INFO );

}

/*!
 * 改行コード検出
 * @author teco
 */
const char* TbFont::GetNewLine(TbBool& isNewLine, const char* ch)
{
    const char* result = ch;
    if (*result == '\n')
    {
        ++result;
        if (*result == '\r') {
            ++result;
        }
        isNewLine = TB_TRUE;
    }
    return result;
}

/*!
 * ASCIIフォントコンスデフォルトトラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.26
 */
TbFontAscii::TbFontAscii()
    : Super( TB_FONT_CHAR_CODE_ASCII , DEFAULT_PARAM_COMMON_ASCII )
{
    TbSizeT charCount = GetCharCount();
    m_CharInfo = TB_NEW CharInfo[ charCount ];
    for( TbUint32 i = 0; i < charCount; ++i ){
        m_CharInfo[i].uvRect = DEFAULT_PARAM_ASCII.uvRectList[i];
    }
}

/*!
 * ASCIIフォントコンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.26
 */
TbFontAscii::TbFontAscii( const TbFontCreateParamAscii& param )
    : Super( TB_FONT_CHAR_CODE_ASCII , param.common )
{
    TbSizeT charCount = GetCharCount();
    m_CharInfo = TB_NEW CharInfo[ charCount ];
    for( TbUint32 i = 0; i < charCount; ++i ){
        m_CharInfo[i].uvRect = param.uvRectList[i];
    }
}
    
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.26
 */
TbFontAscii::~TbFontAscii()
{
    TB_SAFE_DELETE_ARRAY(m_CharInfo);
}

/*!
 * サイズ取得
 * @return 次の文字
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
const char* TbFontAscii::GetSize( TbDimensionF32& size , const TbUint32 fontSize , const char* ch )
{
    TbUint32 index = static_cast<TbUint32>(*ch);
    if( index >= GetCharCount() ){
        return TB_FALSE;
    }
    TbRectF32& uvRect = m_CharInfo[index].uvRect;
    const TbDimensionF32& baseUVSize = GetBaseUVSize();
    size.width = ( uvRect.width / baseUVSize.width ) * static_cast<TbFloat32>(fontSize);
    size.height = ( uvRect.height / baseUVSize.height ) * static_cast<TbFloat32>(fontSize);
    return ch+1;
}

/*!
 * UV座標取得DD
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
const char* TbFontAscii::GetUV( TbRectF32& uv , const char* ch )
{
    TbUint32 index = static_cast<TbUint32>(*ch);
    if( index >= GetCharCount() ){
        return TB_FALSE;
    }
    uv = m_CharInfo[index].uvRect;
    return ch+1;
}

/*!
 * UV座標取得(返り値は次の文字)
 * @return 次の文字
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
const char* TbFontAscii::GetUV( TbRectF32& uv , TbDimensionF32& size , const TbUint32 fontSize , const char* ch )
{
    const char* result = GetUV(uv,ch);
    GetSize( size , fontSize , ch );
    return result;
}

/*!
 * SJISフォントコンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.26
 */
TbFontSjis::TbFontSjis( const TbFontCreateParamSjis& param )
    : Super( TB_FONT_CHAR_CODE_SJIS , param.common )
{
    TbSizeT charCount = GetCharCount();
    m_CharInfo = TB_NEW CharInfo[ charCount ];
    for( TbUint32 i = 0; i < charCount; ++i ){
        m_CharInfo[i].hash = TbCrc32::GetHash( reinterpret_cast<const TbUint8*>(param.charInfoList[i].target) , 
                                                TbStrCharCount(param.charInfoList[i].target) * sizeof(param.charInfoList[i].target[0]) );
        m_CharInfo[i].uvRect.x = param.charInfoList[i].x;
        m_CharInfo[i].uvRect.y = param.charInfoList[i].y;
        m_CharInfo[i].uvRect.width = param.charInfoList[i].width;
        m_CharInfo[i].uvRect.height = param.charInfoList[i].height;
    }
    TbInsertionSort( &m_CharInfo[0] , &m_CharInfo[charCount] , Compare() );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.12.26
 */
TbFontSjis::~TbFontSjis()
{
    TB_SAFE_DELETE_ARRAY(m_CharInfo);
}

/*!
 * サイズ取得(返り値は次の文字)
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
void TbFontSjis::getSize( TbDimensionF32& size , const TbUint32 fontSize , TbSint32 index )
{
    if( index >= 0 ){
        TbRectF32& uvRect = m_CharInfo[index].uvRect;
        const TbDimensionF32& baseUVSize = GetBaseUVSize();
        size.width = ( uvRect.width / baseUVSize.width ) * static_cast<TbFloat32>(fontSize);
        size.height = ( uvRect.height / baseUVSize.height ) * static_cast<TbFloat32>(fontSize);
    }
}

/*!
 * UV座標取得
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
void TbFontSjis::getUV( TbRectF32& uv , TbSint32 index )
{
    if( index >= 0 ){
        uv = m_CharInfo[index].uvRect;
    }
}

/*!
 * インデックス取得(返り値は次の文字)
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
const char* TbFontSjis::getIndex( TbSint32& index , const char* ch )
{
    TbUint32 hash = 0U;
    TbUint32 byteSize = 1;
    if( TbIsDBCSLeadByte(TB_CODE_PAGE_SJIS , *ch) ){
        byteSize = 2;
    }else{
        byteSize = 1;
    }
    hash = TbCrc32::GetHash( reinterpret_cast<const TbUint8*>(ch) , byteSize );
    TbSizeT charCount = GetCharCount();
    Getter getter;
    index = TbBinarySearchIndex( TbGetAdapterIterator<TbUint32>(&m_CharInfo[0],getter) , 
                                          TbGetAdapterIterator<TbUint32>(&m_CharInfo[charCount] , getter) , 
                                          hash );
    if( index < 0 ){
        hash = TbCrc32::GetHash("?");
        index = TbBinarySearchIndex( TbGetAdapterIterator<TbUint32>(&m_CharInfo[0],getter) , 
                                              TbGetAdapterIterator<TbUint32>(&m_CharInfo[charCount] , getter) , 
                                              hash );
        if( index < 0 ){
            return  nullptr;
        }
    }
    const char* result = ch + byteSize;
    return result;
}

}