/*!
 * プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_goal.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "system/sys_input.h"
#include "root.h"

#include <graphics/sprite/tb_sprite_node_tree.h>

static const TbRectF32Pod SPRITE_RECT = 
{
    -32.0f , -64.0f , 64.0f , 64.0f
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -24.0f , -32.0f , 48.0f , 32.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(96,512,48,48)
};

static const TbRectF32Pod SPRITE_UV_RECT_OPEN = 
{
    CMN_TEX_UV_RECT(96,576,48,48)
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionGoal::SideActionGoal()
    : Super()
{
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SPRITE_UV_RECT; //
    m_SpriteNode.Initialize(initParam);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(&m_SpriteNode);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_BACK_3);

    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_BACK_3);

    TbUint32 pushBackTarget = 0;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_GOAL);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionGoal::~SideActionGoal()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionGoal::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionGoal::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        Open();
        break;
    }
    return 0;
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionGoal::onCollided(ActionCollision* target)
{
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_KEY:
        {
            Open();
        }
        break;
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            if( m_BitArray.Test(FLAG_OPEN) ){
                SideActionPlayer* player = static_cast<SideActionPlayer*>(target->GetActor());
            
                // プレイヤーと接触している状態で↑を押すとプレイヤーをクリア状態にする
                if( player->IsPressInput(SysInput::BUTTON_UP) ){
                    player->RequestState(SideActionPlayer::STATE_CLEAR);
                }
            }
        }
        break;
    }
}

/*!
 * 開く
 * @author teco
 * @since 2014.08.02
 */
void SideActionGoal::Open()
{
    // 状態が変わる
    TbSpriteNodeTree* nodeTree = GetMainSprite().GetNodeTree();
    if( nodeTree ) {
        TbSpriteNode& node = nodeTree->GetNodeFromIndex(0)->GetBody();
        node.uvRect = SPRITE_UV_RECT_OPEN;
        m_BitArray.SetBit(FLAG_OPEN);
    }
}

/*!
 * 文字描画
 * @author teco
 * @since 2014.12.28
 */
void SideActionGoal::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
#if CMN_DEPRECATED
    if( IsHint() ){
        const TbVector2& drawPos = GetDrawPos();
        TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

        // ヒント
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                           static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y-36.0f),"2.GOAL");

        fontBuffer->PopSetting();
        fontBuffer->Flush();
    }
#endif
}