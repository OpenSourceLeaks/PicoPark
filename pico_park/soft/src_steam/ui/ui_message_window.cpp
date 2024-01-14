﻿/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_message_window.h"
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
UiMessageWindow::UiMessageWindow( Type type )
    : Super()
    , m_Type(type)
    , m_Result(RESULT_NONE)
    , m_IsEnableFit(TB_TRUE)
{
    switch( m_Type )
    {
    case TYPE_NORMAL:
        break;
    case TYPE_OK:
        m_Button[0].Enter(this);
        m_Button[0].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
        m_Button[0].SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_Button[0].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[0].SetPos(0.0f,BUTTON_OFFSET_Y);
        m_Button[0].SetText("OK");
        m_Button[0].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_YES));
        break;
    case TYPE_YESNO:
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
            m_Button[i].Enter(this);
            m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
            m_Button[i].SetPos(TbVector2(YES_NO_OFFSET_X[i],BUTTON_OFFSET_Y));
            m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
            m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        }
        m_Button[0].SetAlignX(TB_UI_ALIGN_X_RIGHT);
        m_Button[0].SetText("YES");
        m_Button[0].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_YES));
        m_Button[1].SetAlignX(TB_UI_ALIGN_X_LEFT);
        m_Button[1].SetText("YES");
        m_Button[1].SetEnableResize(TB_FALSE);
        m_Button[1].SetText("NO");
        m_Button[1].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,RESULT_NO));
        break;
    };

    m_TextBox.Enter(this);
    m_TextBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_TextBox.SetTextAlignX(TB_UI_ALIGN_X_CENTER);
    m_TextBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_TextBox.SetPos(0.0f,GetLocalTopY());

    SetClientSize(400.0f,100.0f);
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiMessageWindow::~UiMessageWindow()
{
    ClearWidget();
}
    
/*!
 * メッセージ設定
 * @author teco
 * @since 2014.12.05
 */
void UiMessageWindow::SetMessage( const char* message )
{
    m_TextBox.SetText(message);
#if 0
    TbFloat32 buttonY = m_TextBox.GetRegion().height;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].SetPosY(buttonY+40.0f);
    }
#endif
    fit();
}

/*!
 * 派生クラス用
 * @author teco
 * @since 2014.12.05
 */
void UiMessageWindow::OnUpdate( TbFloat32 deltasec  )
{
    if( !IsOpened() ) {
        return;
    }

    // フォーカス遷移
    if( TYPE_YESNO == m_Type ){
        if( SysInput::IsAnyonePress(SysInput::BUTTON_LEFT,GetPlayerBit()) || 
            SysInput::IsAnyonePress(SysInput::BUTTON_RIGHT,GetPlayerBit()) )
        {
            if( m_Button[0].IsFocused() ){
                m_Button[1].SetFocus();
            }else{
                m_Button[0].SetFocus();
            }
        }
    }
}

/*!
 * 生成時
 * @author teco
 * @since 2014.12.15
 */
void UiMessageWindow::OnCreated()
{
    m_Result = RESULT_NONE;
    m_Button[1].SetFocus();
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
void UiMessageWindow::fit()
{
    if( !m_IsEnableFit ){
        return;
    }
    m_TextBox.SetPos(0.0f,GetLocalTopY());

    TbFloat32 width  = m_TextBox.GetWidth();
    TbFloat32 height = m_TextBox.GetGlobalRightBottomPos().GetY() - GetClientRegion().y;
    if( m_Type != TYPE_NORMAL ){
        height += m_Button[0].GetHeight() + BUTTON_OFFSET_Y + 24.0f;
    }else{
        height += 24.0f;
    }
    switch( m_Type )
    {
    case TYPE_OK:
        width = TbMax(width,m_Button[0].GetRegion().width);
        break;
    case TYPE_YESNO:
        {
            const TbRectF32& rightRegion = m_Button[1].GetGlobalRegion();
            TbFloat32 buttonWidth = rightRegion.x + rightRegion.width - m_Button[0].GetGlobalPos().GetX();
            width = TbMax(width,buttonWidth);
        }
        break;
    }
    SetClientSize(width,height);
}