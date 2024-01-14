/*!
 * 幽霊
 * @author teco
 */

#include "fwd.h"
#include "side_action_ghost.h"
#include "side_action_key.h"
#include "side_action/side_action_types.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbFloat32 OFFSET_Y = -100.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -25.0f , OFFSET_Y , 50.0f , 58.0f
};
static const TbRectF32Pod SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(464,288,25,29) , // 平気
    CMN_TEX_UV_RECT(528,288,25,29) , // 恥ずかしい(小)
    CMN_TEX_UV_RECT(560,288,25,29) , // 恥ずかしい(中)
    CMN_TEX_UV_RECT(592,288,25,29) , // 恥ずかしい(大)
};

static const TbRectF32Pod SPRITE_UV_RECT_BLIND =
{
    CMN_TEX_UV_RECT(464,384,25,29) // 目隠し
};

static const TbRectF32Pod COLLISION_RECT =
{
    SPRITE_RECT.x+4.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-8.0f , SPRITE_RECT.height-2.0f 
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionGhost::SideActionGhost()
    : m_LookingPlayerCount(0)
    , m_BorderRate(0.5f)
    , m_Sec(0.0f)
    , m_KeyRange(100.0f)
    , m_Speed(1.3f)
    , m_Alpha(1.0f)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT, SPRITE_UV_RECT_BLIND);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    TbUint32 pushBackTarget = 0;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_KEY_TARGET);

    collision = CreateCollision(COLLISION_RECT, 0, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
    collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionGhost::~SideActionGhost()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionGhost::Setup(const CrActorFactory::UserData& userData)
{
    enum 
    {
        ARG_BORDER ,
        ARG_LOOK_LEFT ,
        ARG_SPEED , 
        ARG_KEY_RANGE 
    };
    m_BorderRate = userData.AsFloat32(ARG_BORDER);
    if (0 < userData.AsSint32(ARG_LOOK_LEFT)) {
        LookLeft();
    }
    if (ARG_SPEED < userData.GetCount() ) {
        m_Speed = userData.AsFloat32(ARG_SPEED);
    }
    if(ARG_KEY_RANGE < userData.GetCount()) {
        m_KeyRange = userData.AsFloat32(ARG_KEY_RANGE);
    }
}

/*!
 * スイッチターゲット設定
 * @author Miyake teco
 */
void SideActionGhost::SetSwitchTarget( const char* targetName )
{
}

/*!
 * ロジック更新
 * @author Miyake teco
 */
void SideActionGhost::OnUpdateAction( TbFloat32 deltaSec )
{
    static const TbFloat32 AMPTITUDE_SPEED = 2.0f;
    static const TbFloat32 AMPTITUDE = 8.0f;
    if( m_BitArray.Test(FLAG_END) ) {
        return;
    }
    m_Sec += deltaSec;
    TbFloat32 rad = m_Sec*AMPTITUDE_SPEED;
    TbFloat32 y = TbSin(rad)*AMPTITUDE;
    if( TB_PI_RAD*2.0f <= rad ) {
        m_Sec = (rad-TB_PI_RAD*2.0f)/AMPTITUDE_SPEED;
    }
    SetOffsetDrawPos(TbVector2(0.0f,y));
    SetVelo(TbVector2());
    const SideActionPlayer* nearPlayer = nullptr;
    TbUint32 activePlayerCount = 0;
    m_LookingPlayerCount = getLookingPlayerCount(&nearPlayer,&activePlayerCount);
    TbUint32 border = TbMax<TbUint32>(1,static_cast<TbFloat32>(activePlayerCount)*m_BorderRate);

    // 切り替えチェック
    if( m_BitArray.Test(FLAG_ACTIVE) )
    {
        if(border <= m_LookingPlayerCount  ){
            setActive(TB_FALSE);
        }else {
            TbUint32 index = TbMin<TbUint32>(m_LookingPlayerCount,TB_ARRAY_COUNT_OF(SPRITE_UV_RECT)-1);
            GetMainSprite().SetUV(SPRITE_UV_RECT[index]);
        }
    }
    else
    {
        if( m_LookingPlayerCount == 0 ){
            setActive(TB_TRUE);
        }
    }

    if( m_BitArray.Test(FLAG_DEAD) )
    {
        m_Alpha -= 0.1f;
        if( 0.0f <= m_Alpha ) {
            setAlpha(m_Alpha);
        }else{
            Exit();
        }
    }
    else if (m_BitArray.Test(FLAG_ACTIVE))
    {
        if (nearPlayer )
        {
            TbVector2 targetPos;
            const SideActionKey* nearKey = getNearKey();
            if (nearKey) {
                targetPos = nearKey->GetPosXY();
            }else{
                targetPos = nearPlayer->GetPosXY();
            }
            // 移動
            TbVector2 delta = targetPos -(GetPosXY()+TbVector2(0.0f,OFFSET_Y/2.0f));
            TbFloat32 speed = TbMin(delta.Length(),m_Speed);
            delta.Normalize();
            SetVelo(delta*speed);
            if (0.0f < delta.GetX()) {
                LookRight();
            }else if (delta.GetX() < 0.0f) {
                LookLeft();
            }
        }

        // プレイヤーが一度でもキーアタッチしたらキーターゲットにはしない
        {
            const SideActionStage* stage = TbDynamicCast<const SideActionStage*>(GetStage());
            const SideActionKey* key = TbDynamicCast<const SideActionKey*>(stage->FindActor("Key"));
            if (key && key->IsPlayerAttached())
            {
                GetCollision(0)->SetActive(TB_FALSE);
            }
        }
    }

}

/*!
 * 自分を見ているプレイヤー数取得
 * @author teco
 */
TbUint32 SideActionGhost::getLookingPlayerCount(const SideActionPlayer** nearPlayer, TbUint32* activePlayerCount) const
{
    TbUint32 result = 0;
    TbVector4 pos = GetPos();
    const SideActionStage* stage = TbDynamicCast<const SideActionStage*>(GetStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    (*activePlayerCount) = 0;
    TbFloat32 distanceMax = TB_FLOAT32_MAX;
    for( TbUint32 i = 0; i < playerCount; ++i ) {
        const SideActionPlayer* player = stage->GetPlayer(i);
        if (player->IsCleared()) {
            continue;
        }
        if (!player->IsAlive()) {
            continue;
        }
        ++(*activePlayerCount);
        TbVector4 delta =  player->GetPos() - pos;
        TbFloat32 distance = delta.Length();
        if (distance < distanceMax) {
            distanceMax = distance;
            if (nearPlayer) {
                *nearPlayer = player;
            }
        }
        if( 0.0f <= delta.GetX() ) {
            if( !player->IsLookingRight() ) {
                ++result;
            }
        }else{
            if( player->IsLookingRight() ) {
                ++result;
            }
        }
    }
    return result;
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionGhost::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        {
            m_BitArray.SetBit(FLAG_SWITCH);
        }
        break;
    case SIDE_ACTION_EVENT_KEY_END:
        {
            m_BitArray.SetBit(FLAG_DEAD);
        }
        break;
    }
    return 0;
}

/*!
 * 有効設定
 * @author teco
 */
void SideActionGhost::setActive( TbBool isActive )
{
    if( TB_BOOL_NOT_EQUAL(m_BitArray.Test(FLAG_ACTIVE),isActive) ) {
        m_BitArray.SetBit(FLAG_ACTIVE,isActive);
        if( isActive ) {
            GetMainSprite().SetUV(SPRITE_UV_RECT[0]);
        }else{
            GetMainSprite().SetUV(SPRITE_UV_RECT_BLIND);
        }
    }
}

/*!
 * 透明度設定
 * @author teco
 */
void SideActionGhost::setAlpha( TbFloat32 alpha )
{
    GetMainSprite().SetAlpha(alpha);
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 */
void SideActionGhost::onCollided(CrActionCollision* target)
{
    if (target->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER)
    {
        // 死亡通知
        GetStage()->RequestScreenQuake();
        TbPlaySound("hit");
        target->GetActor()->NotifyEvent(SIDE_ACTION_EVENT_DEAD, nullptr);
    }
}

/*!
 * 近くにあるキーを探す
 * @author teco
 */
const SideActionKey*  SideActionGhost::getNearKey()
{
    const SideActionStage* stage = TbDynamicCast<const SideActionStage*>(GetStage());
    const SideActionKey* key = TbDynamicCast<const SideActionKey*>(stage->FindActor("Key"));
    if (key && !key->IsPlayerAttached())
    {
        TbVector4 pos = GetPos();
        TbVector4 delta = key->GetPos() - pos;
        if ( TbAbs(delta.GetX()) < m_KeyRange)
        {
            return key;
        }
    }
    return nullptr;
}
