/*!
 * 距離コンストレイント
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_DISTANCE_CONSTRAINT_H_
#define _INCLUDED_SIDE_ACTION_DISTANCE_CONSTRAINT_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include "root.h"

class SideActionPlayer;
class SideActionStage;

class SideActionDistanceConstraint : public CrActionActor
{
public:

    typedef SideActionDistanceConstraint Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionDistanceConstraint();

    // デストラクタ
    virtual ~SideActionDistanceConstraint();

public:

    // セットアップ
    void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        TbUint32 distanceIndex = ((Root::GetInstance().GetPlayerCount() - 2)) * 2;
        TbUint32 gravityTensionIndex = distanceIndex + 1;
        if (userData.GetCount() <= distanceIndex) {
            distanceIndex = 0;
        }
        if (userData.GetCount() <= gravityTensionIndex) {
            gravityTensionIndex = 1;
        }

        TbFloat32 distance = userData.AsFloat32(distanceIndex);
        SetDistance(distance);
        if (gravityTensionIndex < userData.GetCount()) {
            TbFloat32 gravityTension = userData.AsFloat32(gravityTensionIndex);
            SetGracityTension(gravityTension);
        }
    }

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

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
        CalcInfo()
            : player(nullptr)
            , prevFloatingCount(0)
            , nextFloatingCount(0)
        {
        }
        SideActionPlayer* player;
        TbVector2   f;
        TbVector2   prevTension;
        TbVector2   nextTension;
        TbUint32    prevFloatingCount;
        TbUint32    nextFloatingCount;
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
    TbVector2 calcF( SideActionPlayer* src , 
                     SideActionPlayer* target , 
                     const TbVector2& srcTension , 
                     const TbVector2& targetTension , 
                     TbUint32 floatingCount ,
                     TbBool* wallUp );

    // 引っ張り計算
    TbVector2 calcPrevTension( TbSint32 playerIndex , SideActionStage* stage , TbUint32* floatingCount );

    // 引っ張り計算
    TbVector2 calcNextTension( TbSint32 playerIndex , SideActionStage* stage, TbUint32* floatingCount );

    // 引っ張り計算
    TbVector2 calcTension( SideActionPlayer* src , SideActionPlayer* target );

private:

    TbFloat32                       m_Distance;
    TbFloat32                       m_GraityTension;
    CrActionActorSpriteHooker       m_SpriteHooker;
    TbSint32                        m_PlayerBegin;
    TbSint32                        m_PlayerEnd;
    TbBool                          m_EnableForceMove[CMN_PLAYER_MAX];

};


#endif
