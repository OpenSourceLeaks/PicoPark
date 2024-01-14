/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_obstacle.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbFloat32 RECT_HEIGHT = 1000.0f;

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionObstacle::SideActionObstacle()
    : Super()
    , m_ObstacleList()
{
    m_ObstacleList.SetEnableSetCapacityAuto(TB_TRUE);
}

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionObstacle::~SideActionObstacle()
{
    TbForIterator( it , m_ObstacleList)
    {
        it->actor->Release();
    }
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionObstacle::OnPostAction(TbFloat32 deltatime)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    for( auto it = m_ObstacleList.Begin(); it != m_ObstacleList.End(); )
    {
         if ( 0 == it->actor->NotifyEvent(SIDE_ACTION_EVENT_ALIVE_CONFIRM, nullptr)) {
            CrActionActor* actor = it->actor;
            actor->Exit();
            TB_SAFE_RELEASE(actor);
            it = m_ObstacleList.Erase(it);
        }else {
             if (it->isEnableVelo)
             {
                 TbVector2 drawPos = it->actor->GetDrawPos();
                 if (drawPos.GetX() < stage->GetWindowWidth()*1.2f)
                 {
                     it->actor->SetVelo(it->velo);
                     it->isEnableVelo = TB_FALSE;
                 }
             }
            ++it;
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionObstacle::NotifyEvent(TbUint32 event, void* param)
{
    if (SIDE_ACTION_EVENT_SUSPEND == event)
    {
        for (auto it = m_ObstacleList.Begin(); it != m_ObstacleList.End(); ++it )
        {
            it->actor->NotifyEvent(event, nullptr);
        }
        Suspend();
    }
    return 0;
}

/*!
 * 矩形生成
 * @author teco
 */
CrActionActor* SideActionObstacle::generateRect(const TbRectF32& rect)
{
    CrStage* stage = GetStage();
    CrActorFactory::CreateParam createParam;
    createParam.typeName = "DamageRect";
    createParam.name = "";
    createParam.pos.Set(rect.x, rect.y + rect.height);
    createParam.userData.SetCount(2);
    createParam.userData.SetF32(0, rect.width);
    createParam.userData.SetF32(1, rect.height);
    CrActionActor* actor = static_cast<CrActionActor*>(stage->CreateActor(createParam));
    if (actor) {
        actor->Retain();
    }
    return actor;
}

/*!
 * 矩形生成
 * @author teco
 */
CrActionActor* SideActionObstacle::GenerateRect(const TbRectF32& rect)
{
    CrActionActor* actor = generateRect(rect);
    RectInfo info;
    info.actor = actor;
    m_ObstacleList.PushBack(info);
    return actor;
}

/*!
 * 矩形生成
 * @author teco
 */
CrActionActor* SideActionObstacle::GenerateRect(const TbRectF32& rect, TbFloat32 moveSpeed)
{
    CrActionActor* actor = generateRect(rect);
    RectInfo info;
    info.actor = actor;
    info.velo.SetY(moveSpeed);
    info.isEnableVelo = TB_TRUE;
    m_ObstacleList.PushBack(info);
    return actor;
}

/*!
 * 天井生成
 * @author teco
 */
CrActionActor* SideActionObstacle::GenerateCeilRect(TbFloat32 x, TbFloat32 width, TbFloat32 height)
{
    TbRectF32 rect;
    rect.x = x;
    rect.y = -RECT_HEIGHT;
    rect.height = RECT_HEIGHT + height;
    rect.width = width;
    return GenerateRect(rect);
}

/*!
 * 床生成
 * @author teco
 */
CrActionActor* SideActionObstacle::GenerateFloorRect(TbFloat32 x, TbFloat32 width, TbFloat32 height)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbRectF32 rect;
    rect.x = x;
    rect.y = stage->GetWindowHeight()-height;
    rect.height = RECT_HEIGHT;
    rect.width = width;
    return GenerateRect(rect);
}

/*!
 * 箱生成
 * @author teco
 */
CrActionActor* SideActionObstacle::GenerateBox(TbFloat32 x, TbFloat32 y, TbFloat32 size)
{
    TbRectF32 rect;
    rect.x = x - size*0.5f;
    rect.y = y - size*0.5f;
    rect.height = size;
    rect.width = size;
    return GenerateRect(rect);
}

/*!
* 振幅設定
* @author teco
*/
void SideActionObstacle::SetActorAmplitude(CrActionActor*  actor, TbFloat32 veloAngle, TbFloat32 height)
{
    if (actor)
    {
        // actor->NotifyEvent();
    }
}
