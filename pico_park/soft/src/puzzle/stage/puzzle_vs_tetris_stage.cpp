/*!
 * パズルステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/stage/puzzle_vs_tetris_stage.h"
#include "root.h"

#include <base/math/tb_random.h>
#include <base/util/tb_type_id.h>
#include <input/tb_input_setting.h>

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleVsTetrisStage::PuzzleVsTetrisStage( TbScriptLuaNode* node , Type type )
    : Super(1, CMN_SPRITE_MAX, TbFrameworkSimple::RENDER_PASS_2D, Root::GetInstance().GetCommonTexture())
    , m_Type(type)
    , m_Phase(PHASE_PLAY)
    , m_Timer(nullptr)
    , m_Winner(TEAM_INVALID)
    , m_MessageWindow(UiMessageWindow::TYPE_NORMAL)
{
    node->GoToChild("left");
    m_Map[TEAM_LEFT].Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map[TEAM_LEFT].EnterSpriteScene(&GetSpriteScene(0));
    node->GoToParent(1);

    node->GoToChild("right");
    m_Map[TEAM_RIGHT].Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map[TEAM_RIGHT].EnterSpriteScene(&GetSpriteScene(0));
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
PuzzleVsTetrisStage::~PuzzleVsTetrisStage()
{

}

/*!
 * 更新
 * @author teco
 */
void PuzzleVsTetrisStage::OnUpdate( TbFloat32 deltatime )
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
        }else if( m_Timer && m_Timer->IsTimeUp() ){
            if( m_Map[TEAM_LEFT].GetLineCount() < m_Map[TEAM_RIGHT].GetLineCount() ){
                m_Winner = TEAM_RIGHT;
            }else if( m_Map[TEAM_RIGHT].GetLineCount() < m_Map[TEAM_LEFT].GetLineCount() ){
                m_Winner = TEAM_LEFT;
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
CrActor* PuzzleVsTetrisStage::OnCreatedActor(const CrActorFactory::CreateParam& param)
{
    CrActor* actor = nullptr;
    if(param.typeName == "Block" ){
        PuzzleBlockBase* block = nullptr;
        switch (m_Type)
        {
        case TYPE_4:
            block = new PuzzleBlockTetris(PuzzleBlockTetris::TYPE_DEFAULT);
            break;
        case TYPE_3:
            block = new PuzzleBlockTetris(PuzzleBlockTetris::TYPE_3);
            break;
        default:
            TB_ASSERT(0);
            break;
        }
        block->SetStartPoint(static_cast<TbSint32>(param.pos.GetX()), static_cast<TbSint32>(param.pos.GetY()));
        if( param.userData.AsSint32(1) ){
            block->SetMap(&m_Map[TEAM_RIGHT]);
            m_Map[TEAM_RIGHT].AddPlayerBlock(block);
        }else{
            block->SetMap(&m_Map[TEAM_LEFT]);
            m_Map[TEAM_LEFT].AddPlayerBlock(block);
        }
        actor = block;
    }else if(param.typeName == "Timer" ) {
        if( TB_VERIFY(!m_Timer) ){
            m_Timer = new PuzzleTimer(PuzzleTimer::TYPE_DOWN);
            m_Timer->SetPos(param.pos);
            actor = m_Timer;
        }
    }else{
        TB_ASSERT_MSG_FORMAT(0,"unknown name %s", param.name.GetBuffer());
    }
    if (actor) {
        actor->Setup(param.userData);
        actor->SetStage(this);
        actor->Enter(&GetActorScene(), 0, this);
        actor->Release();
    }
    return actor;
}

/*!
 * デバッグメニュー生成
 * @author teco
 */
void PuzzleVsTetrisStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"PuzzleVsTetrisStage");
    if( !menu ){
        return;
    }
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Map); ++i ) {
        m_Map[i].OnCreateDebugMenu(menu);
    }
}
