/*!
 * 鍵箱
 * @author teco
 */

#include "fwd.h"
#include "side_action_key_box.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static TbFloat32 RADIUS = 12.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -24.0f , -64.0f , 48.0f , 64.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(208,640,24,32),
};

static const TbRectF32Pod COLLISION_RECT = 
{
    -22.0f , -62.0f , 44.0f , 60.0f
};

static const TbRectF32Pod TRIGGER_COLLISION_RECT = 
{
    -4.0f , -66.0f , 8.0f , 10.0f
};

static const TbFloat32 VELO_X = 1.0f;

static TB_NDEBUG_CONST TbUint32  DEAD_COUNT = 40;
static TB_NDEBUG_CONST TbUint32  ANIM_COUNT = 10;
static TB_NDEBUG_CONST TbUint32  ANIM_START = 30;

//////////////////////////////////////////////////
// SideActionBallKeyBox
//////////////////////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionBallKeyBox::SideActionBallKeyBox()
    : Super()
    , m_DeadTime(0)
    , m_Key(nullptr)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);

    collision = CreateCollision( TRIGGER_COLLISION_RECT , 0 , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided) );

    m_Key = new SideActionKey();
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionBallKeyBox::~SideActionBallKeyBox()
{
    TB_SAFE_RELEASE(m_Key);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionBallKeyBox::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_DeadTime > 0 ) {
        // 死亡時
        if( m_DeadTime <= ANIM_START ){
            TbSint32 animCount = m_DeadTime - (ANIM_START-ANIM_COUNT);
            if( animCount >= 0 )
            {
                // 消えるアニメーション
                TbSprite& sprite = GetMainSprite();
                TbFloat32 alpha = static_cast<TbFloat32>(animCount) / static_cast<TbFloat32>(ANIM_COUNT);
                sprite.SetAlpha(alpha);
            }
        }
        --m_DeadTime;
        if( 0 == m_DeadTime ) {
            Exit();
        }
    }else{
        // 接しているオブジェクトが押してきていたら
        const CrActionCollision* collision = GetCollision();

        CrActionStage* stage = GetActionStage();
        CrActionMap& map = stage->GetMap();
        TB_RESQUE(collision);

        // 左右への移動
        TbFloat32 veloX = 0.0f;
        SetVeloX(0.0f);
        if( isPushed(CR_CONTACT_LEFT) ) {
            SetVeloX(VELO_X);
    //        MoveContactActor(TbVector2(VELO_X,0.0f),CR_CONTACT_LEFT);

            // 上に乗っかっているやつも動かす
    //        MoveContactActor(TbVector2(VELO_X,0.0f),CR_CONTACT_UP);
        }else if( isPushed(CR_CONTACT_RIGHT) ) {
            SetVeloX(-VELO_X);
    //        MoveContactActor(TbVector2(-VELO_X,0.0f),CR_CONTACT_RIGHT);

            // 上に乗っかっているやつも動かす
    //        MoveContactActor(TbVector2(-VELO_X,0.0f),CR_CONTACT_UP);
        }

        // 自由落下
        if( !collision->IsContacted(CR_CONTACT_DOWN) ) {
            AddVelo(0.0f,SIDE_ACTION_GRAVITY);
        }else{
            SetVeloY(0.0f);
        }
    }
}

/*!
 * 接触開始コールバック
 * @author teco
 */
void SideActionBallKeyBox::onCollided(CrActionCollision* collision)
{
    if( !collision ){
        return;
    }
    TbFloat32 srcX = collision->GetPos().GetX();
    TbFloat32 x = GetPosX();
    if( !TB_FLOAT32_EQUAL_EPS(srcX,x,5.0f) ){
        return;
    }

    TbSint32 result = collision->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_KEYBOX_ON,&x);
    if( result > 0 ){
        m_DeadTime = DEAD_COUNT;
        ResetVelo();

        if( m_Key ){
            CrActionStage* stage = GetActionStage();
            TbActorScene* scene = &stage->GetActorScene();
            m_Key->SetStage(stage);
            TbVector2 pos(GetPosX(),GetPosY()-30.0f);
            m_Key->SetInitPos( pos );
            m_Key->Enter( scene , stage );
        }
    }
}

/*!
 * 任意の方向から押されているか
 * @author Miyake_Shunske
 */ 
TbBool SideActionBallKeyBox::isPushed( CrContactFace type )
{
#if 0
    const CrActionCollision* collision = GetCollision();
    if( !collision ) {
        return TB_FALSE;
    }
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , type );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        CrActionActor* next = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        switch( type ) {
        case CR_CONTACT_UP:
            if( next->GetVelo().GetY() > TB_FLOAT32_EPSILON ){
                return TB_TRUE;
            }
            break;
        case CR_CONTACT_DOWN:
            if( next->GetVelo().GetY() < TB_FLOAT32_EPSILON ){
                return TB_TRUE;
            }
            break;
        case CR_CONTACT_LEFT:
            if( next->GetVelo().GetX() > TB_FLOAT32_EPSILON ){
                return TB_TRUE;
            }
            break;
        case CR_CONTACT_RIGHT:
            if( next->GetVelo().GetX() < TB_FLOAT32_EPSILON ){
                return TB_TRUE;
            }
            break;
        }
    }
#endif
    return TB_FALSE;
}

//////////////////////////////////////////////////
// SideActionLaserKeyBox
//////////////////////////////////////////////////

static const TbRectF32Pod LASER_BOX_SPRITE_RECT = 
{
    -23.0f , -62.0f , 46.0f , 62.0f
};

static const TbRectF32Pod LASER_BOX_SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(288,480,23,31),
    CMN_TEX_UV_RECT(320,480,23,31),
    CMN_TEX_UV_RECT(352,480,23,31),
};

static const TbSizeT CRASH_COUNT = TB_ARRAY_COUNT_OF(LASER_BOX_SPRITE_UV_RECT);

/*!
 * コンストラクタ
 * @author Miyake_Shunske
 */ 
SideActionLaserKeyBox::SideActionLaserKeyBox()
    : m_DeadTime(0)
    , m_Count(0)
    , m_Key(nullptr)
    , m_LevelUpValue(1.0f)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    sprite.Initialize(LASER_BOX_SPRITE_RECT,LASER_BOX_SPRITE_UV_RECT[0]);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
                              CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetContactedCallback( TbCreateMemFunc(this,&Self::onContacted) );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);

    m_Key = new SideActionKey();

}

/*!
 * デストラクタ
 * @author Miyake_Shunske
 */ 
SideActionLaserKeyBox::~SideActionLaserKeyBox()
{
    TB_SAFE_RELEASE(m_Key);
}

/*!
 * ロジック更新
 * @author Miyake_Shunske
 */ 
void SideActionLaserKeyBox::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_DeadTime > 0 ) {
        // 死亡時
        if( m_DeadTime <= ANIM_START ){
            TbSint32 animCount = m_DeadTime - (ANIM_START-ANIM_COUNT);
            if( animCount >= 0 )
            {
                // 消えるアニメーション
                TbSprite& sprite = GetMainSprite();
                TbFloat32 alpha = static_cast<TbFloat32>(animCount) / static_cast<TbFloat32>(ANIM_COUNT);
                sprite.SetAlpha(alpha);
            }
        }
        --m_DeadTime;
        if( 0 == m_DeadTime ) {
            Exit();
        }
    }
}

/*!
 * 接触開始コールバック
 * @author Miyake_Shunske
 */ 
void SideActionLaserKeyBox::onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision)
{
    if( target != CrActionCollision::CONTACT_TARGET_COLLISION ){
        return;
    }
    if( collision.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_OBJECT )
    {
        ++m_Count;
        if( m_Count < CRASH_COUNT ){
            GetMainSprite().SetUV(LASER_BOX_SPRITE_UV_RECT[m_Count]);
            if( !m_LevelUpTarget.IsEmpty() ) {
                GetActionStage()->NotifyEvent(m_LevelUpTarget.GetBuffer(),SIDE_ACTION_EVENT_LEVEL_UP,&m_LevelUpValue);
            }
        }
        if( CRASH_COUNT <= m_Count && m_Key ){
            m_DeadTime = DEAD_COUNT;

            CrActionStage* stage = GetActionStage();
            TbActorScene* scene = &stage->GetActorScene();
            m_Key->SetStage(stage);
            TbVector2 pos(GetPosX(),GetPosY()-30.0f);
            m_Key->SetInitPos( pos );
            m_Key->Enter( scene , stage );
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */ 
TbSint32 SideActionLaserKeyBox::NotifyEvent(TbUint32 event, void* param)
{
    switch (event)
    {
    case SIDE_ACTION_EVENT_RECOVER:
        {
            if (m_Count < CRASH_COUNT) {
                m_Count = 0;
                if (!m_LevelUpTarget.IsEmpty()) {
                    GetActionStage()->NotifyEvent(m_LevelUpTarget.GetBuffer(), SIDE_ACTION_EVENT_LEVEL_DOWN,nullptr);
                }
                GetMainSprite().SetUV(LASER_BOX_SPRITE_UV_RECT[m_Count]);
            }
        }
        break;
    }
    return 0;
}

