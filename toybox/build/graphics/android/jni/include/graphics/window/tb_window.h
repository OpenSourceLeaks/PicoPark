/*!
 * @file
 * @brief ウィンドウクラス
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_WINDOW_H_
#define _INCLUDED_TB_WINDOW_H_

#include <base/math/tb_geometry.h>
#include <graphics/window/depend/tb_window_depend.h>

namespace toybox
{

// ウィンドウ生成パラメータ
struct TbWindowParameter
{
    //! コンストラクタ
    TbWindowParameter()
        : isVisible(TB_FALSE)
        , name( "" )
    {
        TbMemClear( &rect , sizeof(rect) );
    };

    //! デストラクタ
    ~TbWindowParameter(){}

    TbRectS32 rect;    //!< 矩形情報
    const char* name;       //!< 名前
    TbBool isVisible;       //!< 表示するかどうか
};

/*!
 * @class 
 */
class TbWindow
{
    friend class TbWindowManager;
    friend class TbWindowDepend;
private:

    // コンストラクタ
    TbWindow( const TbWindowParameter& parameter );

    // デストラクタ
    ~TbWindow();

public:

    //! Id取得
    TbWindowId GetId() const {
        return m_Id; 
    }

    //! エラー
    TbBool IsError() const { 
        return m_BitArray.Test( FLAG_ERROR ); 
    }

    //! ウィンドウ名設定
    const void SetWindowName(const TbChar8* name){ 
        m_WindowName = name; 
    }

    //! ウィンドウ矩形取得
    const TbRectS32& GetRect() const { 
        return m_Rect; 
    }

    //! ウィンドウサイズ取得
    TbDimensionS32 GetSize() const {
        return TbDimensionS32(m_Rect.width,m_Rect.height);
    }

    //! 座標設定
    TbResult SetPos( TbSint32 x , TbSint32 y );

    //! サイズ設定
    TbResult SetSize(TbSint32 width , TbSint32 height);

    //! 矩形設定
    TbResult SetRect(TbSint32 x,TbSint32 y,TbSint32 width,TbSint32 height);

    //! ウィンドウ横幅
    const TbSint32 GetWidth() const { 
        return m_Rect.width; 
    }

    //! ウィンドウ縦幅
    const TbSint32 GetHeight() const { 
        return m_Rect.height; 
    }

    // 可視設定
    void SetVisible(TbBool isVisible);

    //! 可視設定取得
    TbBool IsVisible() const { 
        return m_BitArray.Test( FLAG_VISIBLE ); 
    }

    // 最大化
    void Maxmize();

    // 最小化
    void Minimize();

    // アクティブかどうか（フォーカスがあたっているか）
    TbBool IsFocused() const { 
        return m_BitArray.Test( FLAG_FOCUS ); 
    }

    //! 最小化している
    TbBool IsMinimized() const { 
        return m_BitArray.Test( FLAG_MINIMIZED ); 
    }

public:

    //! 依存クラス取得
    TbWindowDepend& GetDepend() { 
        return m_Depend; 
    }

    //! 依存クラス取得
    const TbWindowDepend& GetDepend() const { 
        return m_Depend; 
    }
    
private:

    enum
    {
        FLAG_ERROR ,
        FLAG_OPENED ,
        FLAG_VISIBLE ,
        FLAG_FOCUS , 
        FLAG_MINIMIZED , 
    };

private:

    // セットアップ
    TbResult setup( const TbWindowParameter& param );

    // フォーカス設定
    void setFocused( TbBool isFocused ){ 
        m_BitArray.SetBit( FLAG_FOCUS , isFocused ); 
    }

    // 最小化設定
    void setMinimized( TbBool isMinimized ) { 
        m_BitArray.SetBit( FLAG_MINIMIZED , isMinimized ); 
    }

    //! 座標設定
    void setPos( TbSint32 x , TbSint32 y ) { 
        m_Rect.x = x; 
        m_Rect.y = y; 
    }

    //! サイズ設定
    void setSize(TbSint32 width , TbSint32 height) { 
        m_Rect.width = width; 
        m_Rect.height = height; 
    }

    //! 矩形設定
    void setRect(TbSint32 x,TbSint32 y,TbSint32 width,TbSint32 height) { 
        setPos(x,y); 
        setSize(width,height); 
    }

private:

    TbBitArray32        m_BitArray;      //!< フラグ
    TbWindowId          m_Id;            //!< 識別番号
    TbStaticString128   m_WindowName;    //!< ウィンドウ名
    TbRectS32           m_Rect;          //!< 矩形情報
    TbWindowDepend      m_Depend;        //!< 依存クラス
 
};

}

#endif
