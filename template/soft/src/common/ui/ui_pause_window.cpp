﻿/*!
 * ポーズメニュー
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "common/ui/ui_pause_window.h"
#include "root.h"
#include <base/util/tb_bind.h>

static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};

static const char* MENU_TEXT[] = 
{
    "RETURN GAME" ,
//    "STAGE SELECT" ,
    "TITLE" ,
    "RETRY" ,
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MENU_TEXT)==UiPauseWindow::MENU_MAX);

static const TbFloat32 BUTTON_Y = 32.0f;
static const TbFloat32 BUTTON_INTERVAL_Y = 36.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiPauseWindow::UiPauseWindow()
    : Super()
    , m_FocusIndex(0)
    , m_Result(MENU_INVALID)
{
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_Button[i].SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        m_Button[i].SetText(MENU_TEXT[i]);
        m_Button[i].SetPos(TbVector2(0.0f,BUTTON_Y+i*BUTTON_INTERVAL_Y));
        Menu result = static_cast<Menu>(i+MENU_BEGIN);
        m_Button[i].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,result));
    }

    SetVisiblePlayerImage(TB_FALSE);
    SetClientSize(400.0f,100.0f);
    fit();
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiPauseWindow::~UiPauseWindow()
{
    ClearWidget();
}

/*!
 * 派生クラス用
 * @author teco
 * @since 2014.12.05
 */
void UiPauseWindow::OnUpdate( TbFloat32 deltasec  )
{
    if( !IsOpened() ) {
        return;
    }

    // フォーカス遷移
    if( CrInput::IsAnyonePress(CrInput::BUTTON_UP,GetPlayerBit()) ){
        if( 0 == m_FocusIndex ){
            m_FocusIndex = TB_ARRAY_COUNT_OF(m_Button)-1;
        }else{
            m_FocusIndex = (m_FocusIndex-1);
        }
        m_Button[m_FocusIndex].SetFocus();
    }else if( CrInput::IsAnyonePress(CrInput::BUTTON_DOWN,GetPlayerBit()) ){
        m_FocusIndex = (m_FocusIndex+1) % TB_ARRAY_COUNT_OF(m_Button);
        m_Button[m_FocusIndex].SetFocus();
    }

    Super::OnUpdate(deltasec);
}

/*!
 * 生成開始
 * @author teco
 */
void UiPauseWindow::OnCreating()
{
    TbUint32 playerBit = GetPlayerBit();
    if( TB_VERIFY(1==TbBitPopulation(playerBit)) )
    {
        fit();
    }
}

/*!
 * 生成時
 * @author teco
 * @since 2014.12.15
 */
void UiPauseWindow::OnCreated()
{
    m_Result = MENU_INVALID;
    m_FocusIndex = 0;
    m_Button[m_FocusIndex].SetFocus();
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        if( m_Button[i].IsCreated() ){
            m_Button[i].SetEnable(TB_TRUE);
        }
    }
}

/*! 
 * 自動リサイズ
 * @author teco
 * @since 2014.12.18
 */
void UiPauseWindow::fit()
{
    TbFloat32 width  = 0.0f;
    TbFloat32 height = 0.0f;
    for( TbSint32 i = TB_ARRAY_COUNT_OF(m_Button)-1; 0 <= i ; --i ) {
        if( m_Button[i].GetWindow() ) {
            height = m_Button[i].GetGlobalRightBottomPos().GetY();
            break;
        }
    }
    height -= GetClientRegion().y;
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        width = TbMax(width,m_Button[i].GetWidth());
    }
    SetClientSize(width,height);
}
