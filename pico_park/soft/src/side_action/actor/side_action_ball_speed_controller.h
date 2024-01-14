/*!
* ボールの速度コントローラ
* @author teco
*/

#ifndef _INCLUDED_SIDE_ACTION_BALL_SPEED_CONTROLLER_H_
#define _INCLUDED_SIDE_ACTION_BALL_SPEED_CONTROLLER_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action_key.h"
#include "root.h"

class SideActionBallSpeedController
{
public:

    enum SpeedType
    {
        SPEED_TYPE_NORMAL,
        SPEED_TYPE_SLOW,
        SPEED_TYPE_FAST,
        SPEED_TYPE_NORMAL_MAX,

        SPEED_TYPE_VERY_SLOW = SPEED_TYPE_NORMAL_MAX,
        SPEED_TYPE_VERY_FAST,
        SPEED_TYPE_VERY_MAX,

        SPEED_TYPE_VERY_VERY_SLOW = SPEED_TYPE_VERY_MAX,
        SPEED_TYPE_VERY_VERY_FAST,
        SPEED_TYPE_MAX,
    };

public:

    // コンストラクタ
    SideActionBallSpeedController();

    // デストラクタ
    virtual ~SideActionBallSpeedController();

public:

    // セットアップ
    void Setup( const CrActorFactory::UserData& userData , TbUint32 offset );

    // 速い速度に対する係数設定
    void SetFastSpeedCoef(TbFloat32 coef)
    {
        m_FastSpeedCoef = coef;
    }

    // ボール速度スケールランダム
    TbFloat32 RandBallSpeedScale( TbUint32 count , TbUint32 level );

    // ボール速度スケール取得
    TbFloat32 GetBallSpeedScale(SpeedType type);

private:

    TbFloat32           m_SpeedCoef;
    TbFloat32           m_FastSpeedCoef;

};


#endif
