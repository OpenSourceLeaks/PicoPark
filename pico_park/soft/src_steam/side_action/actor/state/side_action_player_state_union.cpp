/*!
 * プレイヤー
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_player.h"
#include "common/stage/stage.h"
#include "common/map/action_map.h"
#include "system/sys_input.h"
#include <base/math/tb_math_util.h>
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -10.0f , -10.0f , 20.0f , 20.0f
};

static TB_NDEBUG_CONST TbFloat32 GRAVITY = 0.65f;
static TB_NDEBUG_CONST TbFloat32 JUMP_VELO = -9.0f;
static TB_NDEBUG_CONST TbFloat32 SPEED_X = 3.0f;

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.02
 */
SideActionPlayer::SideActionPlayer()
    : Super()
    , m_PlayerIndex(0)
    , m_JumpVelo(JUMP_VELO)
{
    TbSpriteNodeTree::InitNodeParam initParam;
    initParam.name = "root";
    initParam.param.color.SetColorU8(0x00,0x00,0xFF,0xFF);
    initParam.param.rect = SPRITE_RECT; //
    m_SpriteNode.Initialize(initParam);
    m_Sprite.Initialize(&m_SpriteNode);
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_OBJECT;
    InitializeCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.02
 */
SideActionPlayer::~SideActionPlayer()
{
}

/*! 
 * ロジック更新
 * @author Miyake Shunsuke
 * @since 2013.09.08
 */
void SideActionPlayer::OnUpdateAction( TbFloat32 deltatime )
{
    const ActionCollision* collision = GetCollision();
    ActionMap* map = GetMap();
    TB_RESQUE(collision && map);

    // 左右移動
    TbVector2 add;
    if( SysInput::IsOn(SysInput::BUTTON_RIGHT,m_PlayerIndex) ) {
        add.SetX(SPEED_X);
    }else if( SysInput::IsOn(SysInput::BUTTON_LEFT,m_PlayerIndex) ) {
        add.SetX(-SPEED_X);
    }

    AddPos(add);
    // 自由落下
    if( !collision->IsContacted(CMN_CONTACT_DOWN) && 
        !map->IsContacted(collision,CMN_CONTACT_DOWN) )
    {
        AddVelo(0.0f,SIDE_ACTION_GRAVITY);
    }else{
        ResetVelo();
    }

    // ジャンプ
    if( SysInput::IsPress(SysInput::BUTTON_JUMP,m_PlayerIndex) &&
        !collision->IsContacted(CMN_CONTACT_UP) && 
        ( collision->IsContacted(CMN_CONTACT_DOWN) || 
         map->IsContacted(collision,CMN_CONTACT_DOWN) ) )
    {
        AddVelo(0.0f,m_JumpVelo);
    }
}

/*!
 * 描画更新
 * @author Miyake Shunsuke
 * @since 2013.09.16
 */
void SideActionPlayer::OnDraw( TbFloat32 deltatime )
{
    m_Sprite.SetTranslate(GetX(),GetY());
}

/*!
 * シーンに入った
 * @author Miyake Shunsuke
 * @since 2013.09.08
 */
void SideActionPlayer::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    Stage* stage = reinterpret_cast<Stage*>(userData);
    if( TB_VERIFY(stage) ) {
        m_Sprite.Enter( &stage->GetSpriteScene() );
    }
}

/*!
 * シーンから出た
 * @author Miyake Shunsuke
 * @since 2013.09.08
 */
void SideActionPlayer::OnExit()
{
    m_Sprite.Exit();
    Super::OnExit();
}


