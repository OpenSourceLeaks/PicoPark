/*!
 * メッセージウィンドウ
 * @author teco
 * @since 2014.09.30
 */

#include "fwd.h"
#include "ui/ui_player_window.h"
#include "root.h"

static const TbRectF32Pod PLAYER_UV_RECT = {CMN_TEX_UV_RECT(5.0f,3.0f,22.0f,24.0f)};

/*!
 * コンストラクタ
 * @author teco
 * @since 2014.12.05
 */
UiPlayerWindow::UiPlayerWindow()
    : Super(TB_TRUE)
    , m_CloseTimer(0.0f)
{
    SetPriority(1.0f);

    m_PlayerImageBox.Enter(this);
    m_PlayerImageBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_PlayerImageBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_PlayerImageBox.SetSize(44.0f,48.0f);
    m_PlayerImageBox.SetImage(nullptr,PLAYER_UV_RECT);

    SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
    SetEnableCloseButton(TB_FALSE);
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
        if( CrInput::IsAnyonePress(CrInput::BUTTON_CANCEL,GetPlayerBit()) ){
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
    TbRectF32 uv(PLAYER_UV_RECT);
    TbFloat32 offsetY = 0.0f;
    if( TbBitPopulation(playerBit) > 1 ) { // 複数立っている場合
        uv.y += CMN_TEX_POS(64.0f);
    }else{
        TbUint32 lsb = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
        uv.y += CMN_TEX_POS(32.0f*(lsb));
    }
    m_PlayerImageBox.SetImage(nullptr,uv);
    updateUv();
}

/*! 
 * ウィンドウ内のY座標取得
 * @author teco
 * @since 2015.01.01
 */
TbFloat32 UiPlayerWindow::GetLocalTopY() const
{
    TbFloat32 y = 0.0f;
    if( m_PlayerImageBox.IsVisible() ){
        y = m_PlayerImageBox.GetPos().GetY() + m_PlayerImageBox.GetHeight();
    }
    return y + 8.0f;
}

// 
/*! 
 * UV更新
 * @author teco
 * @since 2015.01.01
 */

void UiPlayerWindow::updateUv()
{
    TbElasticSprite::UvParam param;

    TbFloat32 offsetX = 0.0f;
    TbFloat32 offsetY = 0.0f;
    if( TbBitPopulation(GetPlayerBit()) > 1 ) { // 複数立っている場合
        offsetX += CMN_TEX_POS(64.0f);
    }else{
        TbUint32 lsb = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(GetPlayerBit()));
        offsetX += CMN_TEX_POS(32.0f*(lsb%5));
        offsetY += CMN_TEX_POS(32.0f*(lsb/5));
    }
    const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV(TB_UI_UV_WINDOW);
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(uv->rectUV); ++i ) {
        param.rectUV[i] = uv->rectUV[i];
        param.rectUV[i].x += offsetX;
        param.rectUV[i].y += offsetY;
    }
    SetDefaultUV(param);
}
