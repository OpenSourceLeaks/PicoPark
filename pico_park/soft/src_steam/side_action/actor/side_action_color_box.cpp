/*!
 * カラーボックス
 * @author teco
 */

#include "fwd.h"
#include "side_action_color_box.h"
#include "side_action/side_action_types.h"
#include "common/map/action_map.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] = 
{
    { CMN_TEX_UV_RECT(144 + 0  ,640     ,8,8) } , // PLACE_LU
    { CMN_TEX_UV_RECT(144 + 0  ,640 + 16,8,8) } , // PLACE_LC
    { CMN_TEX_UV_RECT(144 + 0  ,640 + 32+8,8,8) } , // PLACE_LB

    { CMN_TEX_UV_RECT(144 + 16 ,640     ,8,8) } , // PLACE_UP
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 16,8,8) } , // PLACE_IN
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 32 + 8,8,8) } , // PLACE_BT

    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640     ,8,8) } , // PLACE_RU
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 16,8,8) } , // PLACE_RC
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 32 + 8,8,8) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT)==TbElasticSprite::PLACE_MAX);
static TbFloat32 VELO_X = 2.0f;

// コンストラクタ
SideActionColorBox::SideActionColorBox( const TbRectF32& rect , TbSint32 playerIndex, TbSint32 exceptPlayerIndex )
    : m_PlayerIndex(0)
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    if( 0 <= exceptPlayerIndex ){
        exceptPlayerIndex = exceptPlayerIndex % playerCount;
    }
    m_PlayerIndex = playerIndex % playerCount;
    if (m_PlayerIndex == exceptPlayerIndex) {
        m_PlayerIndex = (m_PlayerIndex+1) % playerCount;
    }
    initialize(rect);
}

// コンストラクタ
SideActionColorBox::SideActionColorBox(const TbRectF32& rect, TbSint32 playerIndex)
    : m_PlayerIndex(playerIndex)
{
    initialize(rect);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionColorBox::~SideActionColorBox()
{
}

/*!
 * 初期化
 * @author teco
 */
void SideActionColorBox::initialize(const TbRectF32& rect)
{
    m_Rect = rect;

    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(16.0f, 16.0f);
    spriteParam.rect = m_Rect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetMaterial(Root::GetInstance().GetPlayerMaterial());
    m_Sprite.GetSprite().SetColor(CmnUtil::GetPlayerColor(m_PlayerIndex));
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    // コリジョン
    TbRectF32 collisionRect = m_Rect;
    TbFloat32 MARGIN = 4.0f;
    collisionRect.x     += MARGIN/2.0f;
    collisionRect.y     += MARGIN/2.0f;
    collisionRect.width -= MARGIN;
    collisionRect.height -= MARGIN;
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
        ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionColorBox::OnUpdateAction( TbFloat32 deltatime )
{
    // 接しているオブジェクトが押してきていたら
    const ActionCollision* collision = GetCollision();

    ActionStage* stage = GetActionStage();
    ActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    // 左右への移動
    TbFloat32 veloX = 0.0f;
    SetVeloX(0.0f);
    if( isPushed(CMN_CONTACT_LEFT) ) {
        SetVeloX(VELO_X);
    }else if( isPushed(CMN_CONTACT_RIGHT) ) {
        SetVeloX(-VELO_X);
    }

    // 自由落下
    if( !collision->IsContacted(CMN_CONTACT_DOWN) ) {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else{
        SetVeloY(0.0f);
    }
}

/*!
 * 後処理
 * @author teco
 */
void SideActionColorBox::OnPostAction(TbFloat32 deltatime)
{
    // ワープ処理
    if (TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq())) {
        return;
    }
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    ActionCollision* collision = GetCollision();
    TbVector2 backup = collision->GetPos();
    collision->SetPos(m_RequestedPos);
    if (!collision->GetScene()->IsCollidedByPushBackTarget(*collision)) {
        SetPosForce(m_RequestedPos);
        ResetVelo();
        m_RequestedPos.Clear();
    }
    else {
        collision->SetPos(backup);
    }

}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionColorBox::NotifyEvent(TbUint32 event, void* param)
{
    if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        SetPos( GetInitPos() );
    }else if (SIDE_ACTION_EVENT_GET_PLAYERINDEX == event) {
        return m_PlayerIndex+1;
    }
    else if (SIDE_ACTION_EVENT_WARP == event) {
        // 瞬間移動
        if (param) {
            m_RequestedPos = *reinterpret_cast<TbVector2*>(param);
        }
    }
    return 0;
}


/*!
 * 任意の方向から押されているか
 * @author teco
 */
TbBool SideActionColorBox::isPushed( CmnContactFace type )
{
    return isPushsdByColor( this , type );
}

/*!
 * 押している数
 * @author teco
 */
TbUint32 SideActionColorBox::isPushsdByColor( ActionActor* actor , CmnContactFace type )
{
    const ActionCollision* collision = actor->GetCollision();
    if( !collision ) {
        return TB_FALSE;
    }
    ActionCollision* contactCollision[ActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , type );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        ActionActor* next = static_cast<ActionActor*>(contactCollision[i]->GetActor());
        TbBool pushed = TB_FALSE;
        switch( type ) {
        case CMN_CONTACT_UP:
            if( next->GetVelo().GetY() > TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CMN_CONTACT_DOWN:
            if( next->GetVelo().GetY() < TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CMN_CONTACT_LEFT:
            if( next->GetVelo().GetX() > TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CMN_CONTACT_RIGHT:
            if( next->GetVelo().GetX() < TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        }
        if (pushed) {
            SideActionPlayer* player = TbDynamicCast<SideActionPlayer*>(next);
            if (player && player->GetPlayerIndex() == m_PlayerIndex) {
                return TB_TRUE;
            }
        }
        if (isPushsdByColor(next, type)) {
            return TB_TRUE;
        }
    }
    return TB_FALSE;;
}

