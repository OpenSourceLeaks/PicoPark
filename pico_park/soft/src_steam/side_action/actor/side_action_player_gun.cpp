/*!
 * プレイヤー銃
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_player_gun.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "common/map/action_map.h"
#include "root.h"


//////////////////////////////////////////
// SideActionPlayerWarpGun
//////////////////////////////////////////

static enum GunType
{
    GUN_TYPE_WARP , 
    GUN_TYPE_MAX
};

static TbFloat32 GUN_WIDTH = 12.0f;
static TbFloat32 GUN_HEIGHT = 7.0f;

static const TbRectF32Pod GUN_SPRITE_RECT[] = 
{
    {-GUN_WIDTH , -GUN_HEIGHT *2 , GUN_WIDTH*2.0f , GUN_HEIGHT*2.0f} ,
};

static const TbRectF32Pod GUN_SPRITE_UV_RECT[] =
{
    CMN_TEX_UV_RECT(224,496,GUN_WIDTH,GUN_HEIGHT),
};

static TB_NDEBUG_CONST TbVector2 OFFSET_POS[] =
{
    TbVector2(20.0f,14.0f) , 
};

static TB_NDEBUG_CONST TbFloat32  DEAD_SEC = 0.5f;

/////////////////////////////
// SideActionPlayerWarpGun
/////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerWarpGun::SideActionPlayerWarpGun( SideActionPlayer* player )
    : Super()
    , m_Player(player)
    , m_Target(nullptr)
    , m_Bullet(nullptr)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    sprite.Initialize(GUN_SPRITE_RECT[GUN_TYPE_WARP], GUN_SPRITE_UV_RECT[GUN_TYPE_WARP]);
    sprite.SetMaterial(&m_Material);
    m_Material.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));
    m_Player->Retain();
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerWarpGun::~SideActionPlayerWarpGun()
{
    TB_SAFE_RELEASE(m_Player);
    TB_SAFE_RELEASE(m_Bullet);
}

/*!
 * ターゲットプレイヤー設定
 * @author teco
 */
void SideActionPlayerWarpGun::SetTarget( ActionActor* actor )
{
    m_Target = actor;
    TbSint32 playerIndex = -1;
    if(actor){
        playerIndex = actor->NotifyEvent(SIDE_ACTION_EVENT_GET_PLAYERINDEX,nullptr)-1;
    }
    if (0 <= playerIndex) 
    {
        m_Material.SetColor(CmnUtil::GetPlayerColor(playerIndex));
    }
    else 
    {
        m_Material.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerWarpGun::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_Player->GetPlayerIndex();
    }
    return 0;
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionPlayerWarpGun::OnUpdateAction(TbFloat32 deltatime)
{
    if (m_Bullet)
    {
        if (!m_Bullet->IsEntered()) {
            TB_SAFE_RELEASE(m_Bullet);
        }
    }
    else
    {
        static TB_NDEBUG_CONST TbFloat32 SPEED = 6.0f;
        if (m_Player->IsPressInput(SysInput::BUTTON_SPECIAL)) {
            TbVector2 initPos( GetPos().GetX() , GetPos().GetY() );
            TbVector2 velo(SPEED, 0.0f );
            TbVector2 offset(10.0f,-7.0f);
            if (!m_Player->IsLookingRight()) {
                velo.SetX(-velo.GetX());
                offset.SetX(-offset.GetX());
            }
            initPos += offset;
            ActionStage* stage = GetActionStage();
            TbActorScene* scene = &stage->GetActorScene();
            SideActionPlayerWarpGunBullet* bullet = new SideActionPlayerWarpGunBullet(this);
            bullet->SetVelo(velo);
            bullet->SetStage(stage);
            bullet->SetInitPos(initPos);
            if(!stage->GetCollisionScene().IsCollidedByPushBackTarget(*bullet->GetCollision())){
                bullet->Enter(scene, stage);
                m_Bullet = bullet;
            }
            else {
                bullet->Release();
            }
        }
    }
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlayerWarpGun::OnPostAction( TbFloat32 deltatime )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if( !TB_VERIFY(stage) ) {
        return;
    }

    GetMainSprite().SetVisible( m_Player->IsVisible() );
    TbVector4 pos = m_Player->GetPos();

    // スプライト反映
    TbSprite& sprite = GetMainSprite();
    if( m_Player->IsLookingRight() ){
        pos.Add(OFFSET_POS[GUN_TYPE_WARP].GetX(), OFFSET_POS[GUN_TYPE_WARP].GetY(), 0.0f);
        sprite.SetScale(1.0f,1.0f);
    }else{
        pos.Add(-OFFSET_POS[GUN_TYPE_WARP].GetX(), OFFSET_POS[GUN_TYPE_WARP].GetY(), 0.0f);
        sprite.SetScale(-1.0f,1.0f);
    }
    SetPos(pos);
    sprite.SetTranslate( GetDrawPos() );
}

/////////////////////////////
// SideActionPlayerWarpGunBullet
/////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerWarpGunBullet::SideActionPlayerWarpGunBullet(SideActionPlayerWarpGun* gun )
    : m_Gun(gun)
    , m_DeadSec(0.0f)
{
    static const TbRectF32Pod SPRITE_RECT =
    {
        -7 , -7 , 14 , 14
    };

    static const TbRectF32Pod SPRITE_UV_RECT =
    {
        CMN_TEX_UV_RECT(240,496,7,7),
    };

    static const TbRectF32Pod COLLISION_RECT =
    {
        -5 , -5 , 10 , 10
    };

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_3);
    sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT);
    sprite.SetMaterial(&m_Material);
    TbSint32 playerIndex = gun->GetTargetPlayerIndex();
    if (playerIndex < 0) {
        playerIndex = gun->GetPlayerIndex();
    }
    m_Material.SetColor(CmnUtil::GetPlayerColor(playerIndex));
    m_Gun->Retain();

    ActionCollision* collision = CreateCollision(COLLISION_RECT,ActionCollision::PUSH_BACK_TARGET_ALL, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetContactedCallback(TbCreateMemFunc(this, &Self::onContacted));

}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerWarpGunBullet::~SideActionPlayerWarpGunBullet()
{
    m_Gun->Release();
}

/*!
 * 定期処理
 * @author teco
 */
void SideActionPlayerWarpGunBullet::OnUpdateAction(TbFloat32 deltatime)
{
    static TB_NDEBUG_CONST TbFloat32  SCALE = 1.01f;
    if (0.0f < m_DeadSec) {
        m_DeadSec -= deltatime;

        TbSprite& sprite = GetMainSprite();

        // 死亡時
        // 消えるアニメーション
        TbVector2 scale = sprite.GetScale();
        scale *= SCALE;
        sprite.SetScale(scale);

        TbFloat32 alpha = TbMax(0.0f,m_DeadSec / DEAD_SEC);
        sprite.SetAlpha(alpha);
        
        if (m_DeadSec <= 0.0f) {
            Exit();
        }
    }
    else 
    {
        TbVector2 pos = GetDrawPos();
        if (pos.GetX() < 0.0f || GetStage()->GetWindowWidth() <= pos.GetX()) {
            Exit();
        }
    }
}

/*!
 * 接触開始コールバック
 * @author teco
 */
void SideActionPlayerWarpGunBullet::onContacted(const TbVector2& normal, ActionCollision::ContactTarget target, const ActionCollisionInfo& collision)
{
    m_DeadSec = DEAD_SEC;
    ResetVelo();
    GetCollision()->SetActive(TB_FALSE);
    if (m_Gun->GetTarget())
    {
        // ワープ
        SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
        ActionCollisionScene& collisionScene = stage->GetCollisionScene();
        ActionActor* target = m_Gun->GetTarget();
        ActionCollision* targetCollision = target->GetCollision();
        TbRectF32 targetRect = targetCollision->GetRect();
        TbVector2 targetPos = GetPosXY();
        if (0.0f < normal.GetX())
        {
            targetPos.Add(-targetRect.width-targetRect.x-1.0f, 0.0f);
        }
        else
        {
            targetPos.Add(-targetRect.x+1.0f, 0.0f);
        }
        targetPos.Add(0.0f,-targetRect.height-targetRect.y-1.0f);
        TbVector2 backup = targetCollision->GetPos();
        targetCollision->SetPos(targetPos);
        if (collisionScene.IsCollidedByPushBackTarget(*targetCollision, ActionCollision::PUSH_BACK_TARGET_MAP)) {
            targetPos.Add(0.0f, targetRect.height+2.0f);
        }
        targetCollision->SetPos(backup);
        target->NotifyEvent(SIDE_ACTION_EVENT_WARP,&targetPos);
        m_Gun->ResetTarget();
    }
    else
    {
        ActionActor* targetActor = nullptr;
        if (collision.collision)
        {
            targetActor = collision.collision->GetActor();
            TbSint32 playerIndex = targetActor->NotifyEvent(SIDE_ACTION_EVENT_GET_PLAYERINDEX, nullptr) - 1;
            if (playerIndex < 0) {
                targetActor = nullptr;
            }
        }
        m_Gun->SetTarget(targetActor);
    }
}
