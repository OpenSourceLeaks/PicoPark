/*!
 * テキストボックス
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_TEXT_BOX_H_
#define _INCLUDED_TB_UI_TEXT_BOX_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <ui/tb_ui_widget.h>
#include <graphics/sprite/tb_elastic_sprite.h>

namespace toybox
{

class TbUiTextBox : public TbUiWidget
{
public:
    typedef TbUiWidget Super;
public:
    
    // コンストラクタ
    TbUiTextBox( TbBool hasFrame = TB_FALSE );
    
    // デストラクタ
    virtual ~TbUiTextBox();

public:

    // 生成時コールバック
    virtual void OnCreated() TB_OVERRIDE;

    // 破棄時コールバック
    virtual void OnDeleted() TB_OVERRIDE;

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec ) TB_OVERRIDE;

    // 描画処理
    virtual void OnDraw() TB_OVERRIDE;

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget ) TB_OVERRIDE;

public:

    // 文字設定
    void SetText( const char* text );

    // 文字フォントサイズ
    void SetFontSize(TbUint32 fontSize) {
        m_FontSize = fontSize;
    }

    // 文字フォントサイズ取得
    TbUint32 GetFontSize() const {
        return m_FontSize;
    }

    // サイズを参照テキストを使って設定
    void SetSizeWithText( const char* text );

    // 文字取得
    const char* GetText() const {
        return m_Text.GetBuffer();
    }

    // 文字色設定
    void SetTextColor( const TbColor& color ) {
        m_TextColor = color;
    }

    // 文字色取得
    const TbColor& GetTextColor() const {
        return m_TextColor;
    }

    // 文字サイズ取得
    const TbDimensionF32& GetTextSize() const {
        return m_TextSize;
    }

    // フレーム有効
    void SetEnableFrame();

    // フレーム有効か
    TbBool IsEnableFrame() const {
        return m_FrameSprite.IsInitialized();
    }

    // フレームのフォーカス表示の手動
    void SetEnableManualFocusFrame( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_MANUAL_FOCUS_FRAME,isEnable);
    }

    // フレームのフォーカス表示の手動
    TbBool IsEnableManualFocusFrame() const {
        return m_BitArray.Test(FLAG_MANUAL_FOCUS_FRAME);
    }

    // フレームのフォーカス表示の手動
    virtual void SetFocusFrame( TbBool isEnable );

    // フレームのリサイズ
    void SetEnableResize( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_RESIZE,isEnable);
    }

    // フレームのリサイズ
    TbBool IsEnableResize() const {
        return m_BitArray.Test(FLAG_ENABLE_RESIZE);
    }

    // テキストアライン設定
    void SetTextAlignX( TbUiAlignX align ) {
        m_TextAlignX = align;
    }

    // テキストアライン設定
    void SetTextAlignY( TbUiAlignY align ) {
        m_TextAlignY = align;
    }

    // テキストアライン設定
    void SetTextAlign( TbUiAlignX alignX , TbUiAlignY alignY ) {
        m_TextAlignX = alignX;
        m_TextAlignY = alignY;
    }

public:

    enum Flag
    {
        FLAG_ENABLE , 
        FLAG_ENABLE_RESIZE ,
        FLAG_VISIBLE_FRAME ,
        FLAG_MANUAL_FOCUS_FRAME ,
        FLAG_MAX
    };

protected:

    TbBitArray32        m_BitArray;
    TbUint32            m_FontSize;
    TbStaticString512   m_Text;    // タイトル文字
    TbDimensionF32      m_TextSize;
    TbColor             m_TextColor;

    TbElasticSprite     m_FrameSprite;        // スプライト
    TbUiAlignX          m_TextAlignX;
    TbUiAlignY          m_TextAlignY;

};

}

#endif
