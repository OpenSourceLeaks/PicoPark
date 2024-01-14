/*!
 * @file
 * @brief TrueTypeフォント情報クラス
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_FONT_INFO_TTF_H_
#define _INCLUDED_TB_FONT_INFO_TTF_H_

#include <graphics/font/tb_font_types.h>
#include <base/math/tb_geometry.h>

#include "./depend/tb_font_info_ttf_depend.h"

namespace toybox
{

class TbWindow;
class TbTexture;

/*!
 * @class 
 */
class TbFontInfoTTF
{
public:

    typedef TbFontInfoTTF Self;

    // 現時設定しているフォントに関する情報
    struct FontInfo
    {
        TbSint32       height;           // 高さ(ascent + descent)
        TbSint32       ascent;           // 文字のベースラインよりも上の幅
        TbSint32       descent;          // 文字のベースラインよりも下の幅
        TbSint32       internalLeading;  // テキストの行間スペース範囲
        TbSint32       externalLeading;
        TbSint32       aveCharWidth;     // 小文字の平均的な文字幅
        TbSint32       maxCharWidth;     // フォント内の最も幅の広い文字の幅
        TbSint32       weight;           // フォントの太さ
        TbSint32       overhang;         // 
        TbSint32       digitizedAspectX; // アスペクトX
        TbSint32       digitizedAspectY; // アスペクトY
        TbChar16       firstChar;        // フォントに含まれる先頭文字の文字コード
        TbChar16       lastChar;         // フォントに含まれる最後文字の文字コード
        TbChar16       defaultChar;      // フォントに含まれない文字の代用字コード
        TbChar16       breakChar;        // 単語の間を表す文字の文字コード
        TbSint8        italic;           // 斜体フォントかどうか
        TbSint8        underlined;       // 下線付きフォントかどうか
        TbSint8        struckOut;        // 打ち消し線付きフォントかどうか
        TbSint8        pitchAndFamily;   // 
        TbSint8        charSet;          // 
    };

    // アウトライン情報
    struct OutlineInfo
    {
        void*       buffer;
        TbUint32    bufferSize;
        TbUint32    charSize;
        TbUint32    pitch;
        TbUint32    blackBoxX;
        TbUint32    blackBoxY;
        TbPointS32  glyphOrigin;
        TbSint16    cellIncX;
        TbSint16    cellIncY;
    };

    enum Flag
    {
        FLAG_BIT_EDGE_U = TB_BIT(0) , // 上に縁
        FLAG_BIT_EDGE_D = TB_BIT(1) , // 下に縁
        FLAG_BIT_EDGE_L = TB_BIT(2) , // 左に縁
        FLAG_BIT_EDGE_R = TB_BIT(3) , // 右に縁

        FLAG_BIT_EDGE    = FLAG_BIT_EDGE_U | FLAG_BIT_EDGE_D | FLAG_BIT_EDGE_L | FLAG_BIT_EDGE_R , // 全部に縁
        FLAG_BIT_EDGE_RD = FLAG_BIT_EDGE_D | FLAG_BIT_EDGE_R   , // 右と下に縁
    };

public:

    // コンストラクタ
    TbFontInfoTTF( TbUint32 size , const TbChar8* name , TbUint32 flagBit = 0 );

    // デストラクタ
    ~TbFontInfoTTF();

public:

    // 初期化
    TbResult Init( TbUint32 size , const TbChar8* name , TbUint32 flagBit = 0 );

    // 現在設定しているフォント情報取得
    const Self::FontInfo& GetInfo() const { 
        return m_Info; 
    }

public: // マルチバイト用

    // 文字アウトライン情報取得なバッファ必要
    TbUint32 GetCharOutlineSize( const TbChar8* string  ) const;

    // 文字アウトライン情報
    TbResult GetCharOutline( TB_INOUT Self::OutlineInfo& outlineInfo , const TbChar8* string );

    // テクスチャに書き込み
    TbResult WriteTexture( TbTexture* texture , const TbChar8* string );

private:

    enum {
        FLAG_READY ,
    };

private:

    TbBitArray32        m_BitArray;    //!< ビット配列
    TbUint32            m_FlagBit;
    FontInfo            m_Info;        //!< 情報
    TbFontInfoTTFDepend m_Depend;      //!< 依存

};

}

#endif
