/*!
 * 物理ボール
 * @author teco
 */

#include "fwd.h"
#include "side_action_seesaw.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/side_action_types.h"


static const TbRectF32Pod SPRITE_RECT[] = 
{
    { -300.0f , -10.0f , 600.0f , 20.0f },
    { -300.0f , -10.0f , 450.0f , 20.0f },
    { -150.0f , -10.0f , 450.0f , 20.0f }
};

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] = 
{
    { CMN_TEX_UV_RECT(432,256,4,4) } , // PLACE_LU
    { CMN_TEX_UV_RECT(432,260,4,4) } , // PLACE_LC
    { CMN_TEX_UV_RECT(432,264,4,4) } , // PLACE_LB
    { CMN_TEX_UV_RECT(436,256,4,4) } , // PLACE_UP
    { CMN_TEX_UV_RECT(436,260,4,4) } , // PLACE_IN
    { CMN_TEX_UV_RECT(436,264,4,4) } , // PLACE_BT
    { CMN_TEX_UV_RECT(440,256,4,4) } , // PLACE_RU
    { CMN_TEX_UV_RECT(440,260,4,4) } , // PLACE_RC
    { CMN_TEX_UV_RECT(440,264,4,4) } , // PLACE_RB
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionSeesaw::SideActionSeesaw( Type type )
    : Super()
{
   // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(8.0f,8.0f);
    spriteParam.rect = SPRITE_RECT[type];
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i ) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    // 物理 (ピン止め)
    m_Shape.SetRect( SPRITE_RECT[type] );
    m_Shape.SetDensity(1.0f);
    m_Shape.SetFriction(1.0f);
    m_Body.GetBody().SetType( TbPhysicsBody2d::TYPE_DYNAMIC );
    m_Body.GetBody().SetShape(&m_Shape);
    m_Body.GetBody().SetGravityScale(0.0f);
    m_Joint.Initialize( &m_Body.GetBody() );
    m_Joint.GetJoint().SetLimit( TbDegToRad(-10.0f) , TbDegToRad(10.0f) );
    
    AddComponent(&m_Body);
    AddComponent(&m_Joint);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionSeesaw::~SideActionSeesaw()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionSeesaw::OnUpdateAction( TbFloat32 deltatime )
{
}

/*! 
 * ポスト更新
 * @author teco
 */
void SideActionSeesaw::OnPostAction( TbFloat32 deltatime )
{
    const TbVector2& pos = m_Body.GetBody().GetPos();
    TbRadian angle = m_Body.GetBody().GetAngle();
    SetPos( pos.GetX() , pos.GetY() );
    m_Sprite.GetSprite().SetRotation( TbRadToAng32(angle) );
}

/*! 
 * 連動シーソーと接続
 * @author teco
 */
void SideActionSeesaw::Attach( SideActionSeesaw* seesaw )
{
    TbPhysicsGearJoint2d& joint = m_GearJoint.GetJoint();
    joint.Initialize( seesaw->GetBody() , GetBody() ,
                      seesaw->GetJoint() , GetJoint() );
    joint.SetRatio(-1.0f);
    AddComponent( &m_GearJoint );
}

///////////////////////////////////////
//  SideActionSeesawParent
///////////////////////////////////////

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionSeesawParent::OnUpdateAction( TbFloat32 deltatime )
{
#if 1
    TbRadian srcAngle = GetBody()->GetAngle();
    TbRadian delta = (m_TargetAngle - srcAngle) * (1/deltatime);
    GetBody()->SetVeloAngle( delta );
#else
    GetBody()->SetVeloAngle( TbDegToRad(30.0f) * value );
#endif
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 SideActionSeesawParent::NotifyEvent( TbUint32 event , void* param )
{
    if( event == SIDE_ACTION_EVENT_ANGLE ){
        TbRadian angle = *reinterpret_cast<TbRadian*>(param);
        m_TargetAngle = angle;
    }
    return 0;
}
