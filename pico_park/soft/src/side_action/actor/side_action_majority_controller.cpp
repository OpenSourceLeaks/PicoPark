/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_majority_controller.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <crayon/stage/cr_action_stage.h>

#include "side_action/actor/side_action_player.h"
#include "common/cmn_util.h"

static const TbFloat32 WIDTH = 40.0f+1.0f;
static const TbFloat32 HEIGHT = 20.0f+1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -WIDTH*2.0f , 0 , WIDTH*4.0f , HEIGHT*4.0f
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
    SetEnableScroll(TB_FALSE);
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
    CrStage* stage = reinterpret_cast<CrStage*>(userData);
    CrActionStage* actionStage = TbDynamicCast<CrActionStage*>(stage);
    if( TB_VERIFY(actionStage) ) {
        m_SpriteHooker.Enter( &actionStage->GetSpriteScene(0) );
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
    CrActionStage* stage = TbDynamicCast<CrActionStage*>(GetStage());
    struct CheckInfo
    {
        CrInput::Button button;
        TbRectF32        rect;
    };    
#if TB_PLATFORM_IS_NX
    static const CheckInfo CHECK_TABLE[] =
    {
        { CrInput::BUTTON_UP     , TbRectF32(-54.0f,12.0f,28.0f,16.0f) } ,
        { CrInput::BUTTON_DOWN   , TbRectF32(-54.0f,52.0f,28.0f,16.0f) } ,
        { CrInput::BUTTON_LEFT   , TbRectF32(-71.0f,27.5f,17.0f,25.0f) } ,
        { CrInput::BUTTON_RIGHT  , TbRectF32(-30.0f,27.5f,16.0f,25.0f) } ,
        { CrInput::BUTTON_JUMP   , TbRectF32(24.5f,20.0f,43.0f,40.0f) } ,
    };
#else
    static const CheckInfo CHECK_TABLE[] =
    {
        { CrInput::BUTTON_UP     , TbRectF32(-54.0f,12.5f,28.0f,16.0f) } ,
        { CrInput::BUTTON_DOWN   , TbRectF32(-54.0f,52.5f,28.0f,16.0f) } ,
        { CrInput::BUTTON_LEFT   , TbRectF32(-69.5f,28.5f,16.0f,24.0f) } ,
        { CrInput::BUTTON_RIGHT  , TbRectF32(-29.5f,28.5f,16.0f,24.0f) } ,
        { CrInput::BUTTON_JUMP   , TbRectF32(24.5f,20.5f,43.0f,40.0f) } ,
    };
#endif
    const SysMajorityInput* input = m_Player->GetMajorityInput();
    if( !TB_VERIFY(input) ){
        return;
    }
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(CHECK_TABLE); ++i )
    {
        TbFloat32 rate = input->GetCurrentInputRate(CHECK_TABLE[i].button);
        if( rate > 0.0f ){
            TbRectF32 rect = CHECK_TABLE[i].rect;
            rect.x += GetPosX();
            rect.y += GetPosY();
            rect.height *= rate;
            rect.y += CHECK_TABLE[i].rect.height - rect.height;

            TbRectF32 uvRect(CMN_TEX_UV_RECT(512 - 8, 0, 8, 8));
            TbMatrix mtx = TbMatrix::IDENTITY;
            material->Draw(rect, uvRect, CmnUtil::GetMainColor(), poseMatrix);
        }
    }
}

