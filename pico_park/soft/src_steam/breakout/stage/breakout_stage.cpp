/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "breakout/stage/breakout_stage.h"
#include "breakout/actor/breakout_ball.h"
#include "breakout/actor/breakout_bar.h"
#include "common/collision/action_collision.h"
#include "common/actor/action_actor.h"
#include "root.h"
#include <base/util/tb_type_id.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
BreakoutStage::BreakoutStage( TbScriptLuaNode* node )
    : Super(node,1)
    , m_BarCount(0)
{
    // コリジョンのレイヤー設定
    GetCollisionScene().SetEnableCheck(ActionCollision::LAYER_DEFAULT,ActionCollision::LAYER_DEFAULT,TB_TRUE);

    ActionMap& map = GetMap();
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
void BreakoutStage::OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData& userData)
{
    TB_RESQUE(name);
    TbVector2 initPos = pos;
    const TbRawString nameStr(typeName);

    ActionActor* actor = NULL;
    if( 0 == TbStrCmp(name,"Ball") ){
        actor = new BreakoutBall();

    }else if( nameStr == "Bar" ){

        if( m_BarCount >= Root::GetInstance().GetPlayerCount() )
        {
            return;
        }
        actor = new BreakoutBar(m_BarCount);
        ++m_BarCount;

    }else{
        TB_ASSERT_MSG_FORMAT(0,"Invalid Name (%s)",name);
    }
    if( actor ) {
        AddActor(actor,initPos);
        actor->Release();
    }
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
