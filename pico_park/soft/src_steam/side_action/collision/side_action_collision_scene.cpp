/*!
 * アクション用コリジョン
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/collision/side_action_collision_scene.h"
#include "side_action/side_action_types.h"
#include "side_action/actor/side_action_player.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.10.06
 */
SideActionCollisionScene::SideActionCollisionScene()
    : Super()
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
    ActionCollision* co = GetTopCollision();
    if( co ) {
        do {
            updateInertia(co,TB_TRUE);
            co = GetNextCollision(co);
        } while( co );
    }
}

/*!
 * 慣性更新
 * coが動いている時、coの上にいるものも一緒に動くようにする
 * @author teco
 * @since 2014.05.02
 */
void SideActionCollisionScene::updateInertia( ActionCollision* co , TbBool isEnableCheckDown )
{
    // プレイヤー関係同士にのみ適用する
    if( co->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER &&
        co->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT) {
        return;
    }
    TbVector2 mov1;
    if( (!isEnableCheckDown || !co->IsContacted(CMN_CONTACT_DOWN,TB_FALSE)) ){
      TbVector2 mov = co->GetMov();
      TbRectF32 rect = co->GetGlobalRect();
      TbFloat32 center = rect.x + rect.width/2.0f;
      mov.SetY(0.0f); // X軸のみ
      if ( 0.0f < mov.GetX()) {
          int a = 0;
      }
      ActionMap* map = GetMap();

      ActionCollision* contactCollision[ActionCollision::CONTACT_GET_MAX];
      TbUint32 contactCount = co->GetContactCollision( contactCollision , CMN_CONTACT_UP );
      for( TbUint32 i = 0; i < contactCount; ++i ) {
            ActionCollision* movTarget = contactCollision[i];

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
            if( !TB_FLOAT32_EQUAL(mov.GetX(),0.0f) &&
                !( rect.x+rect.width < targetRect.x && targetRect.x + targetRect.width < targetRect.x )
              )
            {
                // マップに対して移動
                TbVector2 resultMov;
                TbVector2 before = movTarget->GetPos();
                movTarget->MoveMap(map,mov,&resultMov);

                // コリジョン同士との衝突
                ActionCollision* pushBackTarget = GetTopCollision();
                do {
                    if( pushBackTarget != movTarget && 
                        pushBackTarget->IsActivePushBackTarget(ActionCollision::PUSH_BACK_TARGET_COLLISION) ) 
                    {
                        // pushBackTargetの位置は固定で押し戻し処理を行う
                        ActionCollisionScene::Result res = UpdatePushBack( movTarget , 
                                                                           pushBackTarget , 
                                                                           &before , 
                                                                           &pushBackTarget->GetPos() );
                    }
                    pushBackTarget = GetNextCollision(pushBackTarget);
                } while( pushBackTarget );

                TbVector2 after = movTarget->GetPos();
            }

            // movTargetで再帰処理
            updateInertia( movTarget , TB_FALSE );
        }
    }
}
