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
    : Super(1, CMN_SPRITE_MAX, TbFrameworkSimple::RENDER_PASS_2D,Root::GetInstance().GetCommonTexture())
{
    m_Map.Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map.EnterSpriteScene(&GetSpriteScene(0));

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
CrActor* PuzzleDrStage::OnCreatedActor(const CrActorFactory::CreateParam& param)
{
    CrActor* actor = nullptr;
    if(param.typeName == "Block" ){
        PuzzleBlockBase* block = new PuzzleBlockDr();
        block->SetStartPoint(static_cast<TbSint32>(param.pos.GetX()), static_cast<TbSint32>(param.pos.GetY()));
        block->SetMap(&m_Map);
        m_Map.AddPlayerBlock(block);
        actor= block;
    }else{
        TB_ASSERT_MSG_FORMAT(0,"unknown name %s",param.name.GetBuffer());
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
void PuzzleDrStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"PuzzleDrStage");
    if( !menu ){
        return;
    }
    m_Map.OnCreateDebugMenu(menu);
}
