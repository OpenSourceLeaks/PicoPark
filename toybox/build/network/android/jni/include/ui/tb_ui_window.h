/*!
 * UIウィンドウ
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_WINDOW_H_
#define _INCLUDED_TB_UI_WINDOW_H_

#include <graphics/sprite/tb_elastic_sprite.h>
#include <base/string/tb_static_string.h>
#include <base/container/tb_static_array.h>
#include "ui/tb_ui_types.h"
#include "ui/Tb_ui_widget.h"

namespace toybox
{

class TbUiWindow
{
    friend class TbUiWidget;

public:
    
    // コンストラクタ
    TbUiWindow( TbBool hasTitle );

    // デストラクタ
    virtual ~TbUiWindow();

public:

    // 開く
    void Open();

    // 閉じる
    void Close();

    // 閉じるのを予約
    void ReserverClose( TbFloat32 sec ) {
        m_CloseSec = sec;
    }
    
    // 開かれている
    TbBool IsOpened() const {
        return m_BitArray.Test(FLAG_OPEN);
    }

public:

    // 位置取得
    const TbVector2& GetPos() const {
        return m_Sprite.GetTranslate();
    }

    // サイズ取得
    const TbDimensionF32& GetSize() const {
        return m_Sprite.GetSize();
    }

    // 矩形情報取得
    TbRectF32 GetRegion() const {
        TbRectF32 rect;
        const TbVector2& pos = GetPos();
        const TbDimensionF32& size = GetSize();
        rect.x = pos.GetX();
        rect.y = pos.GetY();
        rect.width = size.width;
        rect.height = size.height;
        return rect;
    }

    // クライント領域取得
    TbRectF32   GetClientRegion() const;

    // ウィジェットのローカル領域
    TbRectF32   GetWidgetRegion() const;

    // タイトルバーを持っている
    TbBool HasTitleBar() const {
        return m_BitArray.Test(FLAG_HAS_TITLE_BAR);
    }

    // 自動リサイズ機能が有効か
    TbBool IsAutoResizable() const {
        return m_BitArray.Test(FLAG_AUTO_RESIZABLE);
    }

    // 所有者設定
    void SetPlayerBit( TbUint32 bit );

    // 所有者取得
    TbUint32 GetPlayerBit() const {
        return m_PlayerBit;
    }

    // フォーカス設定
    void SetFocus( TbBool isFocus );

    // フォーカスか
    TbBool IsFocus() const {
        return m_BitArray.Test(FLAG_FOCUS);
    }

    // マルチフォーカス設定
    void SetEnableMultiFocus( TbBool isEnable ) {
        m_MainWidget.SetEnableMultiFocus(isEnable);
    }

    // マルチフォーカス有効か
    TbBool IsEnableMultiFocus() const {
        return m_MainWidget.IsEnableMultiFocus();
    }

    // 閉じるボタン有効か
    void SetEnableCloseButton( TbBool isEnable ) {
        m_CloseSprite.SetVisible(isEnable);
    }

    // 閉じるボタン有効か
    TbBool IsEnableCloseButton() {
        return m_CloseSprite.IsVisible();
    }

public:

    // 位置設定
    void    SetPos( const TbVector2& pos ) {
        m_Pos = pos;
        updatePos();
    }

    // 位置設定
    void SetPos( TbFloat32 x , TbFloat32 y ) {
        m_Pos.Set(x,y);
        updatePos();
    }

    // クライアント座標設定
    void SetClientPos( const TbVector2& pos );

    // サイズ設定
    void SetSize( const TbDimensionF32& size ) {
        SetSize(size.width,size.height);
    }

    // サイズ設定
    void SetSize( TbFloat32 width , TbFloat32 height );

    // サイズ設定
    void SetWidth( TbFloat32 width );

    // サイズ設定
    void SetHeight( TbFloat32 height );

    // クライアントサイズ設定
    void SetClientSize( TbFloat32 width , TbFloat32 height );

    // クライアントサイズ設定
    void SetClientWidth( TbFloat32 width );

    // クライアントサイズ設定
    void SetClientHeight( TbFloat32 height );

    // 自動リサイズ機能が有効か
    void SetAutoResizable( TbBool resizable ){
        m_BitArray.SetBit(FLAG_AUTO_RESIZABLE,resizable);
    }

    // アライン設定
    void SetAlign( TbUiAlignX x , TbUiAlignY y ) {
        m_Align.x = x; 
        m_Align.y = y;
        updatePos();
    }

    // X軸アライン設定
    void SetAlignX( TbUiAlignX align ) {
        m_Align.x = align;
        updatePos();
    }

    // Y軸アライン設定
    void SetAlignY( TbUiAlignY align ) {
        m_Align.y = align;
        updatePos();
    }

    // デフォルトUV設定
    void SetDefaultUV( const TbElasticSprite::UvParam& uv ) {
        m_DefaultUv = uv;
        if( !IsFullscreen() ) {
            m_Sprite.SetUV(uv);
        }
    }

    // 入力を有効
    void SetEnableInput( TbBool isEnable ){
        m_BitArray.SetBit(FLAG_ENABLE_INPUT,isEnable);
    }

    // 入力を有効
    TbBool IsEnableInput() const {
        return m_BitArray.Test(FLAG_ENABLE_INPUT);
    }

    // マテリアル設定
    void SetWindowMaterial(TbSpriteMaterial* material) {
        m_Sprite.SetMaterial( material );
    }

    // マテリアル設定
    void SetWindowColor(const TbColor& color) {
        m_Sprite.SetColor(color);
    }

    // マテリアル設定
    void SetCloseButtonMaterial(TbSpriteMaterial* material) {
        m_CloseSprite.SetMaterial(material);
    }

public:

    // タイトル設定
    void SetTitle( const char* title ) {
        m_Title = title;
    }

    // タイトル取得
    const toybox::TbStaticString32& GetTitle() {
        return m_Title;
    }

    // 可視情報設定
    void SetVisible( TbBool isVisible ) {
        m_Sprite.SetVisible(isVisible);
    }

    // 可視設定取得
    TbBool IsVisible() const {
        return m_Sprite.IsVisible();
    }

    // フルスクリーン設定
    void SetFullscreen( TbBool isFullscreen );

    // フルスクリーン取得
    TbBool IsFullscreen() const {
        return m_BitArray.Test(FLAG_FULLSCREEN);
    }

public:

    // フォーカスが当たっているウィジェット取得
    TbUiWidget* GetFocusedWidget();

    // フォーカスが当たっているウィジェット取得
    const TbUiWidget* GetFocusedWidget() const;

    // 所属しているウィジェットをクリア
    void ClearWidget() {
        m_MainWidget.ClearWidget();
    }

    // メインウィジェット
    TbUiWidget* GetMainWidget() {
        return &m_MainWidget;
    }

public:
    
    // 定期処理
    void Update( TbFloat32 deltasec );

    // 描画処理
    void Draw();

public: // 非推奨

    // スプライトの奥行きオフセット取得
    TbFloat32 GetSpriteOffsetZ() const {
        return m_SpriteOffsetZ;
    }

protected:
    
    // 派生クラス用
    virtual void OnUpdate( TbFloat32 deltasec ) {}

    // 派生クラス用
    virtual void OnDraw() {}

    // 生成開始時
    virtual void OnCreating() {}

    // 生成時
    virtual void OnCreated() {}

    // 閉じられた時に呼ばれる
    virtual void OnClosed() {}

    // プレイヤービットが変更された
    virtual void OnPlayerBitChanged( TbUint32 playerBit ) {}

    // フルスクリーンが変更された
    virtual void OnFullscreenChanged( TbBool isFullscreen ) {}

protected:

    // 優先度設定
    void SetPriority( TbFloat32 priority ) {
        m_SpriteOffsetZ = TB_UI_WINDOW_SPRITE_OFFSET * priority;
    }

private:

    enum Flag {
        FLAG_HAS_TITLE_BAR , 
        FLAG_AUTO_RESIZABLE , 
        FLAG_FULLSCREEN ,
        FLAG_OPEN ,
        FLAG_FIRST ,
        FLAG_VISIBLE , 
        FLAG_FOCUS , 
        FLAG_ENABLE_INPUT , // 入力有効か
        FLAG_MAX
    };
    
private:

    // 
    void fit();

private: // UiWidget専用

    // ウィジェット追加
    void addWidget( TbUiWidget* widget );

    // ウィジェット除去
    void removeWidget( TbUiWidget* widget );

    // 座標更新
    void updatePos();

private:

    TbBitArray32                m_BitArray;         // ビット配列
    TbStaticString32            m_Title;            // タイトル文字
    TbElasticSprite             m_Sprite;           // スプライト
    TbSprite                    m_CloseSprite;      // 閉じるボタンスプライト
    TbElasticSprite::UvParam    m_DefaultUv;
    TbUiWidget                  m_MainWidget;       // 
    TbUint32                    m_PlayerBit;        // 所有者
    TbVector2                   m_Pos;              // 
    TbUiAlign                   m_Align;            // 
    TbFloat32                   m_SpriteOffsetZ;    // スプライトの奥行きオフセット
    TbFloat32                   m_CloseSec;

};

}

#endif