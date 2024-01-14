/*!
 * アクション用アクター
 * @author teco
 */

#include "tb_fwd.h"
#include "crayon/actor/cr_action_actor.h"
#include "crayon/stage/cr_action_stage.h"

#include <base/util/tb_type_id.h>

using namespace toybox;

namespace crayon
{

const TbFloat32 CrActionActor::WEIGHT_MAX = 100.0f;

/*!
* コンストラクタ
* @author teco
*/
CrActionActor::CrActionActor()
    : Super()
    , m_Collision()
    , m_Weight(1.0f)
{
    m_Components.SetCapacity(5);
    m_Components.SetEnableSetCapacityAuto(TB_TRUE);
    AddComponent(&m_Sprite);
    SetEnableChangeLookDir(TB_TRUE);
    SetEnableScroll(TB_TRUE);
}

/*!
* デストラクタ
* @author teco
* @since 2013.10.06
*/
CrActionActor::~CrActionActor()
{
    TbForIterator(ite, m_Collision) {
        TB_SAFE_DELETE(ite->collision);
    }
    m_Collision.Clear();
}

/*!
* アクションステージ取得
* @author teco
* @since 2014.07.26
*/
const CrActionStage* CrActionActor::GetActionStage() const
{
    return static_cast<const CrActionStage*>(GetStage());
}

/*!
* アクションステージ取得
* @author teco
* @since 2014.07.26
*/
CrActionStage* CrActionActor::GetActionStage()
{
    return static_cast<CrActionStage*>(GetStage());
}

/*!
* コリジョン初期化
* @author teco
* @since 2013.10.06
*/
CrActionCollision* CrActionActor::CreateCollision(const TbRectF32& rect, TbUint32 pushBackTarget, TbBool isEnableSyncPos)
{
    if (m_Collision.IsFull()) {
        return NULL;
    }
    CrActionCollision* collision = new CrActionCollision();
    collision->Initialize(rect, pushBackTarget);
    setupCollision(collision, pushBackTarget, isEnableSyncPos);
    return collision;
}

/*!
* コリジョン初期化
* @author teco
* @since 2013.10.06
*/
CrActionCollision* CrActionActor::CreateCollision(const TbCircleF32& circle, TbUint32 pushBackTarget, TbBool isEnableSyncPos)
{
    if (m_Collision.IsFull()) {
        return NULL;
    }
    CrActionCollision* collision = new CrActionCollision();
    collision->Initialize(circle, pushBackTarget);
    setupCollision(collision, pushBackTarget, isEnableSyncPos);
    return collision;
}

/*!
* コリジョンセットアップ
* @author teco
* @since 2014.07.27
*/
void CrActionActor::setupCollision(CrActionCollision* collision, TbUint32 pushBackTarget, TbBool isEnableSyncPos)
{
    collision->SetActor(this);
    collision->SetLayer(CrActionCollision::LAYER_DEFAULT);
    // 位置を同期
    if (isEnableSyncPos) {
        collision->SetChangedPosCallback(TbCreateMemFunc(this, &Self::onPushedBack));
    }
    // スケールを同期
    if (isEnableSyncPos) {
        collision->SetChangedScaleCallback(TbCreateMemFunc(this, &Self::OnChangedScale));
    }
    if (GetScene()) {
        // 既にシーンに入っていたら即時にシーンに追加
        CrActionStage* actionStage = TbDynamicCast<CrActionStage*>(GetStage());
        collision->Enter(&actionStage->GetCollisionScene());
    }
    CollisionHolder holder;
    holder.collision = collision;
    holder.isEnableSyncPos = isEnableSyncPos;
    m_Collision.PushBack(holder);
}

/*!
* シーンに入った
* @author teco
* @since 2013.10.06
*/
void CrActionActor::OnEnter(void* userData)
{
    CrStage* stage = reinterpret_cast<CrStage*>(userData);
    CrActionStage* actionStage = TbDynamicCast<CrActionStage*>(stage);
    if (TB_VERIFY(actionStage)) {
        TbForIterator(ite, m_Collision) {
            ite->collision->Enter(&actionStage->GetCollisionScene());
        }
        TbForIterator(it, m_Components) {
            TbVector2 pos(GetPos().GetX(), GetPos().GetY());
            (*it)->OnEnter(actionStage, pos, 0.0f);
        }
    }
}

/*!
* シーンから出た
* @author teco
* @since 2013.10.06
*/
void CrActionActor::OnExit()
{
    TbForIterator(ite, m_Collision) {
        ite->collision->Exit();
    }
    TbForIterator(it, m_Components) {
        (*it)->OnExit();
    }
}

/*!
* 描画位置
* @author teco
* @since 2013.10.20
*/
TbVector2 CrActionActor::GetDrawPos() const
{
    TbVector2 result(GetPosX(), GetPosY());
    result += m_OffsetDrawPos;
    if (IsEnableScroll())
    {
        CrStage* stage = GetStage();
        if (stage) {
            result.Add(-stage->GetScrollX(), -stage->GetScrollY());
        }
    }
    return result;
}

/*!
* 接しているアクターを動かす
* @author teco
* @since 2014.08.06
*/
void CrActionActor::moveContactActor(const TbVector2& mov,
    const TbVector2& normal,
    ActorSet* set,
    const MoveContactOption& option)
{
    // くっついているアクターを連動して動かす
    const CrActionCollision* collision = GetCollision(option.collisionIndex);
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision(contactCollision, normal);
    for (TbUint32 i = 0; i < contactCount; ++i) {
        if (!contactCollision[i]->IsEnablePushedFromFixedPosCollision()) {
            // 強制押されをしないものは除く
            continue;
        }
        // 
        if (0 < option.targetCollisionLayerMask)
        {
            // マスクが設定されている場合は絞る
            if (!(TB_BIT(contactCollision[i]->GetLayer()) & option.targetCollisionLayerMask)) {
                continue;
            }
        }
        CrActionActor* actor = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        actor->MoveByContactActor(mov,normal,set,option);
    }

}

// 接しているところから動く
void CrActionActor::MoveByContactActor(const toybox::TbVector2& mov, const toybox::TbVector2& normal, ActorSet* set, const MoveContactOption& option)
{
#if 0 // 一旦無効
    if (!option.isEnableAttached)
    {
        if (actor->IsAttached()) {
            continue;
        }
    }
#endif

    if (set) {
        if (set->Find(this) != set->End()) {
            // 既に動かし済み
            return;
        }
        set->Insert(this);
    }

    TbVector2 newMov = mov;

    // 対象速度を無視して問題ない場合は対象速度から進行方向成分を相殺させる成分を加える
    if (option.isEnableIgnoreTargetVelo) {
        TbVector2 v = mov;
        v.Normalize();
        TbFloat32 dot = v.Dot(GetVelo());
        if (dot < 0.0f) {
            newMov += -(v * dot);
        }
    }

    AddPos(newMov);
    if (option.isEnableSyncCollisionPos) {
        SyncCollisionPos();
    }
    m_MoveByContact = newMov;
    OnMoveByContact(newMov,normal,set,option);

    // 再帰的に動かす
    if (option.isEnableRecursive)
    {
        TbVector2 face = normal;
        if (option.recursiveFaceIsMov)
        {
            face = mov;
            face.Normalize();
        }
        moveContactActor(mov, face, set, option);
    }
}

/*!
* 接しているアクターを動かすことができるか
* @author teco
* @since 2014.10.21
*/
TbBool CrActionActor::IsEnableMoveContactActor(const TbVector2& mov, CrContactFace face, TbSint32 collisionIndex)
{
    if (mov.IsZero()) {
        return TB_FALSE;
    }
    TbVector2 norm = mov;
    norm.Normalize();
    const TbVector2& faceNorm = CrUtil::GetContactFaceNormal(face);

    // 下にくっついているアクターも連動して動かす
    const CrActionCollision* collision = GetCollision(collisionIndex);
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision(contactCollision, norm);
    for (TbUint32 i = 0; i < contactCount; ++i) {
        if (!contactCollision[i]->IsEnablePushedFromFixedPosCollision()) {
            if (faceNorm.Dot(norm) >= 0.0f) {
                // 面している方向と動かしたい方向が一致してる場合は押せない
                return TB_FALSE;
            }
            else {
                continue;
            }
        }
        // マップと接触してたら動かせない
        if (contactCollision[i]->IsContactedMap(norm)) {
            return TB_FALSE;
        }
        CrActionActor* actor = static_cast<CrActionActor*>(contactCollision[i]->GetActor());
        if (!actor->IsEnableMoveContactActor(mov, face)) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
* ジャンプ通知
* @author teco
*/
void CrActionActor::NotifyJump(CrActionCollision* collision, TbFloat32 veloY , TbUint32 jumpEvent )
{
    TbUint32 recursiveMax = collision->GetContactRecursiveMax(CR_CONTACT_UP);
    veloY /= recursiveMax;
    TbVector2 velo(0.0f, veloY);

    CrActionCollision::ContactList contactList;
    TbUint32 count = collision->GetContactCount(CR_CONTACT_UP, TB_FALSE, &contactList);
    TbForIterator(it, contactList) {
        NotifyJump(*it, veloY, veloY , jumpEvent);
    }
}

/*!
* ジャンプ通知
* @author teco
*/
void CrActionActor::NotifyJump(CrActionCollision* collision, TbFloat32 veloY, TbFloat32 add, TbUint32 jumpEvent)
{
    TbVector2 velo(0.0f, veloY);
    collision->GetActor()->NotifyEvent(jumpEvent, &velo);
    CrActionCollision::ContactList contactList;
    if (collision->GetActor()->IsEnableNotifyJumpRecursive())
    {
        TbUint32 count = collision->GetContactCount(CR_CONTACT_UP, TB_FALSE, &contactList);
        TbForIterator(it, contactList) {
            collision->GetActor()->NotifyJump(*it, veloY + add, add, jumpEvent);
        }
    }

}


}
