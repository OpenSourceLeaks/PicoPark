/*!
 * 選択ウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_select_window.h"
#include "system/sys_input.h"
#include "root.h"

#include <base/util/tb_bind.h>

static const TbFloat32 YES_NO_OFFSET_X[] =
{
    -40.0f , 
    40.0f  ,
};

static const TbFloat32 BUTTON_OFFSET_Y  = 0.0f;

static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiSelectWindow::UiSelectWindow()
    : Super()
    , m_Result(RESULT_NONE)
{
    m_TextBox.Enter(this);
    m_TextBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_TextBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_TextBox.SetPos(0.0f,GetLocalTopY());

    m_SelectBox.Enter(this);
    m_SelectBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_SelectBox.SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
    m_SelectBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
    m_SelectBox.SetPos(TbVector2(0.0f,BUTTON_OFFSET_Y));
    m_SelectBox.SetDecidedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_OK,toybox::_1));

    SetClientSize(400.0f,100.0f);
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiSelectWindow::~UiSelectWindow()
{
    ClearWidget();
}

/*! 
 * メッセージ設定
 * @author teco
 * @since 2014.12.21
 */
void UiSelectWindow::SetMessage( const char* message )
{
     m_TextBox.SetText(message);
}

/*!
 * 選択アイテム設定
 * @author teco
 * @since 2014.12.21
 */
void UiSelectWindow::AddSelectItem( const char* name )
{
    m_SelectBox.AddItem(name);
}

/*!
 * 生成時
 * @author teco
 * @since 2014.12.15
 */
void UiSelectWindow::OnCreated()
{
    m_Result = RESULT_NONE;
    m_SelectBox.SetFocus();
    fit();
}

/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiSelectWindow::fit()
{
    TbFloat32 width  = m_TextBox.GetWidth();
    TbFloat32 height = m_TextBox.GetGlobalRightBottomPos().GetY() - GetClientRegion().y;
    height += m_SelectBox.GetHeight() + BUTTON_OFFSET_Y + 16.0f;
    width = TbMax(width,m_SelectBox.GetRegion().width);
    SetClientSize(width,height);
}
