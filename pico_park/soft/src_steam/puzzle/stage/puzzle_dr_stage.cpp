/*!
 * パズルステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "puzzle/stage/puzzle_dr_stage.h"
#include "root.h"

#include <base/math/tb_random.h>
#include <base/util/tb_type_id.h>
#include <input/tb_input_setting.h>

/*!
 * コンストラクタ
 * @author teco
 */
PuzzleDrStage::PuzzleDrStage( TbScriptLuaNode* node )
    : Super(1)
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
PuzzleDrStage::~PuzzleDrStage()
{

}

/*!
 * 更新
 * @author teco
 */
void PuzzleDrStage::OnUpdate( TbFloat32 deltatime )
{
    m_Map.OnUpdate(deltatime);

    if( m_Map.IsGameOver() ){
        RequestGameOver();
    }
    if( 0 == m_Map.GetTargetCount() ) {
        RequestClear();
    }
}

/*!
 * アクター生成
 * @author teco
 */
void PuzzleDrStage::OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData &userData)
{
    TB_RESQUE(name);

    if( 0 == TbStrCmp(typeName,"Block") ){
        PuzzleBlockBase* block = new PuzzleBlockDr(userData.AsSint32(0));
        block->SetStartPoint(static_cast<TbSint32>(pos.GetX()), static_cast<TbSint32>(pos.GetY()));
        block->SetStage(this);
        block->SetMap(&m_Map);
        block->Enter(&GetActorScene(),0,this);
        block->Release();
        m_Map.AddPlayerBlock(block);
    }else{
        TB_ASSERT_MSG_FORMAT(0,"unknown name %s",name);
    }
}

/*!
 * デバッグメニュー生成
 * @author teco
 */
void PuzzleDrStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"PuzzleDrStage");
    if( !menu ){
        return;
    }
    m_Map.OnCreateDebugMenu(menu);
}
