/*!
 * レーザー用ボール
 * @author teco
 */

#include "fwd.h"
#include "side_action_laser_ball.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static TbFloat32 RADIUS = 12.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -RADIUS , -RADIUS , RADIUS*2.0f , RADIUS*2.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(272,496,12,12),
};

static const TbCircleF32 COLLISION_CIRCLE( 0.0f , 0.0f , RADIUS );

static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 30;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbFloat32 SCALE = 1.05f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionLaserBall::SideActionLaserBall()
    : Super()
    , m_Type(TYPE_NORMAL)
    , m_DeadTime(0)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetName("ball");

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetContactedCallback( TbCreateMemFunc(this,&Self::onContacted) );
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionLaserBall::~SideActionLaserBall()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionLaserBall::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_DeadTime > 0 ) {
        // 死亡時
        TbSint32 animCount = m_DeadTime - (DEAD_COUNT-ANIM_COUNT);
        if( animCount >= 0 )
        {
            // 消えるアニメーション
            TbSprite& sprite = GetMainSprite();
            TbVector2 scale = sprite.GetScale();
            if( !m_BitArray.Test(FLAG_DISABLE_DEAD_FADE) ){
                scale *= SCALE;
            }
            sprite.SetScale( scale );

            TbFloat32 alpha = static_cast<TbFloat32>(animCount) / static_cast<TbFloat32>(ANIM_COUNT);
            if( alpha <= 0.0f || !m_BitArray.Test(FLAG_DISABLE_DEAD_FADE) ){
                sprite.SetAlpha(alpha);
            }
        }

        --m_DeadTime;
        if( 0 == m_DeadTime ) {
            if( m_DeadCallback.IsValid() ){
                m_DeadCallback();
            }
            Exit();
        }
    }
}


/*!
 * 接触開始コールバック
 * @author teco
 */
void SideActionLaserBall::onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info)
{
    if( 0 == m_DeadTime ){
        m_DeadTime = DEAD_COUNT;
        ResetVelo();
        GetCollision()->SetActive(TB_FALSE);
    }
    TbPlaySound("ball_hit");

    if (TYPE_NORMAL == m_Type )
    {
        if ( info.collision && 
             info.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER )
        {
            if(!m_EventTarget.IsEmpty()){
                // 回復
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
                stage->NotifyEvent(m_EventTarget.GetBuffer(),SIDE_ACTION_EVENT_RECOVER,nullptr);
            }
        }
    }
    else if ( TYPE_SCORE == m_Type )
    {
        if (info.collision &&
            info.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER)
        {
            // ゲームオーバー
            if (!m_EventTarget.IsEmpty()) {
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
                stage->NotifyEvent(m_EventTarget.GetBuffer(), SIDE_ACTION_EVENT_GAMEOVER, nullptr);
            }
        }
        else
        {
            // カウントアップ
            if (!m_EventTarget.IsEmpty()) {
                SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
                stage->NotifyEvent(m_EventTarget.GetBuffer(), SIDE_ACTION_EVENT_COUNT_UP, nullptr);
            }
        }
    }
    else if (TYPE_DEAD == m_Type)
    {
        if (info.collision &&
            info.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER)
        {
            if (info.collision->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_DEAD, nullptr))
            {
                TbPlaySound("hit");
            }
        }
    }
}

// スケール設定
void SideActionLaserBall::SetScale(TbFloat32 scale) 
{
    CrActionCollision* collision = GetCollision();
    TbSprite& sprite = GetMainSprite();
    sprite.SetScale(TbVector2(scale, scale));
    collision->SetScaleImmediate(scale);
}
