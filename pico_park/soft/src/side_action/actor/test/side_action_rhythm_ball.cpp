/*!
 * リズムボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_rhythm_ball.h"
#include <crayon/stage/cr_stage.h>

namespace
{
    static const TbRectF32Pod SPRITE_RECT = 
    {
        -30.0f , -30.0f , 60.0f , 60.0f
    };
}

static TB_NDEBUG_CONST TbFloat32 INTERVAL_SEC = 1.0f;
static TB_NDEBUG_CONST TbFloat32 HEIGHT = -200.0f;

static TB_NDEBUG_CONST TbFloat32 GRAVITY = 200.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionRhythmBall::SideActionRhythmBall()
    : Super()
    , m_Timer(0.0f)
{
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0x00,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    m_SpriteNode.Initialize(initParam);
    m_Sprite.Initialize(&m_SpriteNode);
//    SetCollisionRect( SPRITE_RECT );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionRhythmBall::~SideActionRhythmBall()
{
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionRhythmBall::OnUpdate( TbFloat32 deltatime )
{
    m_Timer += deltatime;
    if( m_Timer > INTERVAL_SEC ) {
        m_Timer -= INTERVAL_SEC;
    }
#if 0
    TbFloat32 rate = m_Timer/INTERVAL_SEC;
    TbFloat32 y  = TbSin(rate*TB_PI)*HEIGHT;
#else
    TbFloat32 v = -GRAVITY*INTERVAL_SEC/2;
    TbFloat32 y = v*m_Timer+GRAVITY*m_Timer*m_Timer/2.0f;
#endif
    m_OffsetPos.SetY( y );
}

/*!
 * 描画更新
 * @author teco
 * @since 2013.09.16
 */
void SideActionRhythmBall::OnDraw( TbFloat32 deltatime )
{
    m_Sprite.SetTranslate(GetPosX()+m_OffsetPos.GetX(),
                          GetPosY()+m_OffsetPos.GetY());
}

/*!
 * シーンに入った
 * @author teco
 * @since 2013.09.08
 */
void SideActionRhythmBall::OnEnter( void* userData )
{
    CrStage* stage = reinterpret_cast<CrStage*>(userData);
    if( TB_VERIFY(stage) ) {
        m_Sprite.Enter( &stage->GetSpriteScene(0) );
    }
}

/*!
 * シーンから出た
 * @author teco
 * @since 2013.09.08
 */
void SideActionRhythmBall::OnExit()
{
    m_Sprite.Exit();
}
