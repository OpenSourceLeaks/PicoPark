/*!
 * 点滅ブロック
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_blink_block.h"
#include "side_action/side_action_types.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    0.0f , 0.0f , 32.0f , 32.0f
};
static const TbRectF32Pod SPRITE_RECT_BIG =
{
    0.0f , 0.0f , 48.0f , 48.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(0,320,16,16) , // TILE_MAP_CHIP_BLOCK ,
};
static const TbRectF32Pod SPRITE_UV_RECT_BIG =
{
    CMN_TEX_UV_RECT(0,368,24,24) , // TILE_MAP_CHIP_BLOCK ,
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};
static const TbRectF32Pod COLLISION_RECT_BIG=
{
    SPRITE_RECT_BIG.x + 1.0f , SPRITE_RECT_BIG.y + 1 , SPRITE_RECT_BIG.width - 2.0f , SPRITE_RECT_BIG.height - 2.0f
};

static const TbFloat32 BLINK_INTERVAL = 1.0f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBlinkBlock::SideActionBlinkBlock()
    : m_BlinkInterval(BLINK_INTERVAL)
    , m_Timer(0.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBlinkBlock::~SideActionBlinkBlock()
{
}

/*!
 * シーンに入った
 * @author teco
 */
void SideActionBlinkBlock::OnEnter(void* userData)
{
    TbSprite& sprite = GetMainSprite();
    if (!sprite.IsInitialized())
    {
        sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
        sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

        TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
        if (1.0f < GetStage()->GetScale()) {
            sprite.Initialize(SPRITE_RECT_BIG, SPRITE_UV_RECT_BIG);
            CrActionCollision* collision = CreateCollision(COLLISION_RECT_BIG, pushBackTarget, TB_TRUE);
        }
        else {
            sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
            CrActionCollision* collision = CreateCollision(COLLISION_RECT, pushBackTarget, TB_TRUE);
        }
        SyncCollisionPosForce();
        sprite.SetZ(CMN_SPRITE_Z_FRONT_1);
    }

    Super::OnEnter(userData);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBlinkBlock::OnUpdateAction( TbFloat32 deltatime )
{
    m_Timer += deltatime;
    if( m_BlinkInterval <= m_Timer ) {
        CrActionCollision* collision = GetCollision();
        if( GetMainSprite().IsVisible() ) {
            GetMainSprite().SetVisible(TB_FALSE);
            collision->SetActive(TB_FALSE);
            m_Timer = 0.0;
        }else{
            // 出現可能な場合のみ再表示
            GetMainSprite().SetVisible(TB_TRUE);
            if( !collision->GetScene()->IsCollidedByPushBackTarget( *collision ) ){
                GetMainSprite().SetVisible(TB_TRUE);
                collision->SetActive(TB_TRUE);
                m_Timer = 0.0;
            }
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionBlinkBlock::NotifyEvent( TbUint32 event , void* param )
{
    switch( event ) 
    {
    case SIDE_ACTION_EVENT_SUSPEND:
        Suspend();
    }
    return 0;
}
