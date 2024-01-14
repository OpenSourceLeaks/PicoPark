/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_endless_obstacle.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static enum Type
{
    TYPE_ROAD, // 一本待ち

    // 数回に一回のやつ
    TYPE_LONG_ROAD   , // 天井
    TYPE_CORNER1 , // コーナー
    TYPE_CORNER2 , // コーナー
    TYPE_MAX
};

static const TbFloat32 RECT_HEIGHT = 1000.0f;
static const TbFloat32 SP_AREA_WIDTH  = 1000.0f;
static const TbFloat32 SP_OBSTACLE_WIDTH = 600.0f;

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionEndlessObstacle::SideActionEndlessObstacle()
    : Super()
    , m_Level(0)
    , m_Counter(0)
    , m_NormalCounter(0)
    , m_SpCounter(0)
    , m_LatestObstacle(0)
{
}

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionEndlessObstacle::~SideActionEndlessObstacle()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionEndlessObstacle::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_WIDTH ,
        PARAM_ENTER_HEIGHT ,
        PARAM_INTERVAL ,
        PARAM_RANDOM ,
    };
    if (PARAM_WIDTH < userData.GetCount())
    {
        m_Param.width = userData.AsFloat32(PARAM_WIDTH);
    }
    if (PARAM_ENTER_HEIGHT < userData.GetCount())
    {
        m_Param.enterHeight = userData.AsFloat32(PARAM_ENTER_HEIGHT);
    }
    if (PARAM_INTERVAL < userData.GetCount())
    {
        m_Param.interval = userData.AsFloat32(PARAM_INTERVAL);
    }
    if (PARAM_RANDOM < userData.GetCount())
    {
        m_Param.random = userData.AsFloat32(PARAM_RANDOM);
    }
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionEndlessObstacle::OnUpdateAction(TbFloat32 deltatime)
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32 border = stage->GetScrollX()+stage->GetWindowWidth()+100.0f;
    if (GetPosX() < border) 
    {
        TbFloat32 score = stage->GetScrollX() / 10.0f;
        if (10000.0f <= score)
        {
            m_Level = 2;
        }
        TbFloat32 width = generateObstacle();
        AddPos(width, 0.0f, 0.0f);
    }
}

/*!
 * 生成
 * @author teco
 */
TbFloat32 SideActionEndlessObstacle::generateObstacle()
{
    static const TbUint32 SP_INTERVAL = 10;
    TbFloat32 width = 1000.0f;
    Type type = TYPE_ROAD;
    if( m_NormalCounter < SP_INTERVAL)
    {
        type = TYPE_ROAD;
        ++m_NormalCounter;
    }
    else
    {
        if (m_SpCounter < TYPE_MAX - 1)
        {
            type = static_cast<Type>(TYPE_ROAD + 1 + m_SpCounter);
        }
        else
        {
            type = static_cast<Type>(TYPE_ROAD + 1 + TbRandom(TYPE_MAX-2));
        }
        m_NormalCounter = 0;
        ++m_SpCounter;
    }
    switch (type)
    {
    case TYPE_ROAD:
        width = generateRoad(m_Param.width, m_Param.interval);
        break;
    case TYPE_LONG_ROAD:
        {
            float w = m_Param.width*4.0f;
            float interval = m_Param.interval-m_Param.width;
            width = generateRoad(w,w+interval);
        }  
        break;
    case TYPE_CORNER1:
        width = generateCorner(TB_TRUE);
        break;
    case TYPE_CORNER2:
        width = generateCorner(TB_FALSE);
        break;
    default:
        TB_ASSERT(0);
        break;
    }
    TB_ASSERT(width!=0.0f);
    m_LatestObstacle = type;
    ++m_Counter;
    return width;
}

/*!
 * 天井生成
 * @author teco
 */
TbFloat32 SideActionEndlessObstacle::generateLongRoad()
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    GenerateCeilRect(GetPosX(), SP_OBSTACLE_WIDTH,stage->GetWindowHeight() / 2.0f);
    return  SP_AREA_WIDTH;
}

/*!
 * 一本道生成
 * @author teco
 */
TbFloat32 SideActionEndlessObstacle::generateRoad( float width , float interval )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32            y = stage->GetWindowHeight() / 2.0f;
    TbFloat32   roadHeight = m_Param.enterHeight;
    if(2 <= m_Level)
    {
        // 高難易度モード
        roadHeight -= 10.0f;
        interval -= 200.0f;
    }

    y += -(m_Param.random / 2.0f) + TbRandomFloat()*m_Param.random;

    TbFloat32 height = stage->GetWindowHeight()*0.5f - roadHeight*0.5f;

    TbRectF32 rect1(GetPosX(),
        -RECT_HEIGHT,
        width,
        RECT_HEIGHT + y - roadHeight/2.0f);

    TbRectF32 rect2(GetPosX(),
        y + (roadHeight/2.0f),
        width,
        RECT_HEIGHT);

    // 矩形生成
    GenerateRect(rect1);
    GenerateRect(rect2);
    return interval;
}

/*!
 * コーナー生成
 * @author teco
 */
TbFloat32 SideActionEndlessObstacle::generateCorner(TbBool isDownEnter)
{
    TbFloat32 totalWidth = SP_AREA_WIDTH;
    TbFloat32 width      = SP_OBSTACLE_WIDTH/3.0f;

    static const TbFloat32 RANDOM_Y = 100.0f;
    SideActionStage*     stage  = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32            enterY = stage->GetWindowHeight() * 0.75f;
    TbFloat32            exitY  = stage->GetWindowHeight() * 0.25f;
    TbFloat32      enterHeight  = stage->GetWindowHeight() * 0.4f; // 入口高さ

    if (2 <= m_Level)
    {
        // 高難易度モード
        totalWidth -= 200.0f;
        width -= 40.0f;
        enterHeight -= 40.0f;
    }

    TbRectF32 rect1(GetPosX(),
        -RECT_HEIGHT,
        width,
        RECT_HEIGHT + enterY - enterHeight/2.0f );

    TbRectF32 rect2(GetPosX(),
        enterY + (enterHeight / 2.0f),
        width*3.0f,
        RECT_HEIGHT);

    TbRectF32 rect3(GetPosX()+ width*2.0f,
        exitY + (enterHeight / 2.0f),
        width,
        RECT_HEIGHT);

    if (!isDownEnter)
    {
        rect1.x = GetPosX() + width*2.0f;
        rect3.x = GetPosX();
    }

    // 矩形生成
    GenerateRect(rect1);
    GenerateRect(rect2);
    GenerateRect(rect3);

    return  totalWidth;
}

#if 0
/*!
 * メテオ生成
 * @author teco
 */
TbFloat32 SideActionEndlessObstacle::generateMeteor()
{
    static const TbFloat32 STONE_SIZE = 33.0f;
    SideActionStage*     stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32 height = stage->GetWindowHeight()*0.1f;
    GenerateCeilRect(GetPosX(),SP_OBSTACLE_WIDTH, height);
    GenerateFloorRect(GetPosX(), SP_OBSTACLE_WIDTH, height);

    TbFloat32 centerX = SP_OBSTACLE_WIDTH/2.0f;
    TbFloat32 centerY = stage->GetWindowHeight()*0.5f;
    TbFloat32 intervalX = SP_OBSTACLE_WIDTH/4.0f;

    GenerateBox(GetPosX()+centerX,centerY,STONE_SIZE);
    GenerateBox(GetPosX() + centerX - intervalX, centerY, STONE_SIZE);
    GenerateBox(GetPosX() + centerX + intervalX, centerY, STONE_SIZE);

    return  SP_AREA_WIDTH;
}
#endif

/*!
 * 天井生成
 * @author teco
 */
void SideActionEndlessObstacle::generateFloor()
{
}
