/*!
 * レーザー用ボール
 * @author teco
 */

#include "fwd.h"
#include "side_action_laser_ball.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
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
    , m_DeadTime(0)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetName("ball");

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );
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
void SideActionLaserBall::onContacted(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info)
{
    if( 0 == m_DeadTime ){
        m_DeadTime = DEAD_COUNT;
        ResetVelo();
        GetCollision()->SetActive(TB_FALSE);
    }
}
