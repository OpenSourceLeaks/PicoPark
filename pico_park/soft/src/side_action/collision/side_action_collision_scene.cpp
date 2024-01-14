/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/collision/side_action_collision_scene.h"
#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_player.h"
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.06
 */
SideActionCollisionScene::SideActionCollisionScene()
    : Super()
    , m_IsEnableInertia(TB_TRUE)
{
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.10.06
 */
SideActionCollisionScene::~SideActionCollisionScene()
{
}

/*!
 * 押し戻し後処理
 * @author teco
 * @since 2014.06.25
 */
void SideActionCollisionScene::OnPostPushBack()
{
    if( IsEnableInertia() )
    {
        CrActionCollision* co = GetTopCollision();
        if( co ) {
            do {
                CollisionSet set;
                updateInertia(co,TB_TRUE,&set);
                co = GetNextCollision(co);
            } while( co );
        }
        fixPlayerPos();
    }
}

/*!
 * 慣性更新
 * coが動いている時、coの上にいるものも一緒に動くようにする
 * @author teco
 * @since 2014.05.02
 */
void SideActionCollisionScene::updateInertia( CrActionCollision* co , TbBool isEnableCheckDown , CollisionSet* set )
{
    // プレイヤー関係同士にのみ適用する
    if( co->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER &&
        co->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT) 
    {
        return;
    }
    if ( co->GetUserDataU32(SIDE_ACTION_COLLISION_U32_DISABLE_INERTIA) ) {
        return;
    }

    if (set)
    {
        if (set->Find(co) != set->End()) {
            // 既に動かし済み
            return;
        }
        set->Insert(co);
    }

    TbVector2 mov1;
    TbBool isContactDown = TB_FALSE;
    if(isEnableCheckDown)
    {
        CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
        TbUint32 contactCount = co->GetContactCollision(contactCollision, CR_CONTACT_DOWN);
        for (TbUint32 i = 0; i < contactCount; ++i) {
            CrActionCollision* downTarget = contactCollision[i];
            if (co->GetUserDataU32(SIDE_ACTION_COLLISION_U32_DISABLE_INERTIA)) {
                continue;
            }
            if (downTarget->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ||
                downTarget->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT)
            {
                isContactDown = TB_TRUE;
                break;
            }
        }
    }

    if( !isContactDown){
      TbVector2 mov = co->GetMov();
      TbRectF32 rect = co->GetGlobalRect();
      TbFloat32 center = rect.x + rect.width/2.0f;
      mov.SetY(0.0f); // X軸のみ
      CrActionMap* map = GetMap();
      CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
      TbUint32 contactCount = co->GetContactCollision( contactCollision , CR_CONTACT_UP );
      for( TbUint32 i = 0; i < contactCount; ++i ) {
            CrActionCollision* movTarget = contactCollision[i];

            // todo:慣性で動くものはレイヤーで区別してるが
            // フラグを設定できるようにしておく
            if( (movTarget->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER) &&
                (movTarget->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT))
            {
                continue;
            }

            SideActionPlayer* player = TbDynamicCast<SideActionPlayer*>(movTarget->GetActor());
            if( player ){
                if( player->IsSleeping() ) {
                    // 寝てたら何もしない
                    continue;
                }
            }
            TbRectF32 targetRect = movTarget->GetGlobalRect();
            TbVector2 mapMov;
            TbVector2 before;
            TbVector2 after;
            if( !TB_FLOAT32_EQUAL(mov.GetX(),0.0f) &&
                !( rect.x+rect.width < targetRect.x && targetRect.x + targetRect.width < targetRect.x )
              )
            {
                // マップに対して移動
                before = movTarget->GetPos();
                movTarget->MoveMap(map,mov,&mapMov);

                // コリジョン同士との衝突
                CrActionCollision* pushBackTarget = GetTopCollision();
                do {
                    CrActionCollisionScene::Result res = CrActionCollisionScene::RESULT_OK;
                    if( pushBackTarget != movTarget && 
                        pushBackTarget->IsActivePushBackTarget(CrActionCollision::PUSH_BACK_TARGET_COLLISION) ) 
                    {
                        // pushBackTargetの位置は固定で押し戻し処理を行う
                        res = UpdatePushBack( movTarget , 
                                              pushBackTarget , 
                                              &before , 
                                              &pushBackTarget->GetPos() );
                    }
                    if (res == CrActionCollisionScene::RESULT_AGAIN)
                    {
                        // やり直し
                        pushBackTarget = GetTopCollision();
                    }
                    else
                    {
                        pushBackTarget = GetNextCollision(pushBackTarget);
                    }
                } while( pushBackTarget );

                after = movTarget->GetPos();
            }

            // movTargetで再帰処理
            updateInertia( movTarget , TB_FALSE, set);
        }
    }
}

// プレイヤー位置を確定
void SideActionCollisionScene::fixPlayerPos()
{
    CrActionCollision* co = GetTopCollision();
    if (co) {
        do {
            if (co->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ||
                co->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT)
            {
                co->SetPosForce(co->GetPos());
            }
            co = GetNextCollision(co);
        } while (co);
    }
}
