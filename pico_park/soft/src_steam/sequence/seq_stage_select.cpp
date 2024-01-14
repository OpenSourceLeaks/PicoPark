/*!
 * ステージ選択シーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_stage_select.h"
#include "system/sys_input.h"
#include "common/cmn_global_data.h"
#include "sequence/seq_manager.h"
#include "root.h"

#include <base/time/tb_tick.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStageSelect::SeqStageSelect( TbSint32 focusIndex , TbSint32 count )
    : Super(SEQ_TITLE,"SeqStageSelect")
    , m_Phase(0)
    , m_PhaseTime(0.0f)
    , m_IdleSec(0.0f)
    , m_Menu(focusIndex,count)
    , m_ReturnWindow(UiMessageWindow::TYPE_YESNO)
    , m_InitIndex(focusIndex)
{
    Root::GetInstance().GetGlobalData().ResetCheckPoint();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStageSelect::~SeqStageSelect()
{
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.07
 */
void SeqStageSelect::OnUpdate()
{
    static const TbFloat32 RESET_SEC = 60.0f * 10.0f;
    if( SysInput::IsOnAny() ){
        m_IdleSec = 0.0f;
    }else{
        m_IdleSec += static_cast<TbFloat32>(TbTick::GetFrameSpeedMilliSeconds())/1000.0f;
        if( m_IdleSec > RESET_SEC ) {
            SeqManager::GetInstance().RequestSequence(SEQ_TITLE,1);
            return;
        }
    }

    switch( m_Phase )
    {
    case 0:
        CreateBgm("stage_select");
        ++m_Phase;
        break;
    case 1:
        if (!IsReadyBgm()) {
            break;
        }
        PlayBgm();
        m_Menu.Open();
        m_Menu.SetStageIndex(m_InitIndex);
        ++m_Phase;
        break;
    case 2:
        for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_MAX; ++playerIndex ){
            if( !SysInput::IsPress(SysInput::BUTTON_CANCEL,playerIndex) ) {
                continue;
            }
            m_ReturnWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
            m_ReturnWindow.SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
            m_ReturnWindow.SetMessage("Return title ?");
            m_ReturnWindow.SetPlayerBit(TB_BIT(playerIndex));
            m_ReturnWindow.Open();
            m_Menu.SetFocus(TB_FALSE);
            ++m_Phase;
            break;
        }
        break;
    case 3:
        {
            UiMessageWindow::Result res = m_ReturnWindow.GetResult();
            if( res == UiMessageWindow::RESULT_NONE ) {
                break;
            }
            if( res == UiMessageWindow::RESULT_YES ){
                // タイトルに戻る
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE,1);
            }else{
                m_Menu.SetFocus(TB_TRUE);
                --m_Phase;
            }
        }
        break;
    };
}

/*!
 * 描画関数
 * @author teco
 * @since 2013.10.27
 */
void SeqStageSelect::OnDraw()
{
}
