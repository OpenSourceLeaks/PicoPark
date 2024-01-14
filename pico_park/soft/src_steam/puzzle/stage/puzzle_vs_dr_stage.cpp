/*!
 * パズルステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/stage/puzzle_vs_dr_stage.h"
#include "root.h"

#include <base/math/tb_random.h>
#include <base/util/tb_type_id.h>
#include <input/tb_input_setting.h>

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleVsDrStage::PuzzleVsDrStage( TbScriptLuaNode* node )
    : Super(1)
    , m_Phase(0)
    , m_Timer(nullptr)
    , m_Winner(TEAM_INVALID)
    , m_MessageWindow(UiMessageWindow::TYPE_NORMAL)
{
    node->GoToChild("left");
    m_Map[TEAM_LEFT].Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map[TEAM_LEFT].EnterSpriteScene(&GetSpriteScene());
    node->GoToParent(1);

    node->GoToChild("right");
    m_Map[TEAM_RIGHT].Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map[TEAM_RIGHT].EnterSpriteScene(&GetSpriteScene());
    node->GoToParent(1);

    // アクター生成
    GetActorFactory().Initialize(node);

    // 判定結果ウィンドウ
    m_MessageWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
}

/*!
 * デストラクタ
 * @author teco
 */
PuzzleVsDrStage::~PuzzleVsDrStage()
{

}

/*!
 * 更新
 * @author teco
 */
void PuzzleVsDrStage::OnUpdate( TbFloat32 deltatime )
{
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Map); ++i ) {
        m_Map[i].OnUpdate(deltatime);
    }
    switch( m_Phase )
    {
    case PHASE_PLAY:
        if( m_Map[TEAM_LEFT].IsGameOver() ){
            m_Winner = TEAM_RIGHT;
        }else if( m_Map[TEAM_RIGHT].IsGameOver() ){
            m_Winner = TEAM_LEFT;
        }else if( 0 == m_Map[TEAM_LEFT].GetTargetCount() ){
            m_Winner = TEAM_LEFT;
        }else if( 0 == m_Map[TEAM_RIGHT].GetTargetCount() ){
            m_Winner = TEAM_RIGHT;
        }else if( m_Timer && m_Timer->IsTimeUp() ){
            if( m_Map[TEAM_LEFT].GetTargetCount() < m_Map[TEAM_RIGHT].GetTargetCount() ){
                m_Winner = TEAM_LEFT;
            }else if( m_Map[TEAM_RIGHT].GetTargetCount() < m_Map[TEAM_LEFT].GetTargetCount() ){
                m_Winner = TEAM_RIGHT;
            }
        }
        if( m_Winner != TEAM_INVALID ){
            GetActorScene().SetEnableUpdate(TB_FALSE);
            m_Phase = PHASE_RESULT;
        }
        break;
    case PHASE_RESULT:
        {
            m_MessageWindow.SetPlayerBit(0xFFFFFFFF);
            m_MessageWindow.SetVisiblePlayerImage(TB_FALSE);
            if( TEAM_LEFT == m_Winner ){
                m_MessageWindow.SetMessage("WINNER LEFT");
            }else{
                m_MessageWindow.SetMessage("WINNER RIGHT");
            }
            m_MessageWindow.Open();
            m_MessageWindow.ReserverClose(15.0f);
            ++m_Phase;
        }
        break;
    case PHASE_RESULT+1:
        {
            if( m_MessageWindow.IsOpened() ){
                break;
            }
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT,GetId());
        }
        break;
    }
}

/*!
 * アクター生成
 * @author teco
 */
void PuzzleVsDrStage::OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData &userData)
{
    TB_RESQUE(name);

    TbStaticString32 typeNameStr = typeName;

    if( typeNameStr == "Block" ){
        PuzzleBlockBase* block = new PuzzleBlockDr(userData.AsSint32(0));
        block->SetStartPoint(static_cast<TbSint32>(pos.GetX()), static_cast<TbSint32>(pos.GetY()));
        block->SetStage(this);
        if( userData.AsSint32(1) ){
            block->SetMap(&m_Map[TEAM_RIGHT]);
            m_Map[TEAM_RIGHT].AddPlayerBlock(block);
        }else{
            block->SetMap(&m_Map[TEAM_LEFT]);
            m_Map[TEAM_LEFT].AddPlayerBlock(block);
        }
        block->Enter(&GetActorScene(),0,this);
        block->Release();
    }else if( typeNameStr == "Timer" ) {
        if( TB_VERIFY(!m_Timer) ){
            m_Timer = new PuzzleTimer(PuzzleTimer::TYPE_DOWN);
            m_Timer->SetTime(userData.AsSint32(0));
            m_Timer->SetStage(this);
            m_Timer->SetPos(pos);
            m_Timer->Enter(&GetActorScene(),0,this);
            m_Timer->Release();
        }
    }else{
        TB_ASSERT_MSG_FORMAT(0,"unknown name %s",name);
    }
}

/*!
 * デバッグメニュー生成
 * @author teco
 */
void PuzzleVsDrStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"PuzzleVsDrStage");
    if( !menu ){
        return;
    }
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Map); ++i ) {
        m_Map[i].OnCreateDebugMenu(menu);
    }
}
