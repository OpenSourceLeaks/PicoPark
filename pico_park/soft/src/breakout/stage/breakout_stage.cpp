/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "breakout/stage/breakout_stage.h"
#include "breakout/actor/breakout_ball.h"
#include "breakout/actor/breakout_bar.h"
#include <crayon/collision/cr_action_collision.h>
#include <crayon/collision/cr_action_collision_scene.h>
#include <crayon/actor/cr_action_actor.h>
#
#include "root.h"
#include <base/util/tb_type_id.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
BreakoutStage::BreakoutStage( TbScriptLuaNode* node )
    : Super(node,
            1,
            TbFrameworkSimple::RENDER_PASS_2D,
            Root::GetInstance().GetCommonTexture(),
            nullptr,
            new CrActionCollisionScene())
    , m_BarCount(0)
{
    // コリジョンのレイヤー設定
    GetCollisionScene().SetEnableCheck(CrActionCollision::LAYER_DEFAULT, CrActionCollision::LAYER_DEFAULT,TB_TRUE);

    CrActionMap& map = GetMap();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
BreakoutStage::~BreakoutStage()
{
}

/*!
 * 更新
 * @author teco
 * @since 2013.10.02
 */
void BreakoutStage::OnUpdate( TbFloat32 deltatime )
{
    if( !IsActive() ){
        return;
    }

    // コリジョンシーン
    UpdateCollisionScene();
}

/*!
 * アクター生成
 * @author teco
 * @since 2013.10.20
 */
CrActor* BreakoutStage::OnCreatedActor(const CrActorFactory::CreateParam& param)
{
    TbVector2 initPos = param.pos;

    CrActionActor* actor = NULL;
    if( param.typeName == "Ball" ){
        actor = new BreakoutBall(TB_FALSE);

    }else if(param.typeName == "Bar" ){

        if( m_BarCount >= Root::GetInstance().GetPlayerCount() )
        {
            return nullptr;
        }
        actor = new BreakoutBar(m_BarCount);
        ++m_BarCount;

    }else{
        TB_ASSERT_MSG_FORMAT(0,"Invalid Name (%s)", param.name.GetBuffer());
    }
    if( actor ) {
        AddActor(actor,initPos);
        actor->Release();
    }
    return actor;
}

/*!
 * ステージクリアしたか
 * @author teco
 * @since 2013.10.23
 */
TbBool BreakoutStage::isCleared() const
{
    // ブロックが全て消えたら
    return TB_FALSE;
}

/*!
 * ゲームオーバー
 * @author teco
 * @since 2013.10.27
 */
TbBool BreakoutStage::isGameOver() const
{
    return TB_FALSE;
}
