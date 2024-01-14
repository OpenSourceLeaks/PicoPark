/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_endless_plane_obstacle.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionEndlessPlaneObstacle::SideActionEndlessPlaneObstacle()
    : Super()
    , m_Counter(0)
    , m_NormalCounter(0)
    , m_InitAutoScrollSpeed(-1.0f)
{
}

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionEndlessPlaneObstacle::~SideActionEndlessPlaneObstacle()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionEndlessPlaneObstacle::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_WIDTH,
        PARAM_INTERVAL,
        PARAM_INTERVAL_OFFSET,
        PARAM_RANDOM,
        PARAM_CURVE_RANDOM,
        PARAM_LAST_LEVEL_SCROLL,
    };
    if (PARAM_WIDTH < userData.GetCount())
    {
        m_Param.width = userData.AsFloat32(PARAM_WIDTH);
    }
    if (PARAM_INTERVAL < userData.GetCount())
    {
        m_Param.interval = userData.AsFloat32(PARAM_INTERVAL);
    }
    if (PARAM_INTERVAL_OFFSET < userData.GetCount())
    {
        m_Param.intervalOffset = userData.AsFloat32(PARAM_INTERVAL_OFFSET);
    }
    if (PARAM_RANDOM < userData.GetCount())
    {
        m_Param.random = userData.AsFloat32(PARAM_RANDOM);
    }
    if (PARAM_CURVE_RANDOM < userData.GetCount())
    {
        m_Param.curveRandom = userData.AsFloat32(PARAM_CURVE_RANDOM);
    }
    if (PARAM_LAST_LEVEL_SCROLL < userData.GetCount())
    {
        m_Param.lastLevelScroll = userData.AsFloat32(PARAM_LAST_LEVEL_SCROLL);
    }

}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionEndlessPlaneObstacle::OnUpdateAction(TbFloat32 deltatime)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32 score = stage->GetScrollX() / 10.0f;
    TbFloat32 border = stage->GetScrollX() + stage->GetWindowWidth() + 100.0f;
    TbFloat32 rate = TbMin(1.0f, score /m_Param.lastLevelScroll);
    if (m_InitAutoScrollSpeed < 0.0f)
    {
        m_InitAutoScrollSpeed = stage->GetAutoScrollSpeed();
    }
    stage->SetAutoScrollSpeed(m_InitAutoScrollSpeed+m_InitAutoScrollSpeed*rate);
    
    if (GetPosX() < border)
    {
        if (m_Param.lastLevelScroll <= score) {
            SetLevel(2);
        }else if (m_Param.lastLevelScroll/2.0f <= score) {
            SetLevel(1);
        }
        TbFloat32 width = generateObstacle();
        AddPos(width, 0.0f, 0.0f);
    }
}

/*! 
 * 障害物生成
 * @author teco
 */
TbFloat32 SideActionEndlessPlaneObstacle::generateObstacle()
{
    enum Type
    {
        TYPE_ROAD  , 
        TYPE_CURVE , 
    };
    static const TbUint32 SP_INTERVAL = 5;
    TbFloat32 width = m_Param.interval;
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    SetPosY(stage->GetWindowHeight()/2.0f);
    width += (stage->GetPlayerCount()-CMN_PLAYER_MIN) * m_Param.intervalOffset;
    Type type = TYPE_ROAD;
    if (m_NormalCounter < SP_INTERVAL)
    {
        type = TYPE_ROAD;
        ++m_NormalCounter;
    }
    else
    {
        type = TYPE_CURVE;
        m_NormalCounter = 0;
    }
    switch (type)
    {
    case TYPE_ROAD:
        generateRoad(m_Param.width);
        break;
    case TYPE_CURVE:
        generateCurve();
        break;
    default:
        TB_ASSERT(0);
        break;
    }
    ++m_Counter;
    return width;
}

/*!
 * 一本道生成
 * @author teco
 */
void SideActionEndlessPlaneObstacle::generateRoad(float width)
{
    ObstacleParam obstacle;
    obstacle.type   = OBSTACLE_TYPE_ROAD;
    obstacle.random = m_Param.random;
    obstacle.width  = m_Param.width;
    if ( 2 <= GetLevel())
    {
        obstacle.moveSpeed = 1.0f;
    }
    else if (1 <= GetLevel() )
    {
        obstacle.moveSpeed = 0.5f;
    }
    if (TbRandom(1))
    {
        obstacle.moveSpeed *= -1.0f;
    }
    obstacle.moveSpeedGapHeightScale = 1.2f;
    Generate(obstacle);
}

/*! 
 * カーブ生成
 * @author teco
 */
void SideActionEndlessPlaneObstacle::generateCurve()
{
    ObstacleParam obstacle;
    obstacle.type = OBSTACLE_TYPE_CURVE;
    obstacle.random = m_Param.curveRandom;
    obstacle.width = m_Param.width;
    if (TbRandom(1))
    {
        obstacle.curveStep *= -1.0f;
    }
    Generate(obstacle);
}

