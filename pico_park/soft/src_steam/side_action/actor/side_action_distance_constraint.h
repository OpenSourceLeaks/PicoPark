/*!
 * 距離コンストレイント
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_DISTANCE_CONSTRAINT_H_
#define _INCLUDED_SIDE_ACTION_DISTANCE_CONSTRAINT_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionPlayer;
class SideActionStage;

class SideActionDistanceConstraint : public ActionActor
{
public:

    typedef SideActionDistanceConstraint Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionDistanceConstraint();

    // デストラクタ
    virtual ~SideActionDistanceConstraint();

public:

    // 距離設定
    void SetDistance( TbFloat32 distance ){
        m_Distance = distance;
    }

    // 重力に引っ張られる力
    void SetGracityTension(TbFloat32 tension) {
        m_GraityTension = tension ;
    }

protected:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime ) TB_OVERRIDE;

private:

    struct CalcInfo
    {
        SideActionPlayer* player;
        TbVector2   f;
        TbVector2   prevTension;
        TbVector2   nextTension;
    };

    static const TbFloat32 GRAVITY_TENSION_DEFAULT;

private:

    // 前景描画
    void onDrawConstraint( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 前景描画
    void onDrawConstraintPlayer( const SideActionPlayer* player, const SideActionPlayer* next , const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    // 解決
    void calcF( CalcInfo* player, CalcInfo* prev , CalcInfo* next );

    // 解決
    TbVector2 calcF( SideActionPlayer* src , SideActionPlayer* target , const TbVector2& srcTension , const TbVector2& targetTension );

    // 引っ張り計算
    TbVector2 calcPrevTension( TbSint32 playerIndex , SideActionStage* stage );

    // 引っ張り計算
    TbVector2 calcNextTension( TbSint32 playerIndex , SideActionStage* stage );

    // 引っ張り計算
    TbVector2 calcTension( SideActionPlayer* src , SideActionPlayer* target );

private:

    TbFloat32                       m_Distance;
    TbFloat32                       m_GraityTension;
    ActionActorSpriteHooker         m_SpriteHooker;
    TbSint32                        m_PlayerBegin;
    TbSint32                        m_PlayerEnd;

};


#endif
