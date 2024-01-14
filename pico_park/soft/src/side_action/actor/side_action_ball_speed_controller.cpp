/*!
* 鍵箱
* @author teco
*/

#include "fwd.h"
#include "side_action_ball_speed_controller.h"
#include "side_action_text.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"
#include "side_action/side_action_types.h"

/*!
* コンストラクタ
* @author teco
*/
SideActionBallSpeedController::SideActionBallSpeedController()
    : m_SpeedCoef(1.0f)
    , m_FastSpeedCoef(1.0f)
{
}

/*!
* デストラクタ
* @author teco
*/
SideActionBallSpeedController::~SideActionBallSpeedController()
{
}

/*!
* セットアップ
* @author teco
*/
void SideActionBallSpeedController::Setup(const CrActorFactory::UserData& userData , TbUint32 indexOffset )
{
    TbSint32 speedIndex = TbMax<TbSint32>(0,Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN) + indexOffset;
    speedIndex = TbMin<TbSint32>(speedIndex, userData.GetCount()-1);
    if (speedIndex < userData.GetCount())
    {
        m_SpeedCoef = userData.AsFloat32(speedIndex);
    }
}

/*!
 * ボール速度スケールランダム
 * @author teco
 */
TbFloat32 SideActionBallSpeedController::RandBallSpeedScale(TbUint32 count, TbUint32 level)
{
    TbFloat32 scale = 1.0f;
    if (level == 3 && count < 5)
    {
        SpeedType TABLE[] =
        {
            SPEED_TYPE_VERY_SLOW ,
            SPEED_TYPE_SLOW ,
            SPEED_TYPE_NORMAL ,
            SPEED_TYPE_FAST ,
            SPEED_TYPE_VERY_FAST ,
        };
        SpeedType type = TABLE[count];
        scale = GetBallSpeedScale(type);
    }
    else if( count < 2 )
    {
        switch (level)
        {
        case 1:
            {
                SpeedType type = static_cast<SpeedType>(SPEED_TYPE_SLOW + count);
                scale = GetBallSpeedScale(type);
            }
            break;
        case 2:
            {
                SpeedType type = static_cast<SpeedType>(SPEED_TYPE_VERY_SLOW + count);
                scale = GetBallSpeedScale(type);
            }
            break;
        case 4:
            {
                SpeedType type = static_cast<SpeedType>(SPEED_TYPE_VERY_VERY_SLOW + count);
                scale = GetBallSpeedScale(type);
            }
            break;
        }
    }
    else {
        switch (level)
        {
        case 0:
            break;
        case 1:
            {
                SpeedType type = static_cast<SpeedType>(TbRandom(SPEED_TYPE_NORMAL_MAX - 1));
                scale = GetBallSpeedScale(type);
            }
            break;
        case 2:
        case 3:
            default:
            {
                SpeedType type = static_cast<SpeedType>(TbRandom(SPEED_TYPE_VERY_MAX - 1));
                scale = GetBallSpeedScale(type);
            }
            break;
        case 4:
            {
                SpeedType type = static_cast<SpeedType>(TbRandom(SPEED_TYPE_MAX - 1));
                scale = GetBallSpeedScale(type);
            }
            break;
        }
    }
    return scale;

}


/*!
 * ボール速度スケール取得
 * @author teco
 */
TbFloat32 SideActionBallSpeedController::GetBallSpeedScale(SpeedType type)
{
    TbFloat32 scale = 1.0f;
    switch (type)
    {
    case SPEED_TYPE_SLOW:
        scale /= m_SpeedCoef;
        break;
    case SPEED_TYPE_VERY_SLOW:
        scale /= m_SpeedCoef * m_SpeedCoef;
        break;
    case SPEED_TYPE_VERY_VERY_SLOW:
        scale /= ( m_SpeedCoef * m_SpeedCoef ) * 1.5f;
        break;
    case SPEED_TYPE_FAST:
        scale *= m_SpeedCoef;
        scale /= m_FastSpeedCoef;
        break;
    case SPEED_TYPE_VERY_FAST:
        scale *= m_SpeedCoef * m_SpeedCoef;
        scale /= m_FastSpeedCoef;
        break;
    case SPEED_TYPE_VERY_VERY_FAST:
        if (5 <= Root::GetInstance().GetPlayerCount())
        {
            scale *= m_SpeedCoef * m_SpeedCoef * 1.1f;
        }
        else
        {
            scale *= m_SpeedCoef * m_SpeedCoef;
        }
        scale /= m_FastSpeedCoef;
        break;
    }
    return scale;
}


