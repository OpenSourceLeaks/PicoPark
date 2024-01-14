/*!
 * UI矢印ボタン
 * @author teco
 */

#ifndef _INCLUDED_UI_ARROW_BUTTON_H_
#define _INCLUDED_UI_ARROW_BUTTON_H_

#include <graphics/sprite/tb_elastic_sprite.h>
#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>

#include <ui/tb_ui_button.h>

class UiArrowButtonSkin : public TbUiButtonSkin
{
    typedef TbUiButtonSkin Super;
public:

    enum Type
    {
        TYPE_INVALID ,

        TYPE_BEGIN ,
        TYPE_LEFT = TYPE_BEGIN ,
        TYPE_RIGHT ,
        TYPE_END ,
        TYPE_MAX = TYPE_END - TYPE_BEGIN ,
    };

public:
    
    // コンストラクタ
    UiArrowButtonSkin();
    
    // デストラクタ
    virtual ~UiArrowButtonSkin();

public:

    // 初期化
    void Initilaize( Type type );

    // UV設定
    void SetUV( const TbRectF32& uv ) {
        m_Sprite.SetUV(uv);
    }

    // 共通カラー利用
    void SetEnableCommonColor( TbBool isCommonColor )
    {
        m_IsCommonColor = isCommonColor;
    }

public:

    // 表示領域取得
    virtual TbRectF32    GetRegion() const;

    // 生成時コールバック
    virtual void OnCreated() TB_OVERRIDE;

    // 破棄時コールバック
    virtual void OnDeleted() TB_OVERRIDE;

    // 位置変更
    virtual void OnPosChanged( const TbVector2& pos )  TB_OVERRIDE{
        m_Sprite.SetTranslate(pos);
    }

    // リサイズ
    virtual void OnSizeChanged( TbFloat32 width , TbFloat32 height )  TB_OVERRIDE{
        m_Sprite.SetScale(width,height);
    }

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus ) TB_OVERRIDE;

    // 押された状態変更
    virtual void OnPressChanged( TbBool isPress ) TB_OVERRIDE;

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible )  TB_OVERRIDE{
        m_Sprite.SetVisible(isVisible);
    }

private:

    Type         m_Type;
    TbSprite     m_Sprite;           // スプライト
    TbBool       m_IsCommonColor;    // 共通カラー(オレンジ)

};

class UiArrowButton : public TbUiButton
{
public:

    // コンストラクタ
    UiArrowButton()
    {
    }

public:

    // 初期化
    void Initilaize( UiArrowButtonSkin::Type type ) {
        m_Skin.Initilaize(type);
        SetSkin(&m_Skin);
        SetEnableFocusCursorOver(TB_TRUE);
        SetPressButton(type==UiArrowButtonSkin::TYPE_LEFT ? TbUiInput::BUTTON_LEFT : TbUiInput::BUTTON_RIGHT);
    }

    // 共通カラー利用
    void SetEnableCommonColor(TbBool isEnable)
    {
        m_Skin.SetEnableCommonColor(isEnable);
    }

private:

    UiArrowButtonSkin    m_Skin;

};

#endif
