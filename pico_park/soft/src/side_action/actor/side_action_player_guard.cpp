/*!
 * プレイヤー用磁石
 * @author teco
 */

#include "fwd.h"
#include "side_action_player_guard.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/map/cr_action_map.h>
#include "common/cmn_util.h"
#include "root.h"


//////////////////////////////////////////
// SideActionPlayerGuard
//////////////////////////////////////////

static TbFloat32 WIDTH[] = 
{
    6.0f,
    48.0f
};
static TbFloat32 HEIGHT[] = {
    60.0f,
    6.0f
};
static const TbRectF32Pod SPRITE_RECT[] =
{
    { -WIDTH[0]*0.5f , -HEIGHT[0]*0.5f , WIDTH[0] , HEIGHT[0] } ,
    { -WIDTH[1]*0.5f , -HEIGHT[1] * 0.5f , WIDTH[1] , HEIGHT[1] } ,
};
static const TbRectF32Pod SPRITE_UV_RECT[] =
{
    {CMN_TEX_UV_RECT(192,480,3,30)},
    {CMN_TEX_UV_RECT(160,480,24,3)}
};

static const TbVector2 OFFSET_POS[] =
{
    {  0.0f  , -56.0f } ,
    {  0.0f  ,  10.0f } ,
    { -28.0f  , -24.0f } ,
    {  30.0f  , -24.0f } ,
};

/////////////////////////////
// SideActionPlayerGuard
/////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerGuard::SideActionPlayerGuard( SideActionPlayer* player, CmnDir dir )
    : Super()
    , m_Player(player)
    , m_Dir(dir)
{
    TbUint32 spriteIndex = getSpriteIndex();
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT[spriteIndex], SPRITE_UV_RECT[spriteIndex]);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    sprite.SetMaterial(&m_Material);
    m_Material.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));

    // コリジョン設定
    TbUint32 pushBackTarget = 0;
    CrActionCollision* collision = CreateCollision(SPRITE_RECT[spriteIndex], pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
    collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    m_Player->Retain();

    // アクターレイヤー設定
    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerGuard::~SideActionPlayerGuard()
{
    TB_SAFE_RELEASE(m_Player);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerGuard::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_Player->GetPlayerIndex();
    }
    return 0;
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionPlayerGuard::OnUpdateAction(TbFloat32 deltatime)
{
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlayerGuard::OnPostAction( TbFloat32 deltatime )
{
    GetMainSprite().SetVisible(m_Player->IsVisible());
    TbVector4 pos = m_Player->GetPos();
    CrActionCollision* collision = GetCollision();

    pos.Add(OFFSET_POS[m_Dir].GetX(), OFFSET_POS[m_Dir].GetY(), 0.0f);
    if( SaveFile::GetInstance().IsVisiblePlayerNumber() && m_Dir == CMN_DIR_UP )
    {
        pos.Add(0.0f,-10.0f,0.0f);
    }

    // スプライト反映
    TbSprite& sprite = GetMainSprite();
    SetPos(pos);
    sprite.SetTranslate(GetDrawPos());
}

/*! 
 * 衝突時コールバック
 * @author teco
 */
void SideActionPlayerGuard::onCollided(CrActionCollision* candidate)
{
}
