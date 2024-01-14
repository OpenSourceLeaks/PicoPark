/*!
 * アクション用アクター
 * @author teco
 */

#ifndef _INCLUDED_CR_ACTION_ACTOR_H_
#define _INCLUDED_CR_ACTION_ACTOR_H_

#include <graphics/sprite/tb_sprite.h>
#include <base/container/tb_static_set.h>

#include <crayon/actor/cr_actor.h>
#include <crayon/actor/cr_action_actor_sprite.h>
#include <crayon/actor/cr_action_actor_physics.h>
#include <crayon/actor/cr_action_actor_component.h>
#include <crayon/collision/cr_action_collision.h>

namespace crayon
{

class CrActionStage;

class CrActionActor : public CrActor
{
public:

    static const TbFloat32 WEIGHT_MAX;
    typedef toybox::TbStaticSet<CrActionActor*, 100> ActorSet;

    struct MoveContactOption
    {
        MoveContactOption()
            : isEnableSyncCollisionPos(TB_TRUE)
            , isEnableIgnoreTargetVelo(TB_FALSE)
            , isEnableRecursive(TB_TRUE)
            , isEnableAttached(TB_FALSE)
            , recursiveFaceIsMov(TB_FALSE)
            , collisionIndex(0)
            , targetCollisionLayerMask(0)
        {}
        TbBool isEnableSyncCollisionPos;    // コリジョン座標を同期する
        TbBool isEnableIgnoreTargetVelo;    // 対象の速度無視する
        TbBool isEnableRecursive;
        TbBool isEnableAttached;
        TbBool recursiveFaceIsMov;
        TbSint32 collisionIndex;            // コリジョン番号
        TbUint32 targetCollisionLayerMask;  // 対象のレイヤーを絞る
    };

private:

    typedef CrActionActor Self;
    typedef CrActor Super;

public:

    // コンストラクタ
    CrActionActor();

    // デストラクタ
    ~CrActionActor();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // アクションステージ取得
    const CrActionStage* GetActionStage() const;

    // アクションステージ取得
    CrActionStage* GetActionStage();

    // コリジョン取得
    CrActionCollision* GetCollision(TbUint32 index = 0) {
        if (index < m_Collision.GetCount()) {
            return m_Collision[index].collision;
        }
        return NULL;
    }

    // コリジョン取得
    const CrActionCollision* GetCollision(TbUint32 index = 0) const {
        if (index < m_Collision.GetCount()) {
            return m_Collision[index].collision;
        }
        return NULL;
    }

    // メインスプライトのオフセット座標設定
    void SetMainSpriteOffsetPos(const toybox::TbVector2& pos) {
        m_Sprite.SetLocalPos(pos);
    }

    // メインスプライトのオフセット座標設定
    const toybox::TbVector2& GetMainSpriteOffsetPos() const {
        return m_Sprite.GetLocalPos();
    }

    // スプライト取得
    toybox::TbSprite& GetMainSprite() {
        return m_Sprite.GetSprite();
    }

    // スプライト取得
    const toybox::TbSprite& GetMainSprite() const {
        return m_Sprite.GetSprite();
    }

    // コンポーネント追加
    void AddComponent(CrActionActorComponent* component) {
        component->SetOwner(this);
        m_Components.PushBack(component);
    }

    // 初期位置設定
    void SetInitPos(const toybox::TbVector2& pos) {
        m_InitPos = pos;
        SetPos(pos.GetX(), pos.GetY());
        if (!m_Collision.IsEmpty()) {
            TbForIterator(ite, m_Collision) {
                if (ite->isEnableSyncPos) {
                    ite->collision->SetInitPos(pos);
                }
            }
        }
    }

    // 初期位置取得
    const toybox::TbVector2& GetInitPos() const {
        return m_InitPos;
    }

    // 前フレームの位置取得
    const toybox::TbVector4& GetPrePos() const {
        return m_PrePos;
    }

    // 強制的に座標設定
    void SetPosForce(const toybox::TbVector2& pos) {
        SetPos(pos.GetX(), pos.GetY());
        m_PrePos.SetXY(pos.GetX(), pos.GetY());
        TbForIterator(ite, m_Collision) {
            if (ite->isEnableSyncPos) {
                ite->collision->SetPosForce(toybox::TbVector2(GetPos().GetX(), GetPos().GetY()));
            }
        }
    }

    // 速度取得
    const toybox::TbVector2& GetVelo() const {
        return m_Velo;
    }

    // 前フレームの速度取得
    const toybox::TbVector2& GetPreVelo() const {
        return m_PreVelo;
    }

    // コリジョン適用速度取得
    const toybox::TbVector2& GetCollisionVelo() const {
        return m_CollisionVelo;
    }

    // 速度リセット
    void ResetVelo() {
        m_Velo.Clear();
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度設定
    void SetVelo(const toybox::TbVector2& velo) {
        m_Velo = velo;
        TB_ASSERT(!m_Velo.HasNan());
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // Y軸速度設定
    void SetVeloX(TbFloat32 x) {
        m_Velo.SetX(x);
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // Y軸速度設定
    void SetVeloY(TbFloat32 y) {
        m_Velo.SetY(y);
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度加算
    void AddVelo(TbFloat32 x, TbFloat32 y) {
        m_Velo.Add(x, y);
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度加算
    void AddVelo(const toybox::TbVector2& velo) {
        AddVelo(velo.GetX(), velo.GetY());
    }

    // 速度乗算
    void MulVelo(TbFloat32 val) {
        m_Velo *= val;
        TbForIterator(it, m_Components) {
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 加速度取得
    const toybox::TbVector2& GetAccel() const {
        return m_Accel;
    }

    // 加速度設定
    void SetAccel(const toybox::TbVector2& accel) {
        m_Accel = accel;
    }

    // Y軸加速度設定
    void SetAccelX(TbFloat32 x) {
        m_Accel.SetX(x);
    }

    // Y軸加速度設定
    void SetAccelY(TbFloat32 y) {
        m_Accel.SetY(y);
    }

    // 速度リセット
    void ResetAccel() {
        m_Accel.Clear();
    }

    // 重さ取得
    TbFloat32 GetWeight() const {
        return m_Weight;
    }

    // 重さ設定
    void SetWeight(TbFloat32 weight) {
        m_Weight = weight;
    }

    // 風設定
    void SetWind(const toybox::TbVector2& wind) {
        m_Wind = wind;
    }

    // 風設定
    void ResetWind() {
        m_Wind.Clear();
    }

    // 風設定
    const toybox::TbVector2& GetWind() const {
        return m_Wind;
    }

    // 右を向いているか
    virtual TbBool IsLookingRight() const {
        return GetMainSprite().GetScale().GetX() > 0.0f;
    }

    // 左向き
    virtual void LookLeft() {
        if(IsEnableChangeLookDir()){
            const toybox::TbVector2& scale = GetMainSprite().GetScale();
            GetMainSprite().SetScale(-toybox::TbAbs(scale.GetX()), scale.GetY());
        }
    }

    // 右向き
    virtual void LookRight() {
        if (IsEnableChangeLookDir()){
            const toybox::TbVector2& scale = GetMainSprite().GetScale();
            GetMainSprite().SetScale(toybox::TbAbs(scale.GetX()), scale.GetY());
        }
    }

    // 方向変換可能か設定
    void SetEnableChangeLookDir(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_CHANGE_DIR, isEnable);
    }

    // 方向変換可能か設定
    TbBool IsEnableChangeLookDir() const {
        return m_BitArray.Test(FLAG_ENABLE_CHANGE_DIR);
    }

    // スクロール可能か
    void SetEnableScroll(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_ENABLE_SCROLL,isEnable);
    }

    // スクロール可能か
    TbBool IsEnableScroll() const {
        return m_BitArray.Test(FLAG_ENABLE_SCROLL);
    }

    // アタッチフラグ
    TbBool IsAttached() const {
        return m_BitArray.Test(FLAG_ATTACHED);
    }

    // アタッチフラグ
    void SetAttached( TbBool isAttach ) {
        m_BitArray.SetBit(FLAG_ATTACHED, isAttach);
    }

public:

    // 描画位置
    toybox::TbVector2 GetDrawPos() const;

    // 描画補正位置設定
    void SetOffsetDrawPos(const toybox::TbVector2& pos) {
        m_OffsetDrawPos = pos;
    }

    // 描画補正位置設定
    const toybox::TbVector2& GetOffsetDrawPos() const {
        return m_OffsetDrawPos;
    }

    // 接しているアクターを動かす
    void MoveContactActor(const toybox::TbVector2& mov,
        const toybox::TbVector2& normal,
        const MoveContactOption& option = MoveContactOption())
    {
        ActorSet set;
        moveContactActor(mov, normal, &set, option);
    }

    // 接しているアクターを動かす
    void MoveContactActor(const toybox::TbVector2& mov,
        CrContactFace face,
        const MoveContactOption& option = MoveContactOption())
    {
        ActorSet set;
        moveContactActor(mov, CrUtil::GetContactFaceNormal(face), &set, option);
    }

    // MoveContactActorによって移動された移動取得
    toybox::TbVector2 GetMoveByContact() const {
        return m_MoveByContact;
    }

    // ジャンプ通知
    void NotifyJump(CrActionCollision* collision, TbFloat32 veloY , TbUint32 jumpEvent );
    void NotifyJump(CrActionCollision* collision, TbFloat32 veloY, TbFloat32 add , TbUint32 jumpEvent);

    // ジャンプ通知を再帰的に可能か
    void SetEnableNotifyJumpRecursive() {
        m_BitArray.SetBit(FLAG_ENABLE_NOTIFY_JUMP_RECURSIVE);
    }

    // ジャンプ通知を再帰的に可能か
    TbBool IsEnableNotifyJumpRecursive() {
        return m_BitArray.Test(FLAG_ENABLE_NOTIFY_JUMP_RECURSIVE);
    }

public:

    // ロジック更新
    virtual void OnUpdate(TbFloat32 deltatime) TB_OVERRIDE {
        m_OffsetDrawPos.Clear(); // 毎フレームリセット
        m_PrePos = GetPos();
        m_PreVelo = m_Velo;
        OnUpdateAction(deltatime);
        AddPos(m_Velo + (m_Accel*0.5f));
        m_Velo += m_Accel;
        // コリジョンと座標を同期
        SyncCollisionPos();
        toybox::TbVector2 pos(GetPos().GetX(), GetPos().GetY());
        toybox::TbVector2 drawPos = GetDrawPos();
        TbForIterator(it, m_Components) {
            (*it)->OnUpdatePos(pos, drawPos);
        }
    }

    // ロジック更新
    virtual void OnPost(TbFloat32 deltatime) TB_OVERRIDE {
        OnPostAction(deltatime);

        // コリジョンと座標を同期
        SyncCollisionPos();

        toybox::TbVector4 mov = GetPos() - m_PrePos;
        m_CollisionVelo.Set(mov.GetX(), mov.GetY());
        toybox::TbVector2 pos(GetPos().GetX(), GetPos().GetY());
        toybox::TbVector2 drawPos = GetDrawPos();
        TbForIterator(it, m_Components) {
            (*it)->OnUpdatePos(pos,drawPos);
        }

        m_MoveByContact.Clear();
    }

    // シーンに入った
    virtual void OnEnter(void* userData) TB_OVERRIDE;

    // シーンから出た
    virtual void OnExit() TB_OVERRIDE;

    // 処理中断
    virtual void OnSuspend() TB_OVERRIDE {
        m_Sprite.GetSprite().SetEnableStep(TB_FALSE);
        m_PrePos = GetPos();
    }

    // 処理復帰
    virtual void OnResume() TB_OVERRIDE {
        m_Sprite.GetSprite().SetEnableStep(TB_TRUE);
    }

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param) TB_OVERRIDE { return 0; }

    // 表示設定
    virtual void SetVisible(TbBool isVisible) {
        m_Sprite.GetSprite().SetVisible(isVisible);
        TbForIterator(it, m_Components) {
            (*it)->OnChangedVisible(isVisible);
        }
    }

    // 接しているところから動く
    void MoveByContactActor(const toybox::TbVector2& mov, 
                            const toybox::TbVector2& normal, 
                            ActorSet* set, 
                            const MoveContactOption& option);

protected:

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnUpdateAction(TbFloat32 deltatime) {};

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnPostAction(TbFloat32 deltatime) {};

    // コリジョン初期化
    CrActionCollision* CreateCollision(const toybox::TbRectF32& rect, TbUint32 pushBackTarget, TbBool isEnableSyncPos);
    CrActionCollision* CreateCollision(const toybox::TbCircleF32& circle, TbUint32 pushBackTarget, TbBool isEnableSyncPos);

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack(const toybox::TbVector2& pos, const toybox::TbVector2& pushBackVec) {}

    // 接しているアクターを動かすことができるか
    TbBool IsEnableMoveContactActor(const toybox::TbVector2& mov, CrContactFace face, TbSint32 collisionIndex = 0);

    // コリジョンと同期
    void SyncCollisionPos()
    {
        if (!m_Collision.IsEmpty()) {
            TbForIterator(ite, m_Collision) {
                if (ite->isEnableSyncPos) {
                    ite->collision->SetPos(toybox::TbVector2(GetPos().GetX(), GetPos().GetY()));
                }
            }
        }
    }

    // コリジョンと同期
    void SyncCollisionPosForce()
    {
        if (!m_Collision.IsEmpty()) {
            TbForIterator(ite, m_Collision) {
                if (ite->isEnableSyncPos) {
                    ite->collision->SetPosForce(toybox::TbVector2(GetPos().GetX(), GetPos().GetY()));
                }
            }
        }
    }

    // スケール変化
    virtual void OnChangedScale(TbFloat32 scaleX, TbFloat32 scaleY)
    {
        TbForIterator(it, m_Components) {
            (*it)->OnUpdateScale(scaleX, scaleY);
        }
    }

    // 接触による移動
    virtual void OnMoveByContact( const toybox::TbVector2& mov , 
                                  const toybox::TbVector2& normal, 
                                  ActorSet* set , 
                                  const MoveContactOption& option) {}

private:

    enum Flag {
        FLAG_HINT,
        FLAG_ATTACHED , 
        FLAG_ENABLE_CHANGE_DIR,
        FLAG_ENABLE_SCROLL ,
        FLAG_ENABLE_NOTIFY_JUMP_RECURSIVE ,
        FLAG_MAX
    };

    struct CollisionHolder
    {
        CollisionHolder()
            : collision(nullptr)
            , isEnableSyncPos(TB_FALSE)
        {}
        CrActionCollision* collision;
        TbBool           isEnableSyncPos;
    };

    static const TbUint32 COLLISION_MAX = 5;

private:

    // コリジョンセットアップ
    void setupCollision(CrActionCollision* collision, TbUint32 pushBackTarget, TbBool isEnableSyncPos);

    // 押し戻し
    void onPushedBack(const toybox::TbVector2& pos, const toybox::TbVector2& pushBackVec) {
        SetPos(pos.GetX(), pos.GetY());
        OnPushedBack(pos, pushBackVec);
        
        toybox::TbVector2 drawPos = GetDrawPos();
        TbForIterator(it, m_Components) {
            (*it)->OnUpdatePos(pos,drawPos);
        }
    }

    // 接しているアクターを動かす
    void moveContactActor(const toybox::TbVector2& mov, const toybox::TbVector2& normal, ActorSet* set, const MoveContactOption& option);
    
private:

    toybox::TbBitArray32                                 m_BitArray;         // ビット配列
    toybox::TbStaticArray<CollisionHolder, COLLISION_MAX> m_Collision; // コリジョン
    toybox::TbVector2                       m_CollisionVelo;    // コリジョン適用速度
    toybox::TbVector4                       m_PrePos;
    toybox::TbVector2                       m_PreVelo;          // 前フレームの速度
    toybox::TbVector2                       m_Velo;             // 速度
    toybox::TbVector2                       m_MoveByContact;    // 接触による移動 
    toybox::TbVector2                       m_Accel;            // 加速度
    toybox::TbVector2                       m_InitPos;          // 初期位置
    toybox::TbVector2                       m_OffsetDrawPos;    // 初期位置
    toybox::TbVector2                       m_Wind;             // 風
    TbFloat32                               m_Weight;           // 重さ
    CrActionActorSprite                       m_Sprite;           // スプライト
    toybox::TbArray<CrActionActorComponent*>  m_Components;

    toybox::TbVector2                       m_DebugPos;
};

}

#endif
