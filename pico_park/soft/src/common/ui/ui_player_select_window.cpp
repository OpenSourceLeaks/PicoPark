/*!
 * プレイヤー選択メニュー
 * @author teco
 */

#include "fwd.h"
#include "common/ui/ui_player_select_window.h"
#include <crayon/input/cr_input.h>
#include "root.h"
#include "common/cmn_util.h"
#include <base/util/tb_bind.h>
#include <input/tb_pad.h>

#define USE_OK_CANCEL (0)

static const TbFloat32 OK_CANCEL_OFFSET_X[] =
{
    -30.0f , 
    30.0f  ,
};

static const TbFloat32 SELECT_BOX_Y = -16.0f;

/*!
 * コンストラクタ
 * @author teco
 */
UiPlayerSelectWindow::UiPlayerSelectWindow()
    : Super(TB_TRUE)
    , m_MainFocus(MAIN_FOCUS_ITEM)
    , m_Result(RESULT_NONE)
{
    m_PlayerSelectBox.Enter(this);
    m_PlayerSelectBox.SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
    m_PlayerSelectBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    for (TbSint32 i = CMN_PLAYER_MIN; i <= CMN_PLAYER_COUNT; ++i)
    {
        TbStaticString32 name = TbStaticString32::Format("%dPLAYERS GAME",i);
        m_PlayerSelectBox.AddItem(name.GetBuffer());
    }
#if USE_OK_CANCEL
    m_PlayerSelectBox.SetPos(0.0f, 60.0f);
    m_PlayerSelectBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_PlayerSelectBox.SetEnableFocusFrame();
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
        m_Button[i].SetPos(TbVector2(OK_CANCEL_OFFSET_X[i],-10.0f));
        m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        m_Button[i].SetEnable(TB_TRUE);
    }
    m_Button[0].SetAlignX(TB_UI_ALIGN_X_RIGHT);
    m_Button[0].SetText("CANCEL");
    m_Button[0].SetEnableResize(TB_FALSE);
    m_Button[0].SetText("OK");
    m_Button[0].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_OK));
    m_Button[1].SetAlignX(TB_UI_ALIGN_X_LEFT);
    m_Button[1].SetText("CANCEL");
    m_Button[1].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_CANCEL));

    SetClientSize(760.0f, 180.0f);
#else
    m_PlayerSelectBox.SetPos(0.0f, SELECT_BOX_Y);
    m_PlayerSelectBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
#endif
    fit();
    SetAlign(TB_UI_ALIGN_X_CENTER, TB_UI_ALIGN_Y_CENTER);

    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
#if 1
    playerCount = CMN_PLAYER_MIN;
#endif
    m_PlayerSelectBox.SetSelectedIndex(playerCount - CMN_PLAYER_MIN);
}


/*!
 * デストラクタ
 * @author teco
 */
UiPlayerSelectWindow::~UiPlayerSelectWindow()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 */
void UiPlayerSelectWindow::OnUpdate( TbFloat32 deltasec  )
{
    Super::OnUpdate(deltasec);
    if( !IsOpened() ) {
        return;
    }
    if (!IsFocus()) {
        return;
    }

    updateFocusInfo();
    TbUiManager& manager = TbUiManager::GetInstance();

#if USE_OK_CANCEL
    // フォーカスが当たっていて、利用状態の場合はクリック処理
    if( m_MainFocus == MAIN_FOCUS_ITEM )
    {
        if( manager.IsPress(TbUiInput::BUTTON_DOWN,GetPlayerBit()) ){
            m_MainFocus = MAIN_FOCUS_BUTTON;
            m_Button[m_FocusButtonIndex].SetFocus();
        }
    }else{
        if( manager.IsPress(TbUiInput::BUTTON_LEFT,GetPlayerBit()) || 
            manager.IsPress(TbUiInput::BUTTON_RIGHT,GetPlayerBit()) )
        {
            m_FocusButtonIndex = (m_FocusButtonIndex+1)%TB_ARRAY_COUNT_OF(m_Button);
            m_Button[m_FocusButtonIndex].SetFocus();
        }else if( manager.IsPress(TbUiInput::BUTTON_UP,GetPlayerBit()) ){
            m_MainFocus = MAIN_FOCUS_ITEM;
            m_PlayerSelectBox.SetFocus();
        }
    }
#else
    if (manager.IsPress(TbUiInput::BUTTON_DECIDE, GetPlayerBit())) {
        m_Result = RESULT_OK;
        Close();
    }
#endif
}

/*!
 * 生成時
 * @author teco
 */
void UiPlayerSelectWindow::OnCreated()
{
    Super::OnCreated();
    m_MainFocus = MAIN_FOCUS_ITEM;
    m_FocusButtonIndex = BUTTON_CANCEL;
    m_PlayerSelectBox.SetFocus();
    m_Result = RESULT_NONE;
    SetFocus(TB_TRUE);
}

/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiPlayerSelectWindow::fit()
{
    TbUint32 height = GetLocalTopY();
    height += (m_PlayerSelectBox.GetHeight() - SELECT_BOX_Y);
    TbUint32 width = m_PlayerSelectBox.GetRegion().width;
    SetClientSize(width, height);
}

/*! 
 * 決定時コールバック
 * @author teco
 */
void UiPlayerSelectWindow::onDecidedCallback( Result result ) {
    m_Result = result;
    if( m_DecidedCallback.IsValid() ){
        m_DecidedCallback(result);
    }
    SetFocus(TB_FALSE);
    Close();
}


/*! 
 * フォーカス情報更新
 * @author teco
 */
void UiPlayerSelectWindow::updateFocusInfo()
{
    if( m_PlayerSelectBox.IsFocused() ){
        m_MainFocus = MAIN_FOCUS_ITEM;
        return;
    }
#if USE_OK_CANCEL
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        if( m_Button[i].IsFocused() ){
            m_FocusButtonIndex = i;
            m_MainFocus = MAIN_FOCUS_BUTTON;
            return;
        } 
    }
#endif
}
