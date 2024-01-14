/*!
 * UI用イメージボックス
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_select_box.h"
#include <framework/sub/tb_framework_simple.h>
#include <ui/tb_ui_window.h>
#include "root.h"

// UV
static const TbFloat32 SPACE_WIDTH = 8.0f;
static const TbFloat32 CLOSING_SEC = 0.3f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.20
 */
UiSelectBox::UiSelectBox()
    : m_Index(0)
    , m_ChangingTimer(0.0f)
{
    m_Items.SetEnableSetCapacityAuto(TB_TRUE);

    UiArrowButtonSkin::Type TYPE_TABLE[]
    {
        UiArrowButtonSkin::TYPE_LEFT , 
        UiArrowButtonSkin::TYPE_RIGHT , 
    };
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Initilaize(TYPE_TABLE[i]);
        m_Arrow[i].SetBasePos(TbUiWidget::BASE_POS_PARENT_GLOBAL_CENTER);
        m_Arrow[i].SetAlignY(TB_UI_ALIGN_Y_CENTER);
        m_Arrow[i].SetEnable(true);
        m_Arrow[i].SetEnablePress(TB_UI_INPUT_DEVICE_BUTTON,TB_FALSE);
        m_Arrow[i].SetPressedCallback( TbBind(&Self::onPressArrow,this,i==ARROW_RIGHT_NAVI) );
    }
    m_Arrow[ARROW_LEFT_NAVI].SetAlignX(TB_UI_ALIGN_X_RIGHT);
    m_Arrow[ARROW_RIGHT_NAVI].SetAlignX(TB_UI_ALIGN_X_LEFT);

    m_TextBox.SetBasePos(TbUiWidget::BASE_POS_PARENT_GLOBAL_CENTER);
    m_TextBox.SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
    m_TextBox.SetTextAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
    m_TextBox.SetEnableManualFocusFrame(TB_TRUE);

    SetEnableLoop(TB_TRUE);
}
    
/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.20
 */
UiSelectBox::~UiSelectBox()
{
}

/*! 
 * アイテム追加
 * @author teco
 * @since 2014.12.20
 */
void UiSelectBox::AddItem( const char* name )
{
    TB_RESQUE(name);
    TB_RESQUE(!IsCreated());
    Item item;
    item.name = name;

    if( !m_BitArray.Test(FLAG_FIX_MAX_SIZE) )
    {
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        TbRectF32 region;
        if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                          name)) )
        {
            item.size.width = region.width;
            item.size.height = region.height;
        }
        m_ItemMaxSize.width = TbMax(m_ItemMaxSize.width,item.size.width);
        m_ItemMaxSize.height = TbMax(m_ItemMaxSize.height,item.size.height);
    }
    m_Items.PushBack(item);

    resize();
    m_Index = m_Items.GetCount()-1;
}

/*!
 * アイテムの最大文字数の文字設定
 * @author teco
 */
void UiSelectBox::SetItemMaxSizeName( const char* referenceStr )
{
    TbFontBuffer2d* font = Root::GetInstance().GetFont();
    TbRectF32 region;
    if( TB_SUCCEEDED(font->GetRegion( region , 0 , 0 ,
                                      referenceStr)) )
    {
        m_ItemMaxSize.width = region.width;
        m_ItemMaxSize.height = region.height;
        resize();

        m_TextBox.SetEnableResize(TB_TRUE);
        m_TextBox.SetSizeWithText(referenceStr);
        m_TextBox.SetEnableResize(TB_FALSE);
        m_BitArray.SetBit(FLAG_FIX_MAX_SIZE);
    }
}

/*!
 * ウィンドウの所属になる
 * @author teco
 */
void UiSelectBox::OnEnter( TbUiWidget* widget )
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Enter(this);
    }
    m_TextBox.Enter(this);
}

/*!
 * ウィンドウの所属になる
 * @author teco
 * @since 2014.12.20
 */
void UiSelectBox::OnExit()
{
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
        m_Arrow[i].Exit();
    }
}

/*!
 * 定期処理
 * @author teco
 * @since 2014.12.20
 */
void UiSelectBox::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){
        return;
    }
    if( m_Items.IsEmpty() ){
        TB_ASSERT(0);
        return;
    }

    // 変更中時間
    if( m_ChangingTimer > 0.0f ) {
        m_ChangingTimer -= deltaSec;
        if( m_ChangingTimer <= 0.0f ){
            for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
                if( m_Arrow[i].IsFocused() && !m_Arrow[i].IsCursorOver() ){
                    m_TextBox.SetFocus();
                }
            }
        }
    }else{
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Arrow); ++i ){
            if( m_Arrow[i].IsFocused() && !m_Arrow[i].IsCursorOver() ){
                m_ChangingTimer = CLOSING_SEC;
            }
        }
    }

    if( !IsFocused() ){
        return;
    }

    TbUiManager& manager = TbUiManager::GetInstance();

    // フォーカスが当たっていて、利用状態の場合はクリック処理
    if( manager.IsOnRepeat(TbUiInput::BUTTON_RIGHT,GetPlayerBit()) ){
        addIndex(TB_TRUE);
        m_Arrow[ARROW_RIGHT_NAVI].SetFocus();
    }else if( manager.IsOnRepeat(TbUiInput::BUTTON_LEFT,GetPlayerBit()) ){
        addIndex(TB_FALSE);
        m_Arrow[ARROW_LEFT_NAVI].SetFocus();
    }

    // 決定
    if( manager.IsPress(TbUiInput::BUTTON_DECIDE,GetPlayerBit()) ||
        manager.IsAnyoneCursorPress(m_TextRect,TB_FALSE) )
    {
        if( m_DecidedCallback.IsValid() ){
            m_DecidedCallback(m_Index);
        }
    }
}

/*!
 * 生成時コールバック
 * @author teco
 */
void UiSelectBox::OnCreated()
{
    m_TextBox.SetText(GetSelectedString());
    resize();
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void UiSelectBox::OnDeleted()
{
    if( m_Items.IsEmpty() ){
        TB_ASSERT(0);
        return;
    }
}

/*!
 * リサイズ
 * @author teco
 */
void UiSelectBox::resize()
{
    TbFloat32 offsetX = m_ItemMaxSize.width/2.0f+16.0f;
    if( m_TextBox.IsEnableFrame() ){
        offsetX += 8.0f;
    }
    m_Arrow[ARROW_LEFT_NAVI].SetPos(-offsetX,0.0f);
    m_Arrow[ARROW_RIGHT_NAVI].SetPos(offsetX,0.0f);

    TbFloat32 width = m_Arrow[ARROW_RIGHT_NAVI].GetGlobalRightBottomPos().GetX() - 
                      m_Arrow[ARROW_LEFT_NAVI].GetGlobalPos().GetX();
    SetSize(width,m_ItemMaxSize.height);
}

/*!
 * インデックス加算
 * @author teco
 */
void UiSelectBox::addIndex( TbBool up )
{
    if( 1 == m_Items.GetCount() ) {
        return;
    }
    if( up ) {
        if( IsEnableLoop() ){
            m_Index = (m_Index + 1) % m_Items.GetCount();
        }else if( m_Index < m_Items.GetCount()-1 ){
            ++m_Index;
        }
    }else{
        if( 0 == m_Index ) {
            if( IsEnableLoop() ){
                m_Index = m_Items.GetCount()-1;
            }
        }else{
            m_Index--;
        }
    }
    m_TextBox.SetText(GetSelectedString());
    if( m_ChangedCallback.IsValid() ){
        m_ChangedCallback(m_Index);
    }
}

/*!
 * 矢印を押した
 * @author teco
 */
void UiSelectBox::onPressArrow( TbBool isRight )
{
    addIndex(isRight);
}
