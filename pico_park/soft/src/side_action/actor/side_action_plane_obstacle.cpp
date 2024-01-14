/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_plane_obstacle.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbFloat32 RECT_HEIGHT = 1000.0f;

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionPlaneObstacle::SideActionPlaneObstacle()
    : Super()
    , m_Level(0)
    , m_PrevRandom(-1)
    , m_Index(0)
    , m_OffsetX(0.0f)
{
}

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionPlaneObstacle::~SideActionPlaneObstacle()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionPlaneObstacle::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_TYPE , 
        PARAM_LEVEL ,
        PARAM_WIDTH,
        PARAM_OFFSET_INDEX ,
        PARAM_OFFSET_X ,
        PARAM_RANDOM
    };

    // タイプ
    ObstacleParam rectParam;
    const char* nameSrc = userData.AsString(PARAM_TYPE);
    if (nameSrc) 
    {
        const TbStaticString32 name = nameSrc;
        if (name == "Road") {
            rectParam.type = OBSTACLE_TYPE_ROAD;
        }else if (name == "BlockRoad") {
            rectParam.type = OBSTACLE_TYPE_ROAD;
            rectParam.hasBlock = TB_TRUE;
        }else if (name == "RoadUp") {
            rectParam.type = OBSTACLE_TYPE_ROAD;
            rectParam.moveSpeed = -0.5f;
        }else if (name == "RoadDown") {
            rectParam.type = OBSTACLE_TYPE_ROAD;
            rectParam.moveSpeed = 0.5f;
        }else if (name == "UpCurve") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.curveStep = -10.0f;
        }else if (name == "DownCurve") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.curveStep = 10.0f;
#if 0
        }else if (name == "UpCurveUp") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.curveDir = -1.0f;
            rectParam.moveDir  = -1.0f;
        }else if (name == "UpCurveDown") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.curveDir = -1.0f;
            rectParam.moveDir = 1.0f;
        }else if (name == "DownCurveUp") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.curveDir = 1.0f;
            rectParam.moveDir = -1.0f;
        }else if (name == "DownCurveDown") {
            rectParam.type = OBSTACLE_TYPE_CURVE;
            rectParam.moveDir = 1.0f;
            rectParam.curveDir = 1.0f;
#endif
        }else {
            TB_ASSERT(0);
        }
    }
    m_Level = userData.AsSint32(PARAM_LEVEL);
    if (PARAM_LEVEL < userData.GetCount()) {
        m_Level = userData.AsFloat32(PARAM_LEVEL);
    }
    if (PARAM_WIDTH < userData.GetCount()) {
        rectParam.width = userData.AsFloat32(PARAM_WIDTH);
    }
    if (PARAM_OFFSET_INDEX < userData.GetCount()) {
        m_Index = userData.AsFloat32(PARAM_OFFSET_INDEX);
    }
    if (PARAM_OFFSET_X < userData.GetCount()) {
        m_OffsetX = userData.AsFloat32(PARAM_OFFSET_X);
    }
    if (PARAM_RANDOM < userData.GetCount()) {
        rectParam.random = userData.AsFloat32(PARAM_RANDOM);
    }
    if (1 <= m_Level) {
        rectParam.curveStep *= 2.0f;
    }
    rectParam.moveSpeedGapHeightScale = 1.2f;
    m_ObstacleParam = rectParam;
}

/*!
* ロジック更新
* @author teco
*/
void SideActionPlaneObstacle::OnEnter(void* userData)
{
    Super::OnEnter(userData);
    if (m_ObstacleParam.type != OBSTACLE_TYPE_INVALID)
    {
        SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
        TbUint32 playerCount = stage->GetPlayerCount() - CMN_PLAYER_MIN;
        TbFloat32 offsetX = m_OffsetX * static_cast<TbFloat32>(playerCount * m_Index);
        AddPos(offsetX,0.0f,0.0f);
        Generate(m_ObstacleParam);
    }
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlaneObstacle::OnUpdateAction(TbFloat32 deltatime)
{
}

/*!
 * 道生成
 * @author teco
 */
void SideActionPlaneObstacle::GenerateRoad(const ObstacleParam& param )
{
    TbFloat32 gapHeight = 85.0f;
    ObstacleParam tgtParam = param;

    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    TbFloat32 windowHeihgt = stage->GetWindowHeight();

    if ( 4 < playerCount ) {
        TbFloat32 offset = 50.0f;
        if ( 2 <= m_Level ) {
            offset -= 20.0f;
        }
        gapHeight += offset;
    } else {
        gapHeight += (playerCount-CMN_PLAYER_MIN)*5;
    }

    if ( !TB_FLOAT32_EQUAL_ZERO(tgtParam.moveSpeed) ) {
        gapHeight *= tgtParam.moveSpeedGapHeightScale;
        if (playerCount <= 4) {
            gapHeight *= 1.1f;
        }
    }
    
    TbFloat32 y = GetPosY() + randomY(tgtParam.random);
    if(tgtParam.moveSpeed != 0.0f ) {
        if (y < windowHeihgt*0.4f) 
        {
            tgtParam.moveSpeed = TbAbs(tgtParam.moveSpeed);
        }
        else if (windowHeihgt*0.6f < y) {
            tgtParam.moveSpeed = -TbAbs(tgtParam.moveSpeed);
        }
    }

    y -= gapHeight / 2.0f;
    TbRectF32 rect1(GetPosX(),
                    -RECT_HEIGHT,
                    1.0f, 
                    RECT_HEIGHT + y);
    TbRectF32 rect2(GetPosX(), 
                    y+gapHeight,
                    1.0f, 
                    RECT_HEIGHT);

    // 横幅
    rect1.width = tgtParam.width;
    rect2.width = tgtParam.width;

    // 矩形生成
    GenerateRect(tgtParam, rect1);
    GenerateRect(tgtParam, rect2);

    // ブロックを持っているか
    if (tgtParam.hasBlock) {
        static TbFloat32 BLOCK_WIDTH = 80.0f;
        TbRectF32 blockRect(rect1.x+rect1.width- BLOCK_WIDTH,
                            y,
                            BLOCK_WIDTH,
                            gapHeight);
        GenerateBlock(tgtParam, blockRect);
    }
}

/*!
 * Gap生成
 * @author teco
 */
void SideActionPlaneObstacle::GenerateCurve(const ObstacleParam& param)
{
    static const TbFloat32 RECT_WIDTH = 96.0f;
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    TbFloat32 gapHeight = 90.0f;
    TbFloat32 curveStep = param.curveStep;

    // 幅計算
    if (4 < playerCount) {
        TbFloat32 offset = 50.0f;
        if (2 <= m_Level) {
            offset -= 20.0f;
        }else {
            if (1 <= m_Level) {
                offset -= 5.0f;
            }
            curveStep *= 2.0f;
        }
        gapHeight += offset;
    }
    else {
        gapHeight += (playerCount - CMN_PLAYER_MIN) * 5;
        gapHeight -= m_Level*5.0f;
    }

    gapHeight += TbAbs(curveStep);

    TbFloat32 y = GetPosY() - gapHeight / 2.0f;
    y += randomY(param.random);

    TbUint32 count = param.width / RECT_WIDTH;
    for (TbUint32 i = 0; i < count; ++i) 
    {
        TbRectF32 rect1(GetPosX()+i*RECT_WIDTH-1.0f,
            -RECT_HEIGHT,
            RECT_WIDTH+2.0f,
            RECT_HEIGHT + y);
        TbRectF32 rect2(GetPosX()+i*RECT_WIDTH-1.0f,
            y + gapHeight,
            RECT_WIDTH + 2.0f,
            RECT_HEIGHT);

        // 矩形生成
        GenerateRect(param, rect1);
        GenerateRect(param, rect2);
        y += curveStep;
    }

}

/*!
 * 矩形生成
 * @author teco
 */
void SideActionPlaneObstacle::GenerateRect(const ObstacleParam& param, const TbRectF32& rect )
{
    CrActionActor* actor = Super::GenerateRect(rect, param.moveSpeed);
}

/*!
 * 押すブロック生成
 * @author teco
 */
void SideActionPlaneObstacle::GenerateBlock(const ObstacleParam& param, const TbRectF32& rect)
{
    CrStage* stage = GetStage();
    CrActorFactory::CreateParam createParam;
    createParam.typeName = "PushBox";
    createParam.name = "PushBox";
    createParam.pos.Set(rect.x+rect.width/2.0f, rect.y + rect.height);
    createParam.userData.SetCount(3);
    createParam.userData.SetF32(0, 100.0f);
    createParam.userData.SetF32(1, rect.width);
    createParam.userData.SetF32(2, rect.height);
    static_cast<CrActionActor*>(stage->CreateActor(createParam));
}

/*!
 * Y座標のランダム値
 * @author teco
 */
TbFloat32 SideActionPlaneObstacle::randomY(TbFloat32 value)
{
    static TbUint32 DELTA_MAX = 7;
    TbStaticArray<TbUint32,30> candidateTable;
    TbUint32 count = value/25.0f;
    if (count <= 0) {
        return 0.0f;
    }
    for (TbSint32 i = 0; i < count; ++i)
    {
        if (0 <= m_PrevRandom)
        {
#if (0 && TB_IS_DEBUG) // 候補固定検証用
            if (TbAbs(i - m_PrevRandom) == DELTA_MAX) {
                candidateTable.Clear();
                candidateTable.PushBack(i);
                break;
            }
#endif
            if ( TbAbs(i-m_PrevRandom) <= 1) {
                continue;
            }
            if ( DELTA_MAX <= TbAbs(i - m_PrevRandom)) {
                continue;
            }
        }
        candidateTable.PushBack(i);
    }
    TbUint32 index = TbRandom(candidateTable.GetCount()-1);
    m_PrevRandom = candidateTable[index];
    return -value/2.0f + (25.0f * candidateTable[index]);
}
