/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_majority_controller.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/sprite/tb_sprite_material.h>

#include "side_action/actor/side_action_player.h"
#include "common/stage/action_stage.h"

static const TbFloat32 WIDTH = 40.0f+1.0f;
static const TbFloat32 HEIGHT = 20.0f+1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -0 , 0 , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(224,320,WIDTH,HEIGHT)
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionMajorityController::SideActionMajorityController( SideActionPlayer* player )
    : Super()
    , m_Player(player)
{
    m_Player->Retain();

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);

    m_SpriteHooker.SetDrawFunc(TbCreateMemFunc(this,&Self::OnDrawRect));
    m_SpriteHooker.SetZ(CMN_SPRITE_Z_FRONT_6);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionMajorityController::~SideActionMajorityController()
{
    m_Player->Release();
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionMajorityController::OnUpdateAction( TbFloat32 deltatime )
{
}

/*! 
 * シーンに入った
 * @author teco
 * @since 2014.08.05
 */
void SideActionMajorityController::OnEnter( void* userData )
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
 * @since 2014.08.05
 */
void SideActionMajorityController::OnExit()
{
    m_SpriteHooker.Exit();
    Super::OnExit();
}


/*!
 * 前景描画
 * @author teco
 * @since 2014.08.05
 */
void SideActionMajorityController::OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    struct CheckInfo
    {
        SysInput::Button button;
        TbRectF32        rect;
    };
    static const CheckInfo CHECK_TABLE[] =
    {
        { SysInput::BUTTON_UP     , TbRectF32(31.0f,17.0f,21.0f,16.0f) } , 
        { SysInput::BUTTON_DOWN   , TbRectF32(31.0f,49.0f,21.0f,16.0f) } , 
        { SysInput::BUTTON_LEFT   , TbRectF32(12.0f,33.0f,21.0f,16.0f) } , 
        { SysInput::BUTTON_RIGHT  , TbRectF32(49.0f,33.0f,21.0f,16.0f) } ,
        { SysInput::BUTTON_JUMP , TbRectF32(112.5f,24.1f,32.0f,32.0f) } ,
    };
    const SysMajorityInput* input = m_Player->GetMajorityInput();
    if( !TB_VERIFY(input) ){
        return;
    }

    material->EndDraw();

    TbDraw2dBegin();

    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(CHECK_TABLE); ++i )
    {
        TbFloat32 rate = input->GetCurrentInputRate(CHECK_TABLE[i].button);
        if( rate > 0.0f ){
            TbRectF32 rect = CHECK_TABLE[i].rect;
            rect.x += GetPosX();
            rect.y += GetPosY();
            rect.height *= rate;
            rect.y += CHECK_TABLE[i].rect.height - rect.height;
            TbDrawRectParam param;
            param.SetRect( rect );
            param.SetColor( TbColor::Black() );
            TbDrawRect( param );
        }
    }

    TbDraw2dEnd();

    material->BeginDraw();
}

