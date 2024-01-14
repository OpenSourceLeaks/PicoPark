/*!
 * カラーボックス
 * @author teco
 */

#include "fwd.h"
#include "side_action_color_box.h"
#include "side_action/side_action_types.h"
#include <crayon/map/cr_action_map.h>
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"
#include "common/cmn_util.h"

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
static TbFloat32 VELO_X = 1.0f;

// コンストラクタ
SideActionColorBox::SideActionColorBox( const TbRectF32& rect , TbSint32 playerIndex, TbSint32 exceptPlayerIndex )
    : m_PlayerIndex(0)
    , m_IsEnableJump(TB_FALSE)
    , m_JumpCount(0)
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    m_PlayerIndex = playerIndex%playerCount;
    if( 0 <= exceptPlayerIndex )
    {
        exceptPlayerIndex = (exceptPlayerIndex % playerCount);
        if (m_PlayerIndex == exceptPlayerIndex) {
            m_PlayerIndex = (m_PlayerIndex+1)% playerCount;
        }
    }
    initialize(rect);
    SetEnableNotifyJumpRecursive();
    SetWeight(WEIGHT_MAX);
}

// コンストラクタ
SideActionColorBox::SideActionColorBox(const TbRectF32& rect, TbSint32 playerIndex)
    : m_PlayerIndex(playerIndex)
    , m_IsEnableJump(TB_FALSE)
    , m_JumpCount(0) 
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
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
        CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionColorBox::OnUpdateAction( TbFloat32 deltatime )
{
    if (m_BitArray.Test(FLAG_CATCHED)) {
        // 掴まれている
        return;
    }

    // 接しているオブジェクトが押してきていたら
    const CrActionCollision* collision = GetCollision();

    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetActionStage());
    CrActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    // 左右への移動
    TbFloat32 veloX = 0.0f;
    SetVeloX(0.0f);
    if(!m_IsEnableJump)
    {
        if( isPushed(CR_CONTACT_LEFT) ) {
            SetVeloX(VELO_X);
        }else if( isPushed(CR_CONTACT_RIGHT) ) {
            SetVeloX(-VELO_X);
        }
    }

    // ジャンプ処理
    if (GetVelo().GetY() >= 0.0f) {
        m_JumpCount = 0;
    }
    if (isEnableJump())
    {
        SideActionPlayer* player = stage->GetPlayer(m_PlayerIndex);
        if (CrInput::IsPress(CrInput::BUTTON_JUMP, m_PlayerIndex)) {
            TbPlaySound("jump");
            SetVeloY(player->GetJumpVelo());
            m_JumpCount = 1;
        }
        else if (0 < m_JumpCount && m_JumpCount < SIDE_ACTION_PLAYER_JUMP_COUNT_MAX && CrInput::IsOn(CrInput::BUTTON_JUMP, m_PlayerIndex)) {
            TbFloat32 scale = (1 - static_cast<TbFloat32>(m_JumpCount) / static_cast<TbFloat32>(SIDE_ACTION_PLAYER_JUMP_COUNT_MAX))*SIDE_ACTION_PLAYER_JUMP_VELO_COEF;
            AddVelo(0.0f, player->GetJumpVelo()*scale);
            ++m_JumpCount;
        }
        else {
            m_JumpCount = 0;
        }

        // 超暫定(BallBoxがない場合は消す)
        if ( CrActor* actor = stage->FindActor("BallBox")) {
            if (!actor->IsEntered()) {
                TbFloat32 alpha = m_Sprite.GetSprite().GetAlpha();
                alpha -= deltatime;
                if(alpha <= 0.0f ){
                    Exit();
                }else {
                    m_Sprite.GetSprite().SetAlpha(alpha);
                }
            }
        }
    }

    if (collision->IsContacted(CR_CONTACT_UP)) {
// 少しくっついてジャンプしやすいので速度を無効化はしない
//        SetVeloY(TbMax(GetVelo().GetY(), 0.0f));
    }
    else if (!TB_FLOAT32_EQUAL_ZERO(m_ForceJumpVelo.GetX())) {
        // 強制ジャンプ中はXを動かす
        AddVelo(m_ForceJumpVelo.GetX(), 0.0f);
    }

    // 自由落下
    if( !collision->IsContacted(CR_CONTACT_DOWN) ) {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else {
        if (1 == m_JumpCount) {
            SetVeloY(TbMin(GetVelo().GetY(), 0.0f));
        }else {
            SetVeloY(0.0f);
        }
        if (!m_BitArray.Test(FLAG_FORCE_JUMP)) {
            m_ForceJumpVelo.Clear();
        }
    }

    if (m_BitArray.Test(FLAG_FORCE_JUMP)) {
        // 強制ジャンプ
        AddVelo(0.0f, m_ForceJumpVelo.GetY());
        m_BitArray.ResetBit(FLAG_FORCE_JUMP);
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
    CrActionCollision* collision = GetCollision();
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
        SetPosForce( GetInitPos() );
    }else if (SIDE_ACTION_EVENT_GET_PLAYERINDEX == event) {
        return m_PlayerIndex+1;
    }
    else if (SIDE_ACTION_EVENT_WARP == event) {
        // 瞬間移動
        if (param) {
            m_RequestedPos = *reinterpret_cast<TbVector2*>(param);
        }
    }
    else if (SIDE_ACTION_EVENT_JUMP == event || 
             SIDE_ACTION_EVENT_JUMP_OBJECT == event )
    {
        if (param) {
            m_ForceJumpVelo = (*reinterpret_cast<TbVector2*>(param));
            if (GetVelo().GetX() < 0.0f) {
                m_ForceJumpVelo.SetX(TbAbs(m_ForceJumpVelo.GetX())*-1.0f);
            }
            else {
                m_ForceJumpVelo.SetX(TbAbs(m_ForceJumpVelo.GetX()));
            }
            m_BitArray.SetBit(FLAG_FORCE_JUMP);
        }
    }
    else if (SIDE_ACTION_EVENT_SET_VISIBLE == event)
    {
        if (param) {
            TbBool* visible = reinterpret_cast<TbBool*>(param);
            setVisible(*visible);
        }
    }
    else if (SIDE_ACTION_EVENT_CATCHED == event) {
        if (param)
        {
            TbSint32 isOn = *reinterpret_cast<TbSint32*>(param);
            m_BitArray.SetBit(FLAG_CATCHED, isOn);
            GetCollision()->SetUserDataU32(SIDE_ACTION_COLLISION_U32_DISABLE_INERTIA, isOn);
            SetVelo(TbVector2());
            return 1;
        }
    }
    return 0;
}

/*!
 * 表示設定
 * @author teco
 */
void SideActionColorBox::setVisible(TbBool isVisible)
{
    m_Sprite.GetSprite().SetVisible(isVisible);
    GetCollision()->SetActive(isVisible);
    if (isVisible) {
        Resume();
    } else {
        Suspend();
    }
}

/*!
 * 任意の方向から押されているか
 * @author teco
 */
TbBool SideActionColorBox::isPushed( CrContactFace type )
{
    return isPushsdByColor( this , type );
}

/*!
 * 押している数
 * @author teco
 */
TbUint32 SideActionColorBox::isPushsdByColor( CrActionActor* actor , CrContactFace type )
{
    const CrActionCollision* collision = actor->GetCollision();
    if( !collision ) {
        return TB_FALSE;
    }
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , type );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        CrActionActor* next = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        TbBool pushed = TB_FALSE;
        switch( type ) {
        case CR_CONTACT_UP:
            if( next->GetVelo().GetY() > TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_DOWN:
            if( next->GetVelo().GetY() < TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_LEFT:
            if( next->GetVelo().GetX() > TB_FLOAT32_EPSILON ){
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_RIGHT:
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

// ジャンプ可能か
TbBool SideActionColorBox::isEnableJump() const
{
    if (!m_IsEnableJump) {
        return TB_FALSE;
    }
    const CrActionCollision* collision = GetCollision();
    TB_RESQUE_RET(collision, TB_FALSE);

    TbBool result = TB_FALSE;
    if (!collision->IsContacted(CR_CONTACT_UP) && collision->IsContacted(CR_CONTACT_DOWN)) {
        result = TB_TRUE;
    }
    if (m_JumpCount > 0 && m_JumpCount < SIDE_ACTION_PLAYER_JUMP_COUNT_MAX) {
        result = TB_TRUE;
    }
    return result;
}
