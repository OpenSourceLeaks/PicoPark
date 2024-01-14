/*!
 * 物理ボール
 * @author teco
 */

#include "fwd.h"
#include "side_action_physics_ball.h"
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

static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 30;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbFloat32 SCALE = 1.05f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionPhysicsBall::SideActionPhysicsBall()
    : Super()
    , m_DeadTime(0)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetName("ball");

    // コリジョン
    m_CircleShape.Set( RADIUS );
    m_CircleShape.SetDensity(0.1f);
    m_CircleShape.SetFriction(0.5f);
    m_CircleShape.SetRestitution(0.2f);
    m_Body.GetBody().SetType( TbPhysicsBody2d::TYPE_DYNAMIC );
    m_Body.GetBody().SetDampingAngle( 0.5f );
    m_Body.GetBody().SetShape(&m_CircleShape);
    AddComponent(&m_Body);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionPhysicsBall::~SideActionPhysicsBall()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionPhysicsBall::OnUpdateAction( TbFloat32 deltatime )
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
        }else if( m_DeadTime < (DEAD_COUNT/2) ){
            m_Body.GetBody().GetDepend().GetBody()->SetAwake(TB_FALSE);
        }
    }else{
        b2ContactEdge* list = m_Body.GetBody().GetDepend().GetBody()->GetContactList();
        for( ; list; list = list->next ){
            TbPhysicsBody2d* body = reinterpret_cast<TbPhysicsBody2d*>(list->other->GetUserData());
            if( body->GetLayer() == SIDE_ACTION_PHYSICS_LAYER_AREA ) {
                b2Vec2 local = list->contact->GetManifold()->localNormal;
                if( TB_FLOAT32_EQUAL(1.0f,TbAbs(local.y)) ){
                    m_DeadTime = DEAD_COUNT;
                }
            }
        }
    }
}

/*! 
 * ポスト更新
 * @author teco
 */
void SideActionPhysicsBall::OnPostAction( TbFloat32 deltatime )
{
    const TbVector2& pos = m_Body.GetBody().GetPos();
    TbRadian angle = m_Body.GetBody().GetAngle();
    SetPos( pos.GetX() , pos.GetY() );
    GetMainSprite().SetRotation( TbRadToAng32(angle) );
}

/*!
 * イベント通知
 * @author Miyake_Shunske
 * @since 2015.01.26
 */ 
TbSint32 SideActionPhysicsBall::NotifyEvent( TbUint32 event , void* param )
{
    return 0;
}

