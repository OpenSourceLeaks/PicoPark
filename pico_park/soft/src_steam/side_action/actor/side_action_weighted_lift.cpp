/*!
 * 重さで動く台
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_weighted_lift.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -83.0f , 0.0f , 196.0f , 84.0f
};
static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(351,351,98,42)
};
static const TbRectF32Pod SUB_SPRITE_RECT = 
{
    -44.0f , -2.0f , 26.0f , 50.0f
};
static const TbRectF32Pod SUB_SPRITE_UV_RECT_UP = 
{
    CMN_TEX_UV_RECT(351,127,13,25)
};
static const TbRectF32Pod SUB_SPRITE_UV_RECT_DOWN = 
{
    SUB_SPRITE_UV_RECT_UP.x , SUB_SPRITE_UV_RECT_UP.y + SUB_SPRITE_UV_RECT_UP.height ,
    SUB_SPRITE_UV_RECT_UP.width , -SUB_SPRITE_UV_RECT_UP.height
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1+66.0f , SPRITE_RECT.width-2.0f , 20.0f-2.0f 
};

static TB_NDEBUG_CONST TbFloat32 MOVE_SPEED_DEFAULT = 1.0f;
static TB_NDEBUG_CONST TbFloat32 HEIGHT = -70.0f;
static TB_NDEBUG_CONST TbFloat32 WAIT_TIME = 0.06f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWeightedLift::SideActionWeightedLift()
    : Super()
    , m_Height(HEIGHT)
    , m_WaitTimer(0.0f)
    , m_MoveContactCount(1)
    , m_ContactCount(0)
    , m_Speed(MOVE_SPEED_DEFAULT)
{
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_PRE);
    AddComponent(&m_DirSPrite);

    // スプライト
    {
        // 土台
        TbSprite& sprite = GetMainSprite();
        sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
        sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

        // 矢印
        TbSprite& subSprite = m_DirSPrite.GetSprite();
        subSprite.Initialize(SUB_SPRITE_RECT,SUB_SPRITE_UV_RECT_UP);
        subSprite.SetZ(CMN_SPRITE_Z_FRONT_1);

        // 文字
        m_SpriteHooker.SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawCount));
        m_SpriteHooker.SetZ(CMN_SPRITE_Z_FRONT_2);
    }

    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetFixedPos(TB_TRUE);

    SetMoveContactCountParcent(20);
    
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionWeightedLift::~SideActionWeightedLift()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionWeightedLift::OnUpdateAction( TbFloat32 deltatime )
{
    TbVector2 mov;
    TbFloat32 newY = 0.0f;

    const ActionCollision* collision = GetCollision();
    TB_RESQUE(collision);

    if( m_WaitTimer > 0.0f ) {
        m_WaitTimer -= deltatime;
    }
    if( collision->IsContacted(CMN_CONTACT_DOWN) ){
        m_WaitTimer = WAIT_TIME;
    }

    m_ContactCount = collision->GetContactCount(CMN_CONTACT_UP,
                                                TB_TRUE,
                                                nullptr,
                                                TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER));
    if( m_WaitTimer <= 0.0f ){

        TbFloat32 height = TbAbs(m_Height);
        if( m_ContactCount >= m_MoveContactCount ){
            newY  = TbMin( height , TbAbs(m_OffsetPos.GetY()) + m_Speed );
        }else{
            newY  = TbMax( 0.0f , TbAbs(m_OffsetPos.GetY()) - m_Speed );
        }
        newY *= TbSign(m_Height);
        mov.Set( 0.0f ,  newY - m_OffsetPos.GetY() );

        if( IsEnableMoveContactActor(mov,CMN_CONTACT_UP) ){
            m_OffsetPos.SetY(newY);
            SetPos( GetInitPos().GetX() + m_OffsetPos.GetX() , 
                    GetInitPos().GetY() + m_OffsetPos.GetY() );
            if( !TB_FLOAT32_EQUAL_ZERO( mov.LengthSq() ) ){
                // 上に乗っかっているアクターは連動して動かす
                MoveContactActor( mov , CMN_CONTACT_UP );
            }
        }else{
            int a = 0;
        }
    }
}

/*!
 * シーンに入った
 * @author teco
 * @since 2014.09.12
 */
void SideActionWeightedLift::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    Stage* stage = reinterpret_cast<Stage*>(userData);
    ActionStage* actionStage = TbDynamicCast<ActionStage*>(stage);
    if( TB_VERIFY(actionStage) ) {
        m_SpriteHooker.Enter( &actionStage->GetSpriteScene() );
    }
}

/*!
 * シーンから出た
 * @author teco
 * @since 2014.09.12
 */
void SideActionWeightedLift::OnExit()
{
    m_SpriteHooker.Exit();
    Super::OnExit();
}

/*!
 * 高さ設定
 * @author teco
 * @since 2014.11.21
 */
void SideActionWeightedLift::SetHeight( TbFloat32 height )
{
    m_Height = height;
    TbSprite& sprite = m_DirSPrite.GetSprite();
    if( m_Height < 0.0f ) {
        sprite.Initialize(SUB_SPRITE_RECT,SUB_SPRITE_UV_RECT_UP);
    }else{
        sprite.Initialize(SUB_SPRITE_RECT,SUB_SPRITE_UV_RECT_DOWN);
    }
}

/*!
 * 最大押し数
 * @author teco
 * @since 2014.06.28
 */
void SideActionWeightedLift::SetMoveContactCountParcent( TbUint32 parcent )
{
    static const TbUint32 MIN = 2;
    TbFloat32 maxScale = (static_cast<TbFloat32>(parcent)/100.0f);
    TbFloat32 max = static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount())*maxScale;
    m_MoveContactCount = static_cast<TbUint32>(TbCeil(max));
    m_MoveContactCount = TbMax(m_MoveContactCount,MIN);
}

/*!
 * 前景描画
 * @author teco
 * @since 2014.09.12
 */
void SideActionWeightedLift::onDrawCount( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    TbSint32 count = static_cast<TbSint32>(m_MoveContactCount) - 
                     static_cast<TbSint32>(m_ContactCount);
    count = TbMax<TbSint32>( 0 , count );

    TbFontBuffer2d*  font = Root::GetInstance().GetLocalFont();
    font->PushSetting();
    font->SetFontSize(32);
    font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    TbFloat32 x = GetDrawPos().GetX()+15.0f;
    TbFloat32 y = GetDrawPos().GetY() +7.0f;
    font->Printf( static_cast<TbSint32>(x) , 
                  static_cast<TbSint32>(y) , 
                  "%d" , count );
    font->PopSetting();

    font->Flush();
}

