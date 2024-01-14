/*!
 * ブロック崩し用バー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "breakout_bar.h"
#include "root.h"

static TbFloat32 RADIUS = 25.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -RADIUS , -RADIUS , RADIUS*2.0f , RADIUS*2.0f
};

static const TbCircleF32 COLLISION_CIRCLE( 0.0f , 0.0f , RADIUS );

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(288,480,RADIUS-1,RADIUS-1),
};

static TB_NDEBUG_CONST TbFloat32 SPEED = 5.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
BreakoutBar::BreakoutBar( TbUint32 playerIndex )
    : Super()
    , m_PlayerIndex(playerIndex)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    initParam.param.uvRect = SPRITE_UV_RECT; //
    initParam.param.uvRect.x += CMN_TEX_UV(RADIUS-1) * playerIndex;
    m_SpriteNode.Initialize(initParam);
    sprite.Initialize(&m_SpriteNode);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_CIRCLE , pushBackTarget , TB_TRUE );

    SetWeight(WEIGHT_MAX);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
BreakoutBar::~BreakoutBar()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void BreakoutBar::OnUpdateAction( TbFloat32 deltatime )
{
    // アタック

    // 上下
    SetVeloY(0.0f);
    if ( isOnInput(SysInput::BUTTON_UP) ){
        SetVeloY(-SPEED);
    }else if ( isOnInput(SysInput::BUTTON_DOWN) ){
        SetVeloY(SPEED);
    }

    // 左右
    SetVeloX(0.0f);
    if ( isOnInput(SysInput::BUTTON_LEFT) ){
        SetVeloX(-SPEED);
    }else if ( isOnInput(SysInput::BUTTON_RIGHT) ){
        SetVeloX(SPEED);
    }

    // 移動制限
}

/*!
 * 入力し続けている
 * @author teco
 * @since 2014.07.27
 */
TbBool BreakoutBar::isOnInput( SysInput::Button button ) const
{
    TbBool result = TB_FALSE;
    result = SysInput::IsOn(button,m_PlayerIndex);
    return result;
}

/*!
 * 押した
 * @author teco
 * @since 2014.07.27
 */
TbBool BreakoutBar::isPressInput( SysInput::Button button ) const
{
    TbBool result = TB_FALSE;
    result = SysInput::IsPress(button,m_PlayerIndex);
    return result;
}

/*!
 * 離した
 * @author teco
 * @since 2014.07.27
 */
TbBool BreakoutBar::isReleaseInput( SysInput::Button button ) const
{
    TbBool result = TB_FALSE;
    result = SysInput::IsRelease(button,m_PlayerIndex);
    return result;
}
