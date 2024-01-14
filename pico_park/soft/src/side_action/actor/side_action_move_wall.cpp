/*!
 * 動く壁
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_move_wall.h"
#include "side_action/side_action_types.h"
#include <crayon/map/cr_action_map.h>
#include "side_action/stage/side_action_stage.h"
#include "root.h"

static TB_NDEBUG_CONST TbFloat32 MOVE_SPEED             = 2.0f;
static TB_NDEBUG_CONST TbFloat32 RETURN_SPEED           = 1.5f;
static TB_NDEBUG_CONST TbFloat32 IDLE_TIME              = 1.5f;
static TB_NDEBUG_CONST TbFloat32 RETURN_START_TIME      = 1.5f;
static TB_NDEBUG_CONST TbFloat32 MOV_WIDTH_DEFAULT      = 50.0f;
static TB_NDEBUG_CONST TbFloat32 TRIGGER_WIDTH_DEFAULT  = 50.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 1.5f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionMoveWall::SideActionMoveWall( TbFloat32 movWidth , TbFloat32 triggerWidth , TbFloat32 height )
    : Super()
    , m_State(STATE_IDLE)
    , m_StateTimer(0.0f)
    , m_MoveWidth(movWidth)
    , m_PushBackCount(0)
{
    if (height < 0.0f) {
        height = 260.0f;
    }
    const TbRectF32Pod SPRITE_RECT =
    {
        -9.0f , -height , 18.0f , height
    };
    const TbRectF32Pod SPRITE_UV_RECT =
    {
        CMN_TEX_UV_RECT(500,255,9,130)
    };
    const TbRectF32Pod COLLISION_RECT =
    {
        SPRITE_RECT.x + 1.0f , SPRITE_RECT.y + 1 , SPRITE_RECT.width - 2.0f , SPRITE_RECT.height - 2.0f
    };

    if( TB_FLOAT32_EQUAL_ZERO(m_MoveWidth) ){
        m_MoveWidth = MOV_WIDTH_DEFAULT;
    }
    if( TB_FLOAT32_EQUAL_ZERO(triggerWidth) ){
        triggerWidth = TRIGGER_WIDTH_DEFAULT;
    }
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
    collision->SetEnablePushedFromFixedPosCollision(TB_FALSE);

    // 移動開始センサー
    {
        TbFloat32 offset = ( triggerWidth < 0.0f ) ? triggerWidth : 0.0f;
        const TbRectF32Pod RAY_COLLISION_RECT = 
        {
            SPRITE_RECT.x+SPRITE_RECT.width+1.0f + offset ,
            SPRITE_RECT.y+1 , 
            TbAbs(triggerWidth) , 
            SPRITE_RECT.height-2.0f 
        };
        CrActionCollision* triggerCollision = CreateCollision( RAY_COLLISION_RECT , 0 , TB_TRUE );
        triggerCollision->SetCollidedCallback(TbCreateMemFunc(this,&Self::onCollidedCollision));
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionMoveWall::~SideActionMoveWall()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionMoveWall::OnUpdateAction( TbFloat32 deltatime )
{
    TbFloat32 movX = 0.0f;
    TbVector2 oldOffsetPos = m_OffsetPos;
    if( m_StateTimer < 100.0f ) { // 適当
        m_StateTimer += deltatime;
    }
    if( m_PushBackCount > 0 ) {
        if( m_PushBackCount <= 1 ) {
            ++m_PushBackCount;
            return;
        }else{
            m_PushBackCount = 0;
        }
    }
    switch(m_State)
    {
    case STATE_IDLE:
        {
        }
        break;
    case STATE_WAIT:
        {
            if (m_StateTimer < WAIT_TIME ) {
                break;
            }
            setState(STATE_MOVE);
        }
        break;
    case STATE_MOVE:
        {
            movX = MOVE_SPEED * TbSign(m_MoveWidth);
            m_OffsetPos.Add(movX,0.0f);
            if( TbAbs(m_OffsetPos.GetX()) > TbAbs(m_MoveWidth) ) {
                setState(STATE_RETURN);
            }
        }
        break;
    case STATE_RETURN:
        {
            if( m_StateTimer < RETURN_START_TIME ){
                break;
            }

            movX = -RETURN_SPEED * TbSign(m_MoveWidth);

            m_OffsetPos.Add(movX,0.0f);
            if( m_OffsetPos.GetX() * m_MoveWidth <= 0.0f ) {
                m_OffsetPos.SetX(0.0f);
                setState(STATE_WAIT);
            }
        }
        break;
    }

    TbVector2 mov(m_OffsetPos-oldOffsetPos);
    if( IsEnableMoveContactActor(mov,CR_CONTACT_UP) ){
        SetPos( GetInitPos().GetX() + m_OffsetPos.GetX() , 
                GetInitPos().GetY() + m_OffsetPos.GetY() );

        CrActionActor::MoveContactOption option;
        option.isEnableIgnoreTargetVelo = TB_TRUE;
        if( movX > 0.0f ){
            MoveContactActor( mov , CR_CONTACT_RIGHT , option );
        }else{
            MoveContactActor( mov , CR_CONTACT_LEFT , option);
        }
    }else{
        m_OffsetPos = oldOffsetPos;
    }
}

/*!
 * コリジョン衝突コールバック
 * @author teco
 * @since 2014.09.11
 */
void SideActionMoveWall::onCollidedCollision(CrActionCollision* collision)
{
    if( !collision->GetLayer() == SIDE_ACTION_COLLISION_LAYER_PLAYER ){
        return;
    }
    if( m_State != STATE_IDLE && m_State != STATE_WAIT){
        return;
    }
    if (m_StateTimer < IDLE_TIME ){
        return;
    }
    setState(STATE_MOVE);
}
