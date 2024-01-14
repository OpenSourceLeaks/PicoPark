/*!
 * @file
 * @brief フォントクラス
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_FONT_TYPES_H_
#define _INCLUDED_TB_FONT_TYPES_H_

#include "base/math/tb_geometry.h"

namespace toybox
{

    // 対応文字コード
enum TbFontCharCode
{
    TB_FONT_CHAR_CODE_INVALID ,
    TB_FONT_CHAR_CODE_ASCII , // ASCIIコード
    TB_FONT_CHAR_CODE_SJIS , 
};

// フォントテクスチャ情報(
// 正規化はしないで良い
struct TbFontCharInfoSjis
{
    const char*    target;
    TbFloat32      x;
    TbFloat32      y;
    TbFloat32      width;
    TbFloat32      height;
};

// 共通フォント生成パラメータ
struct TbFontCreateParam
{
    TbUint32        defaultFontSize;    // デフォルトのフォントサイズ
    TbDimensionF32  uvBaseSize;         // UVベースとなるサイズ
    TbUint32        charCount;          // 格納している文字数
};

// ASCIIフォント生成パラメータ
struct TbFontCreateParamAscii
{
    TbFontCreateParamAscii( const TbFontCreateParam& com , const TbRectF32Pod* rects )
        : common(com) , uvRectListPod(rects)
    {}
    TbFontCreateParam   common;     // 共通部分
    union {
        const TbRectF32Pod* uvRectListPod;  // ASCIIコード順に並んでいる必要がある
        const TbRectF32*    uvRectList;     // ASCIIコード順に並んでいる必要がある
    };
};

// SJISフォント生成パラメータ
struct TbFontCreateParamSjis
{
    TbFontCreateParam           common;        // 共通部分
    const TbFontCharInfoSjis*   charInfoList;   // SJISコード順に並んでいる必要はない
};

}

#endif
