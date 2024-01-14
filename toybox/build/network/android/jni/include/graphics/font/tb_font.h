/*!
 * @file
 * @brief フォントクラス
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_FONT_H_
#define _INCLUDED_TB_FONT_H_

#include <graphics/font/tb_font_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

class TbTexture;
class TbFile;
struct TbFontCommandHeader;
struct TbDimensionS32;

class TbFont
{
public:

    // コンストラクタ
    TbFont( TbFontCharCode charCode , const TbFontCreateParam& param )
        : m_DefaultFontSize( param.defaultFontSize )
        , m_BaseUVSize(param.uvBaseSize)
        , m_Texture(nullptr)
        , m_CharCode(charCode)
        , m_CharCount(param.charCount)
    {
    }

    // デストラクタ
    virtual ~TbFont()
    {
    }

public:

    // 改行コード検出
    const char* GetNewLine( TbBool& isNewLine , const char* ch );

    // サイズ取得(返り値は次の文字)
    virtual const char* GetSize( TbDimensionF32& size , const TbUint32 fontSize , const char* ch ) = 0;

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , const char* ch ) = 0;

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , TbDimensionF32& size , const TbUint32 fontSize , const char* ch ) = 0;

public:

    // デフォルトのフォントサイズ取得
    TbUint32 GetDefaultFontSize() const {
        return m_DefaultFontSize;
    }

    // 基準となるUVベースサイズ
    const TbDimensionF32& GetBaseUVSize() const {
        return m_BaseUVSize;
    }
    
    // 文字コード取得
    TbFontCharCode GetCharCode() const {
        return m_CharCode;
    }

    // 対応文字数取得
    TbUint32 GetCharCount() const {
        return m_CharCount;
    }
    
    // 対応テクスチャ設定
    void SetTexture( TbTexture* texture ){
        m_Texture = texture;
    }
    
    // 対応テクスチャ取得
    TbTexture* GetTexture() const {
        return m_Texture;
    }
    
private:

    TbUint32        m_DefaultFontSize;  //!< デフォルトのフォントサイズ
    TbDimensionF32  m_BaseUVSize;       //!< フォントサイズの基準となるUVサイズ
    TbTexture*      m_Texture;          //!< テクスチャ
    TbFontCharCode  m_CharCode;         //!< 文字コード
    TbUint32        m_CharCount;        //!< 対応文字数

};

// ASCIIフォント
class TbFontAscii : public TbFont
{
public:
    typedef TbFont Super;
public:
    
    // ASCIIフォントコンスデフォルトトラクタ
    TbFontAscii();

    // ASCIIフォントコンストラクタ
    TbFontAscii( const TbFontCreateParamAscii& param );
    
    // デストラクタ
    virtual ~TbFontAscii();
    
public:

    // サイズ取得(返り値は次の文字)
    virtual const char* GetSize( TbDimensionF32& size , const TbUint32 fontSize , const char* ch );

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , const char* ch );

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , TbDimensionF32& size , const TbUint32 fontSize , const char* ch );

private:

    struct CharInfo
    {
        TbRectF32 uvRect;
    };
    
private:

    TbBitArray32    m_BitArray;   //!< フラグ
    CharInfo*       m_CharInfo;  //!< ASCII文字用情報格納場所(内部用)
    
};
    
// SJISフォント
class TbFontSjis : public TbFont
{
public:
    typedef TbFont Super;
public:
    
    // SJISフォントコンストラクタ
    TbFontSjis( const TbFontCreateParamSjis& param );
    
    // デストラクタ
    virtual ~TbFontSjis();
    
public:
    
    // サイズ取得(返り値は次の文字)
    virtual const char* GetSize( TbDimensionF32& size , const TbUint32 fontSize , const char* ch )
    {
        TbSint32 index = 0;
        const char* result = getIndex(index,ch);
        getSize( size , fontSize , index );
        return result;
    }

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , const char* ch ) 
    {
        TbSint32 index = 0;
        const char* result = getIndex(index,ch);
        getUV( uv , index );
        return result;
    }

    // UV座標取得(返り値は次の文字)
    virtual const char* GetUV( TbRectF32& uv , TbDimensionF32& size , const TbUint32 fontSize , const char* ch )
    {
        TbSint32 index = 0;
        const char* result = getIndex(index,ch);
        getUV( uv , index );
        getSize( size , fontSize , index );
        return result;
    }

private:

    struct CharInfo
    {
        TbUint32 hash;
        TbRectF32 uvRect;
    };

    struct Getter
    {
        TbUint32 operator()(const CharInfo& value)
        {
            return value.hash;
        }
    };

    struct Compare
    {
        TbBool operator()( const CharInfo& left , const CharInfo& right ){
            return left.hash < right.hash;
        }
    };

private:

    // サイズ取得(返り値は次の文字)
    void getSize( TbDimensionF32& size , const TbUint32 fontSize , TbSint32 index );

    // UV座標取得(返り値は次の文字)
    void getUV( TbRectF32& uv , TbSint32 index );

    // UV座標取得(返り値は次の文字)
    const char* getIndex( TbSint32& index ,  const char* ch );

private:
    
    CharInfo*   m_CharInfo; // 文字別情報

};
    

}

#endif
