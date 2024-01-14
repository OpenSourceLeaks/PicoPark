/*!
 * エンドレス飛行機用障害物
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_ENDLESS_PLANE_OBSTACLE_H_
#define _INCLUDED_SIDE_ACTION_ENDLESS_PLANE_OBSTACLE_H_

#include "side_action_plane_obstacle.h"

class SideActionEndlessPlaneObstacle : public SideActionPlaneObstacle
{
public:
    
    typedef SideActionEndlessPlaneObstacle Self;
    typedef SideActionPlaneObstacle Super;
    
public:

    // コンストラクタ
    SideActionEndlessPlaneObstacle();

    // デストラクタ
    virtual ~SideActionEndlessPlaneObstacle();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

private:

    struct Param
    {
        Param()
            : width(200.0f)
            , interval(600.0f)
            , intervalOffset(0.0f)
            , random(0.0f)
            , curveRandom(0.0f)
            , lastLevelScroll(3000.0f)
        {
        }
        TbFloat32   width;
        TbFloat32   interval;
        TbFloat32   intervalOffset;
        TbFloat32   random;
        TbFloat32   curveRandom;
        TbFloat32   lastLevelScroll;
    };

private:

    // 障害物生成
    TbFloat32 generateObstacle();

    // 一本道生成
    void generateRoad(float width );

    // コーナー生成
    void generateCurve();

private:

    Param    m_Param;
    TbUint32 m_Counter;
    TbUint32 m_NormalCounter;
    TbFloat32 m_InitAutoScrollSpeed;

};

#endif
