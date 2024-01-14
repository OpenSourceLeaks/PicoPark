/*!
 * 鍵箱
 * @author teco
 */

#include "fwd.h"
#include "side_action_ball_receiver.h"
#include "side_action_text.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

//////////////////////////////////////////////////
// SideActionBallReceiver
//////////////////////////////////////////////////

static const TbRectF32Pod SPRITE_RECT = 
{
    -23.0f , -62.0f , 46.0f , 62.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(288,512,23,31),
};

static const TbRectF32Pod COLLISION_RECT =
{
    -22.0f , -62.0f , 44.0f , 60.0f
};

/*!
 * コンストラクタ
 * @author teco
 */ 
SideActionBallReceiver::SideActionBallReceiver()
    : m_Target()
    , m_Count(0)
    , m_CountInLevel(0)
    , m_Level(0)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetContactedCallback( TbCreateMemFunc(this,&Self::onContacted) );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
}

/*!
 * デストラクタ
 * @author teco
 */ 
SideActionBallReceiver::~SideActionBallReceiver()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionBallReceiver::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_TARGET , 
        PARAM_SPEED_COEF
    };
    if (userData.AsString(PARAM_TARGET)) {
        m_BallSpeedController.Setup(userData, PARAM_SPEED_COEF);
        m_Target = userData.AsString(PARAM_TARGET);
    }
}

/*!
* レベルアップ
* @author teco
*/
void SideActionBallReceiver::LevelUp()
{
    ++m_Level;
    m_CountInLevel = 0;

    CrActionStage* stage = GetActionStage();
    TbActorScene* scene = &stage->GetActorScene();
    TbVector2 initPos( -80.0f , 180.0f );
    SideActionText* text = TB_NEW SideActionText();
    TbVector2 velo;
    velo.SetX(8.0f);
    text->SetText("LEVEL UP");
    text->SetFontSize(32.0f);
    text->SetVelo(velo);
    text->SetStage(stage);
    text->SetInitPos(initPos);
    text->SetLifeSec(10.0f);
    text->Enter(scene, stage);

}

/*!
 * ロジック更新
 * @author teco
 */ 
void SideActionBallReceiver::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 接触開始コールバック
 * @author teco
 */ 
void SideActionBallReceiver::onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision)
{
    enum
    {
        COUNT_LEVEL_1 = 5   , // 10
        COUNT_LEVEL_2 = 20  , // 50
        COUNT_LEVEL_3 = 40  , // 50
        COUNT_LEVEL_4 = 100 , // 50
    };
    if (target != CrActionCollision::CONTACT_TARGET_COLLISION) {
        return;
    }
    if (collision.collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_OBJECT) {
        ++m_Count;
        ++m_CountInLevel;
        if ( COUNT_LEVEL_1 == m_Count || 
             COUNT_LEVEL_2 == m_Count ||
             COUNT_LEVEL_3 == m_Count ||
             COUNT_LEVEL_4 == m_Count )
        {
            LevelUp();
        }
        if (COUNT_LEVEL_3 == m_Count)
        {
            GetStage()->NotifyEvent(m_Target.GetBuffer(), SIDE_ACTION_EVENT_SPEACIAL, nullptr);
        }
        TbFloat32 scale = m_BallSpeedController.RandBallSpeedScale(m_CountInLevel,m_Level);
        if (!m_Target.IsEmpty())
        {
            GetStage()->NotifyEvent(m_Target.GetBuffer(), SIDE_ACTION_EVENT_LEVEL_DOWN, nullptr);
            GetStage()->NotifyEvent(m_Target.GetBuffer(), SIDE_ACTION_EVENT_LEVEL_UP, &scale);
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */ 
TbSint32 SideActionBallReceiver::NotifyEvent(TbUint32 event, void* param)
{
    return 0;
}


