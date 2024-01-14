/*!
 * 重さで動く台
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_variable_weighted_lift.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static TB_NDEBUG_CONST TbFloat32 MOVE_SPEED_DEFAULT = 1.0f;
static TB_NDEBUG_CONST TbFloat32 HEIGHT = -70.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 0.06f;

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] =
{
    { CMN_TEX_UV_RECT(0       ,528     ,16,16) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,528 + 16,16,16) } , // PLACE_LC
    { CMN_TEX_UV_RECT(0       ,528 + 32,16,16) } , // PLACE_LB
    { CMN_TEX_UV_RECT(16      ,528     ,16,16) } , // PLACE_UP
    { CMN_TEX_UV_RECT(16      ,528 + 16,16,16) } , // PLACE_IN
    { CMN_TEX_UV_RECT(16      ,528 + 32,16,16) } , // PLACE_BT
    { CMN_TEX_UV_RECT(32      ,528     ,16,16) } , // PLACE_RU
    { CMN_TEX_UV_RECT(32      ,528 + 16,16,16) } , // PLACE_RC
    { CMN_TEX_UV_RECT(32      ,528 + 32,16,16) } , // PLACE_RB
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionVariableWeightedLift::SideActionVariableWeightedLift()
    : Super()
    , m_Height(HEIGHT)
    , m_WaitTimer(0.0f)
    , m_MoveContactCountParcent(100)
    , m_ContactCount(0)
    , m_Speed(MOVE_SPEED_DEFAULT)
    , m_ReturnSpeed(MOVE_SPEED_DEFAULT)
    , m_IsEnableReturn(TB_TRUE)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionVariableWeightedLift::~SideActionVariableWeightedLift()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionVariableWeightedLift::Setup(const CrActorFactory::UserData& userData)
{

    TbRectF32 collisionRect;
    collisionRect.width = userData.AsFloat32(0);
    collisionRect.height = userData.AsFloat32(1);

    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(32.0f, 32.0f);
    spriteParam.rect = collisionRect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetName("LiftRect");
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_BACK_2);
    m_Sprite.GetSprite().SetVisible(TB_FALSE);

    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT);
    collision->SetFixedPos(TB_TRUE);
    m_Height = userData.AsFloat32(2);
    if( 3 < userData.GetCount() ){
        m_IsEnableReturn = userData.AsSint32(3);
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionVariableWeightedLift::NotifyEvent(TbUint32 event, void* param)
{
    switch (event)
    {
    case SIDE_ACTION_EVENT_SET_VISIBLE:
        if (param) {
            TbBool* visible = reinterpret_cast<TbBool*>(param);
            m_Sprite.GetSprite().SetVisible(*visible);
        }
        break;
    }
    return 0;
}


/*!
 * ロジック更新
 * @author teco
 */
void SideActionVariableWeightedLift::OnUpdateAction( TbFloat32 deltatime )
{
    TbVector2 mov;
    TbFloat32 newY = 0.0f;

    const CrActionCollision* collision = GetCollision();
    TB_RESQUE(collision);

    if( m_WaitTimer > 0.0f ) {
        m_WaitTimer -= deltatime;
    }
    if( collision->IsContacted(CR_CONTACT_DOWN) ){
        m_WaitTimer = WAIT_TIME;
    }

    m_ContactCount = collision->GetContactCount(CR_CONTACT_UP,
                                                TB_TRUE,
                                                nullptr,
                                                TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER)|
                                                TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT));

    
    if( m_WaitTimer <= 0.0f ){
        TbUint32 border = getMoveContactCount();
        TbFloat32 height = TbAbs(m_Height);
        if( m_ContactCount >= border){
            newY  = TbMin( height , TbAbs(m_OffsetPos.GetY()) + m_Speed );
        }else{
            if(m_IsEnableReturn){
                newY  = TbMax( 0.0f , TbAbs(m_OffsetPos.GetY()) - m_ReturnSpeed);
            }else {
                newY = m_OffsetPos.GetY();
            }
        }
        newY *= TbSign(m_Height);
        mov.Set( 0.0f ,  newY - m_OffsetPos.GetY() );

        if( IsEnableMoveContactActor(mov,CR_CONTACT_UP) ){
            m_OffsetPos.SetY(newY);
            SetPos( GetInitPos().GetX() + m_OffsetPos.GetX() , 
                    GetInitPos().GetY() + m_OffsetPos.GetY() );
            if( !TB_FLOAT32_EQUAL_ZERO( mov.LengthSq() ) ){
                // 上に乗っかっているアクターは連動して動かす
                MoveContactActor( mov , CR_CONTACT_UP );
            }
        }
    }
}

/*!
 * 最大押し数
 * @author teco
 * @since 2014.06.28
 */
TbUint32 SideActionVariableWeightedLift::getMoveContactCount()
{
    static const TbUint32 MIN = 2;
    SideActionStage* actionStage = TbDynamicCast<SideActionStage*>(GetStage());
    TbFloat32 maxScale = (static_cast<TbFloat32>(m_MoveContactCountParcent)/100.0f);
    TbFloat32 max = static_cast<TbFloat32>(actionStage->GetPlayerCount())*maxScale;
    TbUint32 result = static_cast<TbUint32>(TbCeil(max));
    result = TbMax(result, MIN);
    return result;
}


