/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_player_window.h"
#include "common/cmn_util.h"
#include "system/sys_input.h"
#include "root.h"

static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};

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
        m_PlayerImageBox.SetSize(44.0f*1.5f,48.0f*1.5f);
        m_PlayerImageBox.SetImage(nullptr,PLAYER_UV_RECT);
        m_PlayerImageBox.SetMaterial(&m_Material);
        m_PlayerImageBox.SetColor(CmnUtil::GetPlayerColor(0));
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
    }else{
        // キャンセル
        if( IsEnableInput() && SysInput::IsAnyonePress(SysInput::BUTTON_CANCEL,GetPlayerBit()) ){
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
        TbUint32 index = SysInput::GetPlayerPadIndex(TbBitLeastSignificant(GetPlayerBit()));
        m_PlayerImageBox.SetColor(CmnUtil::GetPlayerColor(index));
    }
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
    return y + 8.0f;
}
