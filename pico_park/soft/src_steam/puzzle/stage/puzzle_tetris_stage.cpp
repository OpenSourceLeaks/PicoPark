/*!
 * パズルステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/stage/puzzle_tetris_stage.h"
#include "root.h"

#include <base/math/tb_random.h>
#include <base/util/tb_type_id.h>
#include <input/tb_input_setting.h>

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleTetrisStage::PuzzleTetrisStage( TbScriptLuaNode* node , BlockType type )
    : Super(1)
    , m_Type(type)
{
    m_Map.Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map.EnterSpriteScene(&GetSpriteScene());

    // アクター生成
    GetActorFactory().Initialize(node);
}

/*!
 * デストラクタ
 * @author teco
 */
PuzzleTetrisStage::~PuzzleTetrisStage()
{
}

/*!
 * 更新
 * @author teco
 */
void PuzzleTetrisStage::OnUpdate( TbFloat32 deltatime )
{
    m_Map.OnUpdate(deltatime);

    if( m_Map.IsGameOver() ){
        RequestGameOver();
    }
    if( 0 < m_Map.GetNormaLineCount() ) {
        if( m_Map.GetNormaLineCount() <= m_Map.GetLineCount() ) {
            RequestClear();
        }
    }
}

/*!
 * アクター生成
 * @author teco
 */
void PuzzleTetrisStage::OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData &userData)
{
    TB_RESQUE(name);

    if( 0 == TbStrCmp(typeName,"Block") ){
        PuzzleBlockBase* block = nullptr;
        switch (m_Type)
        {
        case BLOCK_TYPE_4:
            block = new PuzzleBlockTetris(userData.AsSint32(0),PuzzleBlockTetris::TYPE_DEFAULT);
            break;
        case BLOCK_TYPE_3:
            block = new PuzzleBlockTetris(userData.AsSint32(0),PuzzleBlockTetris::TYPE_3);
            break;
        default:
            TB_ASSERT(0);
            break;
        }
        block->SetStartPoint(static_cast<TbSint32>(pos.GetX()), static_cast<TbSint32>(pos.GetY()));
        block->SetStage(this);
        block->SetMap(&m_Map);
        block->Enter(&GetActorScene(),0,this);
        m_Map.AddPlayerBlock(block);
        block->Release();
    }else{
        TB_ASSERT_MSG_FORMAT(0,"unknown name %s",name);
    }
}

/*!
 * デバッグメニュー生成
 * @author teco
 * @since 2014.05.11
 */
void PuzzleTetrisStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"PuzzleTetrisStage");
    if( !menu ){
        return;
    }
    m_Map.OnCreateDebugMenu(menu);
}
