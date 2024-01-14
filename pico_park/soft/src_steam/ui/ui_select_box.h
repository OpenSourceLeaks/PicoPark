/*!
 * 選択ボックス
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_UI_SELECT_BOX_H_
#define _INCLUDED_UI_SELECT_BOX_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <base/container/tb_array.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/sprite/tb_sprite_material.h>

#include <ui/tb_ui_widget.h>
#include <ui/tb_ui_text_box.h>
#include "ui/ui_arrow_button.h"

class TestTextBox : public toybox::TbUiTextBox
{
public:
    // コンストラクタ
    TestTextBox( TbBool hasFrame = TB_FALSE )
        : TbUiTextBox(hasFrame)
    {}
public:
    // フォーカス状態変更
    virtual void SetFocusFrame( TbBool isEnable ) TB_OVERRIDE;
};

class UiSelectBox : public toybox::TbUiWidget
{
public:

    typedef toybox::TbUiWidget Super;
    typedef UiSelectBox Self;
    typedef TbStaticFunction32<void (TbSint32 index)> ChangedCallback;
    typedef TbStaticFunction32<void (TbSint32 index)> DecidedCallback;

public:
    
    // コンストラクタ
    UiSelectBox();
    
    // デストラクタ
    virtual ~UiSelectBox();

public:

    // アイテム追加
    void AddItem( const char* name );

    // アイテム数取得
    TbSint32 GetItemCount() const {
        return m_Items.GetCount();
    }

    // 選択している番号取得
    TbSint32 GetSelectedIndex() const {
        return m_Index;
    }

    // 選択文字列取得
    const char* GetSelectedString() const {
        if( 0 <= m_Index && m_Index < GetItemCount() ){
            return m_Items[m_Index].name.GetBuffer();
        }
        return  nullptr;
    }

    // 選択している番号設定
    void SetSelectedIndex( TbSint32 index ) {
        if( 0 <= index && index < GetItemCount() ){
            m_Index = index;
            m_TextBox.SetText(GetSelectedString());
        }
    }

    // フォーカスフレーム利用するか
    void SetEnableFocusFrame() {
        m_TextBox.SetEnableFrame();
    }

    // ループ可能か
    void SetEnableLoop( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_LOOP,isEnable);
    }

    // ループ可能
    TbBool IsEnableLoop() const {
        return m_BitArray.Test(FLAG_ENABLE_LOOP);
    }

    // アイテムの最大文字数の文字設定
    void SetItemMaxSizeName( const char* referenceStr );

public:

    // 選択項目変更時コールバック設定
    void SetChangedCallback( const ChangedCallback& cb ) {
        m_ChangedCallback = cb;
    }

    // 決定時コールバック設定
    void SetDecidedCallback( const DecidedCallback& cb ) {
        m_DecidedCallback = cb;
    }

public:

    // 所属になる
    virtual void OnEnter( TbUiWidget* widget ) TB_OVERRIDE;

    // 所属になる
    virtual void OnExit() TB_OVERRIDE;

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec ) TB_OVERRIDE;

    // 生成時コールバック
    virtual void OnCreated() TB_OVERRIDE;

    // 破棄時コールバック
    virtual void OnDeleted() TB_OVERRIDE;

    // フォーカス状態変更
    virtual void OnFocusChanged( TbBool isFocus , TbUiWidget* widget ) TB_OVERRIDE
    {
        if( !isFocus ) {
            m_TextBox.SetFocus();
        }
        m_TextBox.SetFocusFrame(isFocus);
    }

private:

    struct Item
    {
        TbStaticString128   name;
        TbDimensionF32      size;
    };

    enum ArrowIndex
    {
        ARROW_LEFT_NAVI , 
        ARROW_RIGHT_NAVI , 
        ARROW_MAX
    };

    enum Flag
    {
        FLAG_ENABLE_LOOP ,
        FLAG_FIX_MAX_SIZE ,
        FLAG_MAX
    };

private:

    void resize();

    // インデックス加算
    void addIndex( TbBool up );

    // 矢印を押した
    void onPressArrow( TbBool isRight );

private:

    TbBitArray32        m_BitArray;
    TbSint32            m_Index;
    TbFloat32           m_ChangingTimer;
    TbArray<Item>       m_Items;
    TbDimensionF32      m_ItemMaxSize;
    TbRectF32           m_TextRect;
    TestTextBox         m_TextBox;
    UiArrowButton       m_Arrow[ARROW_MAX];
    ChangedCallback     m_ChangedCallback;
    DecidedCallback     m_DecidedCallback;

};

#endif
