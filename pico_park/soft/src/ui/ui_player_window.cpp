/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_player_window.h"
#include "common/cmn_util.h"
#include <crayon/input/cr_input.h>
#include "root.h"

static const TbRectF32Pod PLAYER_UV_RECT = { CMN_TEX_UV_RECT(32 * 5 + 5, 8, 22, 24) };

static TbFloat32 HEAD_WIDTH = 25.0f;
static TbFloat32 HEAD_HEIGHT = 13.0f;
static const TbRectF32Pod HEAD_UV_RECT = { CMN_TEX_UV_RECT(256,464,HEAD_WIDTH - 1,HEAD_HEIGHT - 1) };

static TbRectF32Pod PLAYER_RECT = { 0.0f , 0.0f , 44.0f*1.5f,48.0f*1.5f };
static TbRectF32Pod HELMET_RECT = { 0.0f , -12.0f , HEAD_WIDTH*3.0f, HEAD_HEIGHT*3.0f };
static TbRectF32Pod NUMBER_RECT = { 2.0f , -4.0f , 24.0f, 24.0f };
static TbFloat32    HELMET_SCALE = 0.9f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiPlayerWindow::UiPlayerWindow( TbBool isEnableImage )
    : Super(TB_TRUE)
    , m_CloseTimer(0.0f)
{
    SetPriority(1.0f);

    if( isEnableImage )
    {
        m_PlayerImageBox.Enter(this);
        m_PlayerImageBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_PlayerImageBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_PlayerImageBox.SetSize(PLAYER_RECT.width, PLAYER_RECT.height);
        m_PlayerImageBox.SetImage(nullptr,PLAYER_UV_RECT);
        m_PlayerImageBox.SetMaterial(&m_Material);
        m_PlayerImageBox.SetColor(CmnUtil::GetPlayerColor(0));

        m_HelmetPlayerImageBox.Enter(this);
        m_HelmetPlayerImageBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_HelmetPlayerImageBox.SetPosY(HELMET_RECT.y);
        m_HelmetPlayerImageBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_HelmetPlayerImageBox.SetSize(HELMET_RECT.width, HELMET_RECT.height);
        m_HelmetPlayerImageBox.SetImage(nullptr, HEAD_UV_RECT , 0.01f);
        m_HelmetPlayerImageBox.SetMaterial(&m_Material);
        m_HelmetPlayerImageBox.SetColor(CmnUtil::GetPlayerColor(0));

        m_PlayerNumberTextBox.Enter(this);
        m_PlayerNumberTextBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
        m_PlayerNumberTextBox.SetPos(NUMBER_RECT.x, NUMBER_RECT.y);
        m_PlayerNumberTextBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
        m_PlayerNumberTextBox.SetFontSize(NUMBER_RECT.width);
        m_PlayerNumberTextBox.SetText("1");

        m_BitArray.SetBit(FLAG_ENABLE_IMAGE);
    }

    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);

    // SetWindowMaterial(&m_Material);
    // SetWindowColor(CmnUtil::GetPlayerColor(0));
}


/*!
 * デストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiPlayerWindow::~UiPlayerWindow()
{
    ClearWidget();
}

/*!
 * 閉じるを時間付きでリクエスト
 * @author teco
 * @since 2014.12.05
 */
void UiPlayerWindow::RequestClose( TbFloat32 delayTime )
{
    if( !IsOpened() ){
        return;
    }
    m_CloseTimer = delayTime;
    m_BitArray.SetBit(FLAG_REQUEST_CLOSE);
}

/*!
 * 派生クラス用
 * @author teco
 * @since 2014.12.05
 */
void UiPlayerWindow::OnUpdate( TbFloat32 deltasec  )
{
    if( !IsOpened() ) {
        return;
    }

    if( m_BitArray.Test(FLAG_REQUEST_CLOSE) ){
        // 遅延クローズ
        m_CloseTimer -= deltasec;
        if( m_CloseTimer <= 0.0f ){
            // 閉じる
            Close();
            m_BitArray.ResetBit(FLAG_REQUEST_CLOSE);
        }
    }else if( IsEnableCloseButton() ){
        // キャンセル
        if( IsEnableInput() && CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL,GetPlayerBit()) ){
            TbUiManager::GetInstance().PlaySE(TB_UI_SE_CANCEL);
            OnCancel();
        }
    }
}

/*!
 * プレイヤービットが変更された
 * @author teco
 * @since 2014.12.27
 */
void UiPlayerWindow::OnPlayerBitChanged( TbUint32 playerBit )
{
    if( TbBitPopulation(playerBit) > 1 ) { // 複数立っている場合
        m_PlayerImageBox.SetColor(CmnUtil::GetPlayerColor(2));
    }else{
        TbUint32 index = TbBitLeastSignificant(GetPlayerBit());
        m_PlayerImageBox.SetColor(CmnUtil::GetPlayerColor(index));
        m_HelmetPlayerImageBox.SetColor(CmnUtil::GetPlayerColor(index));
        TbStaticString32 text = TbStaticString32::Format("%u",index+1);
        m_PlayerNumberTextBox.SetText(text.GetBuffer());
    }
}

/*!
 * 生成開始時
 * @author teco
 */
void UiPlayerWindow::OnCreating()
{
    TbFloat32 scale = 1.0f;
    if (SaveFile::GetInstance().IsVisiblePlayerNumber() &&
        IsVisiblePlayerImage() ) 
    {
        m_HelmetPlayerImageBox.SetVisible(TB_TRUE);
        m_PlayerNumberTextBox.SetVisible(TB_TRUE);
        scale = HELMET_SCALE;
    } else {
        m_HelmetPlayerImageBox.SetVisible(TB_FALSE);
        m_PlayerNumberTextBox.SetVisible(TB_FALSE);
    }

    TbFloat32 y = (PLAYER_RECT.height) - (PLAYER_RECT.height*scale);
    m_PlayerImageBox.SetPosY(y);
    m_PlayerImageBox.SetSize(PLAYER_RECT.width*scale, PLAYER_RECT.height*scale);
    m_HelmetPlayerImageBox.SetPosY(HELMET_RECT.y*scale+y);
    m_HelmetPlayerImageBox.SetSize(HELMET_RECT.width*scale, HELMET_RECT.height*scale);
    m_PlayerNumberTextBox.SetPos(NUMBER_RECT.x*scale, NUMBER_RECT.y*scale+y);
    m_PlayerNumberTextBox.SetFontSize(NUMBER_RECT.width*scale);
}

/*! 
 * ウィンドウ内のY座標取得
 * @author teco
 * @since 2015.01.01
 */
TbFloat32 UiPlayerWindow::GetLocalTopY() const
{
    TbFloat32 y = 0.0f;
    if( m_BitArray.Test(FLAG_ENABLE_IMAGE) && m_PlayerImageBox.IsVisible() ){
        y = m_PlayerImageBox.GetPos().GetY() + m_PlayerImageBox.GetHeight();
    }
    return y + 12.0f;
}
