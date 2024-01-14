/*!
 * 永遠障害物
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_ENDLESS_OBJSTACLE_H_
#define _INCLUDED_SIDE_ACTION_ENDLESS_OBJSTACLE_H_

#include "side_action_obstacle.h"

class SideActionEndlessObstacle : public SideActionObstacle
{
public:
    
    typedef SideActionEndlessObstacle Self;
    typedef SideActionObstacle Super;
    
public:

    // コンストラクタ
    SideActionEndlessObstacle();

    // デストラクタ
    virtual ~SideActionEndlessObstacle();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

private:

    struct NormalParam
    {
        NormalParam()
            : width(200.0f)
            , enterHeight(100.0f)
            , interval(600.0f)
            , random(0.0f)
        {
        }
        TbFloat32   width;
        TbFloat32   enterHeight;
        TbFloat32   interval;
        TbFloat32   random;
    };

private:

    // 生成
    TbFloat32 generateObstacle();

    // 一本道生成
    TbFloat32 generateRoad( float width , float interval );

    // コーナー生成
    TbFloat32 generateCorner( TbBool isDownEnter );

    // 天井生成
    TbFloat32 generateLongRoad();

    // 床生成
    void generateFloor();

private:

    TbBitArray32             m_BitArray;
    NormalParam              m_Param;
    TbUint32                 m_Level;
    TbUint32                 m_Counter;
    TbUint32                 m_NormalCounter;
    TbUint32                 m_SpCounter;
    TbUint32                 m_LatestObstacle;

};

#endif
