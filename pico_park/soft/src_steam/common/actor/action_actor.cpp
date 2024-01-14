/*!
 * アクション用アクター
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "action_actor.h"
#include "common/stage/action_stage.h"

#include <base/util/tb_type_id.h>

const TbFloat32 ActionActor::WEIGHT_MAX = 100.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.06
 */
ActionActor::ActionActor()
    : Super()
    , m_Collision()
    , m_Weight(1.0f)
{
    m_Components.SetCapacity(5);
    m_Components.SetEnableSetCapacityAuto(TB_TRUE);
    AddComponent(&m_Sprite);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.06
 */
ActionActor::~ActionActor()
{
    TbForIterator( ite , m_Collision ) {
        TB_SAFE_DELETE(ite->collision);
    }
    m_Collision.Clear();
}

/*!
 * アクションステージ取得
 * @author teco
 * @since 2014.07.26
 */
const ActionStage* ActionActor::GetActionStage() const
{
    return static_cast<const ActionStage*>(GetStage());
}

/*!
 * アクションステージ取得
 * @author teco
 * @since 2014.07.26
 */
ActionStage* ActionActor::GetActionStage()
{
    return static_cast<ActionStage*>(GetStage());
}

/*!
 * コリジョン初期化
 * @author teco
 * @since 2013.10.06
 */
ActionCollision* ActionActor::CreateCollision( const TbRectF32& rect , TbUint32 pushBackTarget , TbBool isEnableSyncPos )
{
    if( m_Collision.IsFull() ){
        return NULL;
    }
    ActionCollision* collision = new ActionCollision();
    collision->Initialize(rect,pushBackTarget);
    setupCollision( collision , pushBackTarget , isEnableSyncPos );
    return collision;
}

/*!
 * コリジョン初期化
 * @author teco
 * @since 2013.10.06
 */
ActionCollision* ActionActor::CreateCollision( const TbCircleF32& circle , TbUint32 pushBackTarget , TbBool isEnableSyncPos )
{
    if( m_Collision.IsFull() ){
        return NULL;
    }
    ActionCollision* collision = new ActionCollision();
    collision->Initialize(circle,pushBackTarget);
    setupCollision( collision , pushBackTarget , isEnableSyncPos );
    return collision;
}

/*!
 * コリジョンセットアップ
 * @author teco
 * @since 2014.07.27
 */
void ActionActor::setupCollision( ActionCollision* collision , TbUint32 pushBackTarget , TbBool isEnableSyncPos )
{
    collision->SetActor(this);
    collision->SetLayer(ActionCollision::LAYER_DEFAULT);
    if( isEnableSyncPos ){
        collision->SetChangedPosCallback(TbCreateMemFunc(this,&Self::onPushedBack));
    }
    if( GetScene() ){
        // 既にシーンに入っていたら即時にシーンに追加
        ActionStage* actionStage = static_cast<ActionStage*>(GetStage());
        collision->Enter( &actionStage->GetCollisionScene() );
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
void ActionActor::OnEnter( void* userData )
{
    Stage* stage = reinterpret_cast<Stage*>(userData);
    ActionStage* actionStage = TbDynamicCast<ActionStage*>(stage);
    if( TB_VERIFY(actionStage) ) {
        TbForIterator( ite , m_Collision ) {
            ite->collision->Enter( &actionStage->GetCollisionScene() );
        }
        TbForIterator( it , m_Components ){
            TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
            (*it)->OnEnter( actionStage , pos , 0.0f );
       }
    }
}

/*! 
 * シーンから出た
 * @author teco
 * @since 2013.10.06
 */
void ActionActor::OnExit()
{
    TbForIterator( ite , m_Collision ) {
        ite->collision->Exit();
    }
    TbForIterator( it , m_Components ){
        (*it)->OnExit();
    }
}

/*!
 * 描画位置
 * @author teco
 * @since 2013.10.20
 */
TbVector2 ActionActor::GetDrawPos() const
{
    TbVector2 result(GetPosX(),GetPosY());
    result += m_OffsetDrawPos;
    Stage* stage = GetStage();
    if( stage ) {
        result.Add(-stage->GetScroll(),0.0f);
    }
    return result;
}

/*!
 * 接しているアクターを動かす
 * @author teco
 * @since 2014.08.06
 */
void ActionActor::moveContactActor( const TbVector2& mov , 
                                    const TbVector2& normal , 
                                    ActorSet* set , 
                                    const MoveContactOption& option)
{
    // くっついているアクターを連動して動かす
    const ActionCollision* collision = GetCollision(option.collisionIndex);
    ActionCollision* contactCollision[ActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , normal );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        if( !contactCollision[i]->IsEnablePushedFromFixedPosCollision() ){
            // 強制押されをしないものは除く
            continue;
        }
        // 
        if( 0 < option.targetCollisionLayerMask )
        {
            // マスクが設定されている場合は絞る
            if ( !(TB_BIT(contactCollision[i]->GetLayer()) & option.targetCollisionLayerMask )) { 
                continue;
            }
        }
        ActionActor* actor = static_cast<ActionActor*>(contactCollision[i]->GetActor());
        if( set ) {
            if( set->Find(actor) != set->End() ){
                // 既に動かし済み
                continue;
            }
            set->Insert(actor);
        }

        TbVector2 newMov = mov;

        // 対象速度を無視して問題ない場合は対象速度から進行方向成分を相殺させる成分を加える
        if(option.isEnableIgnoreTargetVelo) {
            TbVector2 v = mov;
            v.Normalize();
            TbFloat32 dot = v.Dot(actor->GetVelo());
            if( dot < 0.0f ) {
                newMov += -(v * dot);
            }
        }

        actor->AddPos( newMov );
        if(option.isEnableSyncCollisionPos){
            actor->SyncCollisionPos();
        }
        // 再帰的に動かす
        actor->moveContactActor( mov , normal , set , option );
    }

}

/*!
 * 接しているアクターを動かすことができるか
 * @author teco
 * @since 2014.10.21
 */
TbBool ActionActor::IsEnableMoveContactActor( const TbVector2& mov , CmnContactFace face , TbSint32 collisionIndex )
{
    if( mov.IsZero() ) {
        return TB_FALSE;
    }
    TbVector2 norm = mov;
    norm.Normalize();
    const TbVector2& faceNorm = CmnUtil::GetContactFaceNormal(face);

    // 下にくっついているアクターも連動して動かす
    const ActionCollision* collision = GetCollision(collisionIndex);
    ActionCollision* contactCollision[ActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = collision->GetContactCollision( contactCollision , norm );
    for( TbUint32 i = 0; i < contactCount; ++i ) {
        if( !contactCollision[i]->IsEnablePushedFromFixedPosCollision() ){
            if( faceNorm.Dot(norm) >= 0.0f ) {
                // 面している方向と動かしたい方向が一致してる場合は押せない
                return TB_FALSE;
            }else{
                continue;
            }
        }
        // マップと接触してたら動かせない
        if( contactCollision[i]->IsContactedMap(norm) ){
            return TB_FALSE;
        }
        ActionActor* actor = static_cast<ActionActor*>(contactCollision[i]->GetActor());
        if( !actor->IsEnableMoveContactActor( mov , face ) ){
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}
