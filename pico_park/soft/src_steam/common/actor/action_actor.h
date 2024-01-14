/*!
 * アクション用アクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_ACTOR_H_
#define _INCLUDED_ACTION_ACTOR_H_

#include <graphics/sprite/tb_sprite.h>
#include <base/container/tb_static_set.h>

#include "common/actor/actor.h"
#include "common/actor/action_actor_sprite.h"
#include "common/actor/action_actor_physics.h"
#include "common/actor/action_actor_component.h"
#include "common/collision/action_collision.h"

class ActionStage;

class ActionActor : public Actor
{
public:

    static const TbFloat32 WEIGHT_MAX;

    struct MoveContactOption
    {
        MoveContactOption()
            : isEnableSyncCollisionPos(TB_TRUE)
            , isEnableIgnoreTargetVelo(TB_FALSE)
            , collisionIndex(0)
            , targetCollisionLayerMask(0)
        {}
        TbBool isEnableSyncCollisionPos;    // コリジョン座標を同期する
        TbBool isEnableIgnoreTargetVelo;    // 対象の速度無視する
        TbSint32 collisionIndex;            // コリジョン番号
        TbUint32 targetCollisionLayerMask;  // 対象のレイヤーを絞る
    };

private:

    typedef ActionActor Self;
    typedef Actor Super;
    typedef TbStaticSet<ActionActor*,100> ActorSet;

public:

    // コンストラクタ
    ActionActor();

    // デストラクタ
    ~ActionActor();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public: 
    
    // アクションステージ取得
    const ActionStage* GetActionStage() const;

    // アクションステージ取得
    ActionStage* GetActionStage();

    // コリジョン取得
    ActionCollision* GetCollision( TbUint32 index = 0 ) {
        if( index < m_Collision.GetCount() ){
            return m_Collision[index].collision;
        }
        return NULL;
    }
    
    // コリジョン取得
    const ActionCollision* GetCollision( TbUint32 index = 0 ) const {
        if( index < m_Collision.GetCount() ){
            return m_Collision[index].collision;
        }
        return NULL;
    }

    // メインスプライトのオフセット座標設定
    void SetMainSpriteOffsetPos( const TbVector2& pos ) {
        m_Sprite.SetLocalPos(pos);
    }

    // スプライト取得
    TbSprite& GetMainSprite() {
        return m_Sprite.GetSprite();
    }

    // スプライト取得
    const TbSprite& GetMainSprite() const {
        return m_Sprite.GetSprite();
    }

    // コンポーネント追加
    void AddComponent( ActionActorComponent* component ) {
        component->SetOwner(this);
        m_Components.PushBack(component);
    }

    // ヒント
    void SetHint( TbBool isHint ) {
        m_BitArray.SetBit(FLAG_HINT);
    }

    // ヒント
    TbBool IsHint() const {
        return m_BitArray.Test(FLAG_HINT);
    }

    // 初期位置設定
    void SetInitPos( const TbVector2& pos ) {
        m_InitPos = pos;
        SetPos(pos.GetX(),pos.GetY());
        if( !m_Collision.IsEmpty() ){
            TbForIterator( ite , m_Collision ) {
                if( ite->isEnableSyncPos ){
                    ite->collision->SetInitPos(pos);
                }
            }
        }
    }

    // 初期位置取得
    const TbVector2& GetInitPos() const {
        return m_InitPos;
    }

    // 前フレームの位置取得
    const TbVector4& GetPrePos() const {
        return m_PrePos;
    }

    // 強制的に座標設定
    void SetPosForce( const TbVector2& pos ) {
        SetPos( pos.GetX() , pos.GetY() );
        m_PrePos.SetXY( pos.GetX() , pos.GetY() );
        TbForIterator( ite , m_Collision ) {
            if( ite->isEnableSyncPos ){
                ite->collision->SetPosForce(TbVector2(GetPos().GetX(),GetPos().GetY()));
            }
        }
    }

    // 速度取得
    const TbVector2& GetVelo() const {
        return m_Velo;
    }

    // 前フレームの速度取得
    const TbVector2& GetPreVelo() const {
        return m_PreVelo;
    }

    // コリジョン適用速度取得
    const TbVector2& GetCollisionVelo() const {
        return m_CollisionVelo;
    }

    // 速度リセット
    void ResetVelo() {
        m_Velo.Clear();
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度設定
    void SetVelo( const TbVector2& velo ) {
        m_Velo = velo;
        TB_ASSERT(!m_Velo.HasNan());
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // Y軸速度設定
    void SetVeloX( TbFloat32 x ) {
        m_Velo.SetX(x);
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // Y軸速度設定
    void SetVeloY( TbFloat32 y ) {
        m_Velo.SetY(y);
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度加算
    void AddVelo( TbFloat32 x , TbFloat32 y ) {
        m_Velo.Add(x,y);
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 速度加算
    void AddVelo( const TbVector2& velo ) {
        AddVelo(velo.GetX(),velo.GetY());
    }

        // 速度乗算
    void MulVelo( TbFloat32 val ) {
        m_Velo *= val;
        TbForIterator( it , m_Components ){
            (*it)->OnSetVelo(&m_Velo);
        }
    }

    // 加速度取得
    const TbVector2& GetAccel() const {
        return m_Accel;
    }

    // 加速度設定
    void SetAccel( const TbVector2& accel ){
        m_Accel = accel;
    }

    // Y軸加速度設定
    void SetAccelX( TbFloat32 x ) {
        m_Accel.SetX(x);
    }

    // Y軸加速度設定
    void SetAccelY( TbFloat32 y ) {
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
    void SetWeight( TbFloat32 weight ) {
        m_Weight = weight;
    }

    // 風設定
    void SetWind( const TbVector2& wind ){
        m_Wind = wind;
    }

    // 風設定
    void ResetWind(){
        m_Wind.Clear();
    }

    // 風設定
    const TbVector2& GetWind() const {
        return m_Wind;
    }

    // 右を向いているか
    virtual TbBool IsLookingRight() const {
        return GetMainSprite().GetScale().GetX() > 0.0f;
    }

    // 左向き
    virtual void LookLeft() {
        GetMainSprite().SetScale(-1.0f,1.0f);
    }

    // 右向き
    virtual void LookRight() {
        GetMainSprite().SetScale(1.0f,1.0f);
    }

public:

    // 描画位置
    TbVector2 GetDrawPos() const;

    // 描画補正位置設定
    void SetOffsetDrawPos( const TbVector2& pos ) {
        m_OffsetDrawPos = pos;
    }

    // 描画補正位置設定
    const TbVector2& GetOffsetDrawPos() const {
        return m_OffsetDrawPos;
    }

    // 接しているアクターを動かす
    void MoveContactActor( const TbVector2& mov , 
                           const TbVector2& normal, 
                           const MoveContactOption& option = MoveContactOption() )
    {
        ActorSet set;
        moveContactActor(mov,normal,&set,option);
    }

    // 接しているアクターを動かす
    void MoveContactActor( const TbVector2& mov , 
                           CmnContactFace face , 
                           const MoveContactOption& option = MoveContactOption())
    {
        ActorSet set;
        moveContactActor(mov,CmnUtil::GetContactFaceNormal(face),&set,option);
    }

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime ) {
        m_OffsetDrawPos.Clear(); // 毎フレームリセット
        m_PrePos = GetPos();
        m_PreVelo = m_Velo;
        OnUpdateAction(deltatime);
        AddPos( m_Velo + (m_Accel*0.5f) );
        m_Velo += m_Accel;
        // コリジョンと座標を同期
        SyncCollisionPos();
        TbVector2 pos = GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnUpdatePos(pos);
        }
    }

    // ロジック更新
    virtual void OnPost( TbFloat32 deltatime ) {
        OnPostAction(deltatime);

        // コリジョンと座標を同期
        SyncCollisionPos();

        TbVector4 mov = GetPos() - m_PrePos;
        m_CollisionVelo.Set( mov.GetX() , mov.GetY() ) ;
        TbVector2 pos = GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnUpdatePos(pos);
        }
    }

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // 処理中断
    virtual void OnSuspend() {
        m_Sprite.GetSprite().SetEnableStep(TB_FALSE);
    }

    // 処理復帰
    virtual void OnResume() {
        m_Sprite.GetSprite().SetEnableStep(TB_TRUE);
    }

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param ) { return 0; }

protected:

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnUpdateAction( TbFloat32 deltatime ) {};

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnPostAction( TbFloat32 deltatime ) {};

    // コリジョン初期化
    ActionCollision* CreateCollision( const TbRectF32& rect , TbUint32 pushBackTarget , TbBool isEnableSyncPos );
    ActionCollision* CreateCollision( const TbCircleF32& circle , TbUint32 pushBackTarget , TbBool isEnableSyncPos );

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {}

    // 接しているアクターを動かすことができるか
    TbBool IsEnableMoveContactActor( const TbVector2& mov , CmnContactFace face , TbSint32 collisionIndex = 0 );

    // コリジョンと同期
    void SyncCollisionPos() 
    {
        if( !m_Collision.IsEmpty() ){
            TbForIterator( ite , m_Collision ) {
                if( ite->isEnableSyncPos ){
                    ite->collision->SetPos(TbVector2(GetPos().GetX(),GetPos().GetY()));
                }
            }
        }
    }

private:

    enum Flag {
        FLAG_HINT ,
        FLAG_MAX 
    };

    struct CollisionHolder
    {
        CollisionHolder() 
            : collision(nullptr) 
            , isEnableSyncPos(TB_FALSE)
        {}
        ActionCollision* collision;
        TbBool           isEnableSyncPos;
    };

    static const TbUint32 COLLISION_MAX = 5;

private:

    // コリジョンセットアップ
    void setupCollision( ActionCollision* collision , TbUint32 pushBackTarget , TbBool isEnableSyncPos );

    // 押し戻し
    void onPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        SetPos( pos.GetX() , pos.GetY() );
        OnPushedBack(pos,pushBackVec);
        TbVector2 drawPos = GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnUpdatePos(drawPos);
        }
    }

    // 接しているアクターを動かす
    void moveContactActor( const TbVector2& mov , const TbVector2& normal , ActorSet* set , const MoveContactOption& option );

private:

    TbBitArray32                                 m_BitArray;         // ビット配列
    TbStaticArray<CollisionHolder,COLLISION_MAX> m_Collision; // コリジョン
    TbVector2                       m_CollisionVelo;    // コリジョン適用速度
    TbVector4                       m_PrePos;
    TbVector2                       m_PreVelo;          // 前フレームの速度
    TbVector2                       m_Velo;             // 速度
    TbVector2                       m_Accel;            // 加速度
    TbVector2                       m_InitPos;          // 初期位置
    TbVector2                       m_OffsetDrawPos;    // 初期位置
    TbVector2                       m_Wind;             // 風
    TbFloat32                       m_Weight;           // 重さ
    ActionActorSprite               m_Sprite;           // スプライト
    TbArray<ActionActorComponent*>  m_Components;

    TbVector2                       m_DebugPos;
};

#endif
