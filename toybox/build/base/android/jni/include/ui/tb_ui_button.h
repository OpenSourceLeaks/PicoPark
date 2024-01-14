/*!
 * UIボタン
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_BUTTON_H_
#define _INCLUDED_TB_UI_BUTTON_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>

#include <ui/tb_ui_widget.h>
#include <ui/tb_ui_button_skin.h>
#include <ui/tb_ui_input.h>

namespace toybox
{

class TbUiButton : public TbUiWidget
{
public:

    typedef TbStaticFunction32<void ()>         PressedCallback;
    typedef TbStaticFunction32<void ()>         ReleasedCallback;
    typedef TbUiWidget Super;

public:
    
    // コンストラクタ
    TbUiButton();
    
    // デストラクタ
    virtual ~TbUiButton();

public:

    // スキン設定
    void SetSkin( TbUiButtonSkin* skin );

    // 押されている状態
    TbBool IsOn() const {
        return PHASE_PRESS_CURSOR == m_Phase ||
               PHASE_PRESS_KEY == m_Phase;
    }

public:

    // 文字設定
    void SetText( const char* text );

    // フォントサイズ設定
    void SetFontSize(TbUint32 fontSize) {
        m_FontSize = fontSize;
    }

    // サイズを参照テキストを使って設定
    void SetSizeWithText( const char* text );

    // 文字取得
    const TbStaticString32& GetText() const {
        return m_Text;
    }

    // 文字色設定
    void SetTextColor( const TbColor& color ) {
        m_TextColor = color;
    }

    // 文字色取得
    const TbColor& GetTextColor() const {
        return m_TextColor;
    }

    void SetMargin( TbFloat32 l , TbFloat32 r , TbFloat32 t , TbFloat32 b ) {
        m_Margin.left = l;
        m_Margin.right = r;
        m_Margin.top = t;
        m_Margin.bottom = b;
    }

    // 有効
    void SetEnable( TbBool isEnable );

    // 有効かどうか
    TbBool IsEnable() const {
        return m_BitArray.Test(FLAG_ENABLE);
    }

    // リサイズ有効
    void SetEnableResize( TbBool isEnable ){
        if( IsEnableResize() && !isEnable ) {
            TbRectF32 globalRegion = GetGlobalRegion();
            m_Skin->OnSizeChanged(globalRegion.width,globalRegion.height);
        }
        m_BitArray.SetBit(FLAG_ENABLE_RESIZE,isEnable);
    }

    // リサイズ有効か
    TbBool IsEnableResize() const {
        return m_BitArray.Test(FLAG_ENABLE_RESIZE);
    }

    // カーソルオーバーでフォーカス
    void SetEnableFocusCursorOver( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_FOCUS_CURSOR_OVER,isEnable);
    }

    // カーソルオーバーでフォーカス
    TbBool IsEnableFocusCursorOver() const {
        return m_BitArray.Test(FLAG_ENABLE_FOCUS_CURSOR_OVER);
    }

    // カーソルオーバーで押した
    void SetEnablePressCursorOver( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_PRESS_CURSOR_OVER,isEnable);
    }

    // カーソルオーバーで押した
    TbBool IsEnablePressCursorOver() const {
        return m_BitArray.Test(FLAG_ENABLE_PRESS_CURSOR_OVER);
    }

    // 任意デバイスで押すことが可能か
    void SetEnablePress( TbUiInputDevice device , TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_PRESS_DEVICE_BEGIN+device,isEnable);
    }

    // 任意デバイスで押すことが可能か
    TbBool IsEnablePress( TbUiInputDevice device ) const {
        return m_BitArray.Test(FLAG_ENABLE_PRESS_DEVICE_BEGIN+device);
    }

    // カーソルオーバー
    TbBool IsCursorOver() const;

    // テキストのアライン設定
    void SetTextAlign( TbUiAlignX align ) {
        m_TextAlignX = align;
    }

    // 押した時コールバック
    void SetPressedCallback( const PressedCallback& cb ) {
        m_PressedCallback = cb;
    }

    // 話した時コールバック
    void SetReleasedCallback( const ReleasedCallback& cb ) {
        m_ReleasedCallback = cb;
    }

    // 入力ボタン設定
    void SetPressButton(TbUiInput::Button button)
    {
        m_PressButton = button;
    }

    // 押したプレイヤー番号
    TbSint32 GetPressPlayerIndex() const {
        return m_PressPlayerIndex;
    }

protected:

    // 生成時コールバック
    virtual void OnCreated() TB_OVERRIDE;

    // 破棄時コールバック
    virtual void OnDeleted() TB_OVERRIDE;

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec ) TB_OVERRIDE;

    // 定期後処理
    virtual void OnPost( TbFloat32 deltaSec ) TB_OVERRIDE;

    // 描画処理
    virtual void OnDraw() TB_OVERRIDE;

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus , TbUiWidget* focusedWidget ) TB_OVERRIDE;

    // プレイヤービットが変更された
    virtual void OnPlayerBitChanged( TbUint32 playerBit ) TB_OVERRIDE {
        if( m_Skin ){
            m_Skin->OnPlayerBitChanged(playerBit);
        }
    }

    // 押された状態変更
    virtual void OnPressChanged( TbBool isPress ) ;

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible ) TB_OVERRIDE{
        m_Skin->OnVisibleChanged(isVisible);
    }

private:

    enum Flag
    {
        FLAG_ENABLE , 
        FLAG_ENABLE_RESIZE , 
        FLAG_ENABLE_FOCUS_CURSOR_OVER , // カーソルオーバーでフォーカス
        FLAG_ENABLE_PRESS_CURSOR_OVER , // カーソルオーバーで押した扱い
        FLAG_CURSOR_OVER ,
        FLAG_ENABLE_PRESS_DEVICE_BEGIN , 
        FLAG_ENABLE_PRESS_DEVICE_END = FLAG_ENABLE_PRESS_DEVICE_BEGIN + TB_UI_INPUT_DEVICE_MAX ,
        FLAG_PRESS , 
        FLAG_MAX
    };

    enum Phase
    {
        PHASE_IDLE ,
        PHASE_ACTIVE ,
        PHASE_PRESS_CURSOR ,
        PHASE_PRESS_KEY ,
    };

private:

    // カーソルが上にあるか
    TbBool isCursorOver( TbUint32 index , TbBool isEnableCursorRadius ) const;

private:

    TbBitArray32        m_BitArray;
    TbSint32            m_Phase;
    TbUint32            m_FontSize;
    TbFloat32           m_InputDelaytimer;
    TbStaticString32    m_Text;             // 文字
    TbDimensionF32      m_TextSize;         // テキストサイズ
    TbUiAlignX          m_TextAlignX;
    TbColor             m_TextColor;        // 文字色
    PressedCallback     m_PressedCallback;
    ReleasedCallback    m_ReleasedCallback;
    TbMarginF32         m_Margin;
    TbUiInput::Button   m_PressButton;      // ボタン
    TbSint32            m_PressPlayerIndex;

private:

    TbUiButtonSkin*         m_Skin;
    TbUiButtonDefaultSkin   m_DefaultSkin;

};

}

#endif
