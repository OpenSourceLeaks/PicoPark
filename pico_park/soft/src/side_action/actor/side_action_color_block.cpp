/*!
* カラーボックス(引っ張りあることを想定)
* 没候補
* @author teco
*/

#include "fwd.h"
#include "side_action_color_block.h"
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
    { CMN_TEX_UV_RECT(144 + 0  ,640 + 32 + 8,8,8) } , // PLACE_LB

    { CMN_TEX_UV_RECT(144 + 16 ,640     ,8,8) } , // PLACE_UP
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 16,8,8) } , // PLACE_IN
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 32 + 8,8,8) } , // PLACE_BT

    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640     ,8,8) } , // PLACE_RU
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 16,8,8) } , // PLACE_RC
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 32 + 8,8,8) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT) == TbElasticSprite::PLACE_MAX);
static TbFloat32 VELO_X = 2.0f;

// コンストラクタ
SideActionColorBlock::SideActionColorBlock(const TbRectF32& rect, TbSint32 playerIndex, TbSint32 exceptPlayerIndex)
    : m_PlayerIndex(0)
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    if (0 <= exceptPlayerIndex) {
        exceptPlayerIndex = exceptPlayerIndex % playerCount;
    }
    m_PlayerIndex = playerIndex % playerCount;
    if (m_PlayerIndex == exceptPlayerIndex) {
        m_PlayerIndex = (m_PlayerIndex + 1) % playerCount;
    }
    SetEnableNotifyJumpRecursive();
    initialize(rect);
}

/*!
* デストラクタ
* @author teco
*/
SideActionColorBlock::~SideActionColorBlock()
{
}

/*!
* 初期化
* @author teco
*/
void SideActionColorBlock::initialize(const TbRectF32& rect)
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
    collisionRect.x += MARGIN / 2.0f;
    collisionRect.y += MARGIN / 2.0f;
    collisionRect.width -= MARGIN;
    collisionRect.height -= MARGIN;
    TbUint32 pushBackTarget =   CrActionCollision::PUSH_BACK_TARGET_MAP |
                                CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
}

/*!
* ロジック更新
* @author teco
* @since 2013.09.19
*/
void SideActionColorBlock::OnUpdateAction(TbFloat32 deltatime)
{
    // 接しているオブジェクトが押してきていたら
    const CrActionCollision* collision = GetCollision();

    CrActionStage* stage = GetActionStage();
    CrActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    // 左右への移動
    TbFloat32 veloX = 0.0f;
    SetVeloX(0.0f);
    if (isPushed(CR_CONTACT_LEFT)) {
        SetVeloX(VELO_X);
    }
    else if (isPushed(CR_CONTACT_RIGHT)) {
        SetVeloX(-VELO_X);
    }

    // 自由落下
    if (!collision->IsContacted(CR_CONTACT_DOWN)) {
        AddVelo(0.0f, SIDE_ACTION_GRAVITY);
    }
    else {
        SetVeloY(0.0f);
    }
}


/*!
* イベント通知
* @author teco
*/
TbSint32 SideActionColorBlock::NotifyEvent(TbUint32 event, void* param)
{
    if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        SetPos(GetInitPos());
    }else if (SIDE_ACTION_EVENT_GET_PLAYERINDEX == event) {
        return m_PlayerIndex+1;
    }
    return 0;
}


/*!
* 任意の方向から押されているか
* @author teco
*/
TbBool SideActionColorBlock::isPushed(CrContactFace type)
{
    return isPushsdByColor(this, type);
}

/*!
* 押している数
* @author teco
*/
TbUint32 SideActionColorBlock::isPushsdByColor(CrActionActor* actor, CrContactFace type)
{
    const CrActionCollision* collision = actor->GetCollision();
    if (!collision) {
        return TB_FALSE;
    }
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision(contactCollision, type);
    for (TbUint32 i = 0; i < contactCount; ++i) {
        CrActionActor* next = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        TbBool pushed = TB_FALSE;
        switch (type) {
        case CR_CONTACT_UP:
            if (next->GetVelo().GetY() > TB_FLOAT32_EPSILON) {
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_DOWN:
            if (next->GetVelo().GetY() < TB_FLOAT32_EPSILON) {
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_LEFT:
            if (next->GetVelo().GetX() > TB_FLOAT32_EPSILON) {
                pushed = TB_TRUE;
            }
            break;
        case CR_CONTACT_RIGHT:
            if (next->GetVelo().GetX() < TB_FLOAT32_EPSILON) {
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


static TbFloat32 K = 0.2f; // ばね定数的な意味なもの

/*!
* コンストラクタ
* @author teco
*/
SideActionColorBlockConstraint::SideActionColorBlockConstraint()
    : m_Distance(0.0f)
{
}

/*!
* デストラクタ
* @author teco
*/
SideActionColorBlockConstraint::~SideActionColorBlockConstraint()
{
}

/*!
* ロジック更新
* @author teco
*/
void SideActionColorBlockConstraint::OnUpdateAction(TbFloat32 deltatime)
{
    TbUint32 blockCount = m_BlockList.GetCount();
    CalcInfo info[BLOCK_MAX];
    for (TbUint32 i = 0; i < blockCount; ++i) {
        info[i].block = m_BlockList[i];
        info[i].prevTension = calcPrevTension(i);
        info[i].nextTension = calcNextTension(i);
    }

    for (TbUint32 i = 0; i < blockCount; ++i) {
        CalcInfo* prev = 0 < i ? &info[i - 1] : nullptr;
        CalcInfo* next = i < blockCount - 1 ? &info[i + 1] : nullptr;
        calcF(&info[i], prev, next);
    }

    for (TbUint32 i = 0; i < blockCount; ++i) {
        info[i].block->AddPos(info[i].f); // そのまま位置補正に使ってみる
        info[i].block->AddVelo(info[i].f);
    }
}

/*!
* 解決
* @author teco
*/
void SideActionColorBlockConstraint::calcF(CalcInfo* dst, CalcInfo* prev, CalcInfo* next)
{
    static const TbFloat32 RESULT_Y_MAX = SIDE_ACTION_GRAVITY_MAX + SIDE_ACTION_GRAVITY;
    TbVector2 result;
    if (prev) {
        result += calcF(dst->block, prev->block, dst->nextTension, prev->prevTension);
    }
    if (next) {
        result += calcF(dst->block, next->block, dst->prevTension, next->nextTension);
    }

    TbFloat32 signY = TbSign(result.GetY());
    result.SetY(signY*TbMin(TbAbs(result.GetY()), RESULT_Y_MAX));
    dst->f = result;
}

/*!
* 解決
* @author teco
*/
TbVector2 SideActionColorBlockConstraint::calcF(SideActionColorBlock* src, SideActionColorBlock* target, const TbVector2& srcTension, const TbVector2& targetTension)
{
    TbVector2 result;
    if (!src || !target) {
        return result;
    }

    TbVector4 deltaPos = target->GetPos() - src->GetPos();

    TbFloat32 length = deltaPos.Length();
    if (TB_FLOAT32_EQUAL_ZERO(length)) {
        return result;
    }

    if (length <= m_Distance) {
        return result;
    }

    TbVector2 dir(deltaPos.GetX(), deltaPos.GetY());
    dir.Normalize();
    TbFloat32 div = 1.0f;
    TbFloat32 tension1 = 1.0f + TbMax(0.0f, -dir.Dot(srcTension));
    TbFloat32 tension2 = 1.0f + TbMax(0.0f, dir.Dot(targetTension));
    div = 1.0f - (tension1 / (tension1 + tension2));

    TbFloat32 lengthF = length - m_Distance;
    TbFloat32 f = (lengthF*div)*K;
    result = dir * f;

    return result;
}

/*!
* 引っ張り計算
* @author teco
*/
TbVector2 SideActionColorBlockConstraint::calcPrevTension(TbSint32 index )
{
    TbVector2 result; 
    for (TbSint32 i = index; 0 < i; --i) {
        SideActionColorBlock* src = m_BlockList[i];
        SideActionColorBlock* pre = m_BlockList[i - 1];
        TbVector2 tension = calcTension(src, pre);
        if (TB_FLOAT32_EQUAL_ZERO(tension.LengthSq())) {
            continue;
        }
        result += tension;
    }
    return result;
}

/*!
* 引っ張り計算
* @author teco
*/
TbVector2 SideActionColorBlockConstraint::calcNextTension( TbSint32 index )
{
    TbVector2 result;
    SideActionColorBlock* main = m_BlockList[index];

    TbSint32  count = m_BlockList.GetCount();
    for (TbSint32 i = index; i < count - 2; ++i) {
        SideActionColorBlock* src = m_BlockList[i];
        SideActionColorBlock* next = m_BlockList[i + 1];
        TbVector2 tension = calcTension(src, next);
        if (TB_FLOAT32_EQUAL_ZERO(tension.LengthSq())) {
            continue;
        }
        result += tension;
    }
    return result;
}

/*!
* 引っ張り計算
* @author teco
*/
TbVector2 SideActionColorBlockConstraint::calcTension(SideActionColorBlock* src, SideActionColorBlock* target)
{
    if (!src || !target) {
        return TbVector2::ZERO;
    }

    TbVector4 deltaPos = target->GetPos() - src->GetPos();

    TbFloat32 length = deltaPos.Length();
    if (TB_FLOAT32_EQUAL_ZERO(length)) {
        return TbVector2::ZERO;
    }

    if (length <= m_Distance) {
        return TbVector2::ZERO;
    }

    // 一定距離を超えた
    TbVector2 dir(deltaPos.GetX(), deltaPos.GetY());
    dir.Normalize();
    return dir;
}

