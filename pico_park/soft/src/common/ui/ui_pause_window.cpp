/*!
 * ポーズメニュー
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "common/ui/ui_pause_window.h"
#include <crayon/input/cr_input.h>
#include "root.h"
#include <base/util/tb_bind.h>

static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};

static const char* MENU_TEXT[] = 
{
    "RETURN GAME" ,
    "RETRY" ,
    "STAGE SELECT" ,
    "MAIN MENU" ,
    "TITLE" ,
    "JOIN"
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(MENU_TEXT)==UiPauseWindow::MENU_MAX);

static TB_NDEBUG_CONST TbFloat32 BUTTON_Y = 150.0f;
static TB_NDEBUG_CONST TbFloat32 BUTTON_INTERVAL_Y = 70.0f;

/*!
 * コンストラクタ
 * @author teco
 */
UiPauseWindow::UiPauseWindow()
    : Super()
    , m_FocusIndex(0)
    , m_Result(MENU_INVALID)
    , m_ActiveMenuCount(MENU_MAX)
{
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_MenuMap); ++i ) {
        m_MenuMap[i] = i;
    }
    for( TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i ) {
        m_Button[i].Enter(this);
        m_Button[i].SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_Button[i].SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_Button[i].SetAlignY(TB_UI_ALIGN_Y_BOTTOM);
        m_Button[i].SetTextAlign( TB_UI_ALIGN_X_CENTER );
        m_Button[i].SetFontSize(48);
        m_Button[i].SetText(MENU_TEXT[i]);
        m_Button[i].SetPos(TbVector2(0.0f,BUTTON_Y+i*BUTTON_INTERVAL_Y));
        Menu result = static_cast<Menu>(i+MENU_BEGIN);
        m_Button[i].SetPressedCallback(TbBind(&Self::onDecidedCallback,this,result));
    }

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
        while( 1 )
        {
            if( 0 == m_FocusIndex ){
                m_FocusIndex = m_ActiveMenuCount-1;
            }else{
                m_FocusIndex = (m_FocusIndex-1);
            }
            if( m_Button[m_MenuMap[m_FocusIndex]].IsEntered() ){
                m_Button[m_MenuMap[m_FocusIndex]].SetFocus();
                break;
            }
        }
    }else if( CrInput::IsAnyonePress(CrInput::BUTTON_DOWN,GetPlayerBit()) ){
        while( 1 )
        {
            m_FocusIndex = (m_FocusIndex+1) % m_ActiveMenuCount;
            if( m_Button[m_MenuMap[m_FocusIndex]].IsEntered() ){
                m_Button[m_MenuMap[m_FocusIndex]].SetFocus();
                break;
            }
        }
    }

    Super::OnUpdate(deltasec);
}

// バトルモード設定
void UiPauseWindow::SetBattleMode()
{
    TB_RESQUE(!IsOpened());
    for (TbSint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Button); ++i) {
        m_Button[i].Exit();
    }
    m_ActiveMenuCount = 3;
    m_MenuMap[0] = MENU_RETURN_GAME-MENU_BEGIN;
    m_MenuMap[1] = MENU_RETURN_MAIN_MENU - MENU_BEGIN;
    m_MenuMap[2] = MENU_RETURN_TITLE-MENU_BEGIN;
    for (TbSint32 i = 0; i < m_ActiveMenuCount; ++i) {
        m_Button[m_MenuMap[i]].Enter(this);
        m_Button[m_MenuMap[i]].SetPos(TbVector2(0.0f, BUTTON_Y + i*BUTTON_INTERVAL_Y));
    }
    fit();
}

/*!
 * 生成開始
 * @author teco
 */
void UiPauseWindow::OnCreating()
{
    Super::OnCreating();
    if(m_ActiveMenuCount == MENU_MAX)
    {
        // JOIN判定
        TbUint32 playerBit = GetPlayerBit();
        if( TB_VERIFY(1==TbBitPopulation(playerBit)) )
        {
            TbSint32 playerCount = Root::GetInstance().GetPlayerCount();
            TbSint32 playerIndex = TbBitMostSignificant(playerBit);
            TbUiWidget& joinButton = m_Button[MENU_JOIN-MENU_BEGIN];
            if( playerIndex < playerCount ){
                if( joinButton.GetWindow() ){
                    joinButton.Exit();
                }
            }else{
                if( !joinButton.GetWindow() ){
                    joinButton.Enter(this);
                }
            }
            fit();
        }
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
    m_Button[m_MenuMap[m_FocusIndex]].SetFocus();
    for( TbSizeT i = 0; i < m_ActiveMenuCount; ++i ) {
        if( m_Button[m_MenuMap[i]].IsCreated() ){
            m_Button[m_MenuMap[i]].SetEnable(TB_TRUE);
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
    for( TbSint32 i = m_ActiveMenuCount-1; 0 <= i ; --i ) {
        if( m_Button[m_MenuMap[i]].GetWindow() ) {
            height = m_Button[m_MenuMap[i]].GetGlobalRightBottomPos().GetY();
            break;
        }
    }
    height -= GetClientRegion().y;
    for( TbSint32 i = 0; i < m_ActiveMenuCount; ++i ) {
        width = TbMax(width,m_Button[m_MenuMap[i]].GetWidth());
    }
    SetClientSize(width,height);
}
