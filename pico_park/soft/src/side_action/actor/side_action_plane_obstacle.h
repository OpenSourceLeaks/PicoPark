/*!
 * 飛行機用障害物
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_PLANE_OBSTACLE_H_
#define _INCLUDED_SIDE_ACTION_PLANE_OBSTACLE_H_

#include "side_action_obstacle.h"

class SideActionPlaneObstacle : public SideActionObstacle
{
public:
    
    typedef SideActionPlaneObstacle Self;
    typedef SideActionObstacle Super;
    
public:

    // コンストラクタ
    SideActionPlaneObstacle();

    // デストラクタ
    virtual ~SideActionPlaneObstacle();

public:

    // エンター処理
    virtual void OnEnter(void* userData) TB_OVERRIDE;

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

protected:

    enum ObstacleType
    {
        OBSTACLE_TYPE_INVALID ,
        OBSTACLE_TYPE_ROAD    , // 一本道
        OBSTACLE_TYPE_CURVE   , // 隙間道
    };

    struct ObstacleParam
    {
        ObstacleParam()
            : type(OBSTACLE_TYPE_INVALID)
            , hasBlock(TB_FALSE)
            , moveSpeed(0.0f)
            , moveSpeedGapHeightScale(1.0f)
            , width(0.0f)
            , random(100.0f)
            , curveStep(10.0f)
        {
        }

        ObstacleType    type;
        TbBool          hasBlock;
        TbFloat32       moveSpeed;
        TbFloat32       moveSpeedGapHeightScale;
        TbFloat32       width;
        TbFloat32       random;
        TbFloat32       curveStep; // Curve専用
    };

protected:

    // レベル設定
    void SetLevel(TbUint32 level)
    {
        m_Level = level;
    }

    // レベル取得
    TbUint32 GetLevel() const
    {
        return m_Level;
    }
    
    // 矩形生成
    void GenerateRect(const ObstacleParam& param, const TbRectF32& rect);

    // ブロック生成
    void GenerateBlock(const ObstacleParam& param, const TbRectF32& rect);

    // 生成
    void Generate(const ObstacleParam& param)
    {
        switch (param.type)
        {
        case OBSTACLE_TYPE_ROAD:
            GenerateRoad(param);
            break;
        case OBSTACLE_TYPE_CURVE:
            GenerateCurve(param);
            break;
        }
    }

    // 道生成
    void GenerateRoad( const ObstacleParam& param );

    // カーブ生成
    void GenerateCurve( const ObstacleParam& param );

private:

    // Y座標のランダム値
    TbFloat32 randomY( TbFloat32 value );

private:

    TbBitArray32             m_BitArray;
    TbUint32                 m_Level;
    TbSint32                 m_PrevRandom;
    ObstacleParam            m_ObstacleParam;
    TbUint32                 m_Index;
    TbFloat32                m_OffsetX;

};

#endif
