/*!
 * マーカー
 * @author teco
 * @since 2014.05.25
 */

#include "fwd.h"
#include "side_action_tetris.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 */
SideActionTetris::SideActionTetris()
    : Super()
    , m_Norma(5)
    , m_PuzzleStage(nullptr)
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionTetris::~SideActionTetris()
{
    TB_SAFE_DELETE(m_PuzzleStage);
}

/*!
 * 初期化
 * @author teco
 */
TbResult SideActionTetris::InitializeByScript( const char* nodePath )
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode*    node   = script.GetVM(0)->GetNode(0);
    TbScriptLuaNode::Result result = node->GoToAbsolutely( nodePath );
    if( result != TbScriptLuaNode::RESULT_OK ) {
        return TB_E_FAIL;
    }
    TB_SAFE_DELETE(m_PuzzleStage);
    m_PuzzleStage = new PuzzleTetrisStage(node,PuzzleTetrisStage::BLOCK_TYPE_3);
    m_PuzzleStage->SetNorma(m_Norma);
    m_PuzzleStage->SetActive(TB_TRUE);
    if( m_NodePath.GetBuffer() != nodePath ) {
        m_NodePath = nodePath;
    }
    return TB_S_OK;
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionTetris::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_PuzzleStage && !m_BitArray.Test(FLAG_CLEAR) ) {
        m_PuzzleStage->Update();
        if( m_PuzzleStage->IsRequestedGameOver() ){
            InitializeByScript( m_NodePath.GetBuffer() );
        }
        if( m_PuzzleStage->IsRequestedClear() ){
            m_PuzzleStage->SetPause(TB_TRUE);
            GetStage()->SetPausePlayerInput(TB_FALSE);
            if( !m_SwitchTarget.IsEmpty() ){
                TbSint32 param = 0;
                GetStage()->NotifyEvent(m_SwitchTarget.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,&param);
            }
            GetStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_WAKEUP,nullptr);
            m_BitArray.SetBit(FLAG_CLEAR);
        }
    }
    TbVector2 pos = GetDrawPos();
    TbMatrix poseMatrix = TbMatrix::IDENTITY;
    poseMatrix.SetTranslation(pos.GetX(),pos.GetY(),0.0f);
    m_PuzzleStage->GetSpriteScene().SetPoseMatrix(poseMatrix);
}

/*!
 * 描画更新
 * @author teco
 */
void SideActionTetris::OnDraw( TbFloat32 deltatime )
{
    if( m_PuzzleStage ){
        m_PuzzleStage->Draw();
        if( m_PuzzleStage->IsRequestedClear() )
        {
            TbVector2 drawPos( CMN_WINDOW_WIDTH/2.0f , 300.0f );
            TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
            fontBuffer->PushSetting();
            fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
            fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
            // 数
            fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                               static_cast<TbSint32>(drawPos.GetY()),"OK");
            fontBuffer->PopSetting();
            fontBuffer->Flush();
        }
    }
}

/*!
 * シーンに入った
 * @author teco
 */
void SideActionTetris::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    GetStage()->SetPausePlayerInput(TB_TRUE);
    GetStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_SLEEP,nullptr);
}

/*!
 * シーンに入った
 * @author teco
 */
void SideActionTetris::OnExit()
{
    Super::OnExit();
    GetStage()->SetPausePlayerInput(TB_FALSE);
}
