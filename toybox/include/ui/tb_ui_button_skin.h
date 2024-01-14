/*!
 * UIボタンスキン
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_BUTTON_SKIN_H_
#define _INCLUDED_TB_UI_BUTTON_SKIN_H_

#include <ui/tb_ui_types.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <base/string/tb_static_string.h>

namespace toybox
{

class TbUiButton;

class TbUiButtonSkin
{
public:

    // コンストラクタ
    TbUiButtonSkin() : m_Owner(nullptr) {}
   
    // デストラクタ
    virtual ~TbUiButtonSkin() {}

public:

    // 所有者設定
    void SetOwner( TbUiButton* owner ) {
        m_Owner = owner;
    }

    // 所有者取得
    TbUiButton* GetOwner() {
        return m_Owner;
    }

    // 所有者取得
    const TbUiButton* GetOwner() const {
        return m_Owner;
    }

public:

    // 表示領域取得
    virtual TbRectF32    GetRegion() const = 0;;

public:

    // 生成時コールバック
    virtual void OnCreated() = 0;

    // 破棄時コールバック
    virtual void OnDeleted() = 0;

    // 位置変更
    virtual void OnPosChanged( const TbVector2& pos ) = 0;

    // リサイズ
    virtual void OnSizeChanged( TbFloat32 width , TbFloat32 height  ) = 0;

    // 有効設定変更
    virtual void OnEnableChanged( TbBool isEnable ) {}

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus ) = 0;

    // プレイヤービット変更
    virtual void OnPlayerBitChanged( TbUint32 playerBit ) {}

    // 押された状態変更
    virtual void OnPressChanged( TbBool isPress ) = 0;

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible ) = 0;

private:

    TbUiButton* m_Owner;

};

class TbUiButtonDefaultSkin : public TbUiButtonSkin
{
    typedef TbUiButtonSkin Super;
public:
    
    // コンストラクタ
    TbUiButtonDefaultSkin();
    
    // デストラクタ
    virtual ~TbUiButtonDefaultSkin();

public:

    // UV設定
    void SetUV( const TbElasticSprite::UvParam& uv ) {
        m_Sprite.SetUV(uv);
    }

public:

    // 表示領域取得
    virtual TbRectF32    GetRegion() const TB_OVERRIDE;

    // 生成時コールバック
    virtual void OnCreated() TB_OVERRIDE;

    // 破棄時コールバック
    virtual void OnDeleted() TB_OVERRIDE;

    // 位置変更
    virtual void OnPosChanged( const TbVector2& pos ) TB_OVERRIDE {
        m_Sprite.SetTranslate(pos);
    }

    // リサイズ
    virtual void OnSizeChanged( TbFloat32 width , TbFloat32 height ) TB_OVERRIDE {
        m_Sprite.SetSize(width,height);
    }

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus ) TB_OVERRIDE;

    // 押された状態変更
    virtual void OnPressChanged( TbBool isPress ) TB_OVERRIDE;

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible ) TB_OVERRIDE{
        m_Sprite.SetVisible(isVisible);
    }

private:

    TbElasticSprite     m_Sprite;           // スプライト

};

}

#endif
