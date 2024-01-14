/*!
 * プレイヤー
 * @author teco
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_default.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "common/map/action_map.h"
#include "system/sys_input.h"

#include <sound/tb_sound_manager.h>

static TB_NDEBUG_CONST TbUint32  JUMP_COUNT_MAX = 14;
static TB_NDEBUG_CONST TbFloat32 JUMP_VELO_COEF = 0.2f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerStateDefault::SideActionPlayerStateDefault()
    : m_ForceJumpVelo()
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerStateDefault::~SideActionPlayerStateDefault()
{
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlayerStateDefault::Update( SideActionPlayer& player , TbFloat32 deltatime )
{
    const ActionCollision* collision = player.GetCollision();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
    ActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    TbVector2 velo = player.GetVelo();

    // 左右移動
    velo.SetX(0.0f);
    if( player.IsOnInput(SysInput::BUTTON_RIGHT) ) {
        player.LookRight();
        if( player.IsEnableAirWalk() )
        {
            TbVector2 drawPos = player.GetDrawPos();
            if (!player.IsMainMenuMode()) {
                TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, player.GetWalkVelo());
                velo.SetX(v);
            }else{
                velo.SetX(player.GetWalkVelo());
            }
        }
        player.SetAnim( collision->IsContacted(CMN_CONTACT_RIGHT) ? SideActionPlayer::ANIM_PUSH_WALK : SideActionPlayer::ANIM_WALK);
    }else if( player.IsOnInput(SysInput::BUTTON_LEFT) ) {
        player.LookLeft();
        if( player.IsEnableAirWalk() )
        {
            if( !player.IsMainMenuMode()){
                TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, -player.GetWalkVelo());
                // 画面左端から消えないように
                velo.SetX(v);
            }else{
                velo.SetX(-player.GetWalkVelo());
            }
        }
        player.SetAnim( collision->IsContacted(CMN_CONTACT_LEFT) ? SideActionPlayer::ANIM_PUSH_WALK : SideActionPlayer::ANIM_WALK);
    }else{
        if( player.IsSleeping() ) {
            if(  player.GetAnim() != SideActionPlayer::ANIM_SLEEP ) {
                player.SetAnim(SideActionPlayer::ANIM_SLEEP);
            }
        }else{
            if( player.GetAnim() == SideActionPlayer::ANIM_WALK || 
                player.GetAnim() == SideActionPlayer::ANIM_PUSH_WALK ||
                player.GetAnim() == SideActionPlayer::ANIM_SLEEP )
            {
                player.SetAnim(SideActionPlayer::ANIM_DEFAULT);
            }
        }
    }

    // 自動スクロール専用ルール
    // 左端に消えると死ぬ
    if (stage->IsAutoScroll()) 
    {
        TbVector2 drawPos = player.GetDrawPos();
        TbFloat32 speed = stage->GetAutoScrollSpeed();
        TbFloat32 exptectedX = drawPos.GetX() + velo.GetX();
        if (exptectedX - speed < 0.0f) {
            // 画面左端から消えないように
            TbVector2 v(TbMax(0.0f, -drawPos.GetX()) , 0 ) ;
            velo.SetX( v.GetX() );
            // 隣接しているやつも一緒に移動させる
            ActionActor::MoveContactOption option;
            option.targetCollisionLayerMask = TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER);
            player.MoveContactActor(TbVector2(-drawPos.GetX(),0.0f),CMN_CONTACT_RIGHT, option);
        }
        // 中心が消えるとゲームオーバー
        if (drawPos.GetX()+player.GetWidth()/2.0f - velo.GetX() < 0.0f) {
            player.RequestState(SideActionPlayer::STATE_DEAD);
        }
    }

    // 自由落下
    if( player.IsFloating() ) {
        // 自由落下
//        velo.Add(0.0f,SIDE_ACTION_GRAVITY);
        velo.SetY( TbMin( SIDE_ACTION_GRAVITY_MAX , velo.GetY() + SIDE_ACTION_GRAVITY ) );
    }else{
        if( SideActionPlayer::ANIM_JUMP == player.GetAnim() ){
            player.SetAnim(SideActionPlayer::ANIM_DEFAULT);
            player.NotifyEvent(SIDE_ACTION_EVENT_RESET_JUMP,nullptr);
        }
        velo.SetY(0.0f);
        if( !m_BitArray.Test(FLAG_FORCE_JUMP) ){
            m_ForceJumpVelo.Clear();
        }
        player.SetEnableAirWalk(TB_TRUE);

        // 多少の段差は超える
        CmnContactFace stepFace = CMN_CONTACT_INVALID;
        if( collision->IsContacted(CMN_CONTACT_RIGHT) ){
            stepFace = CMN_CONTACT_RIGHT;
        }else if( collision->IsContacted(CMN_CONTACT_LEFT) ){
            stepFace = CMN_CONTACT_LEFT;
        }
        if( stepFace != CMN_CONTACT_INVALID && !TB_FLOAT32_EQUAL_ZERO(velo.GetX()) ){
            ActionCollision* target[ActionCollision::CONTACT_GET_MAX];
            TbUint32 count = collision->GetContactCollision(target,stepFace);
            TbFloat32 highestY = 0.0f;
            for( TbUint32 i = 0; i < count; ++i ){
                TbRectF32 rect = target[i]->GetGlobalRect();
                if( i == 0 || rect.y < highestY ) {
                    highestY = rect.y;
                }
            }
            TbRectF32 myRect =  collision->GetGlobalRect();
            TbFloat32 delta = (myRect.y+myRect.height) - highestY;
            if( delta < 0.5f ) {
                velo.SetY(-delta-0.01f);
            }
        }
    }

    // ジャンプ
    if( velo.GetY() >= 0.0f ) {
        m_JumpCount = 0;
    }
    if( isEnableJump(player) )
    {
        if( m_BitArray.Test(FLAG_FORCE_JUMP) ){
            velo.SetY(m_ForceJumpVelo.GetY());
            m_ForceJumpVelo.SetY(0.0f);
            m_BitArray.SetBit(FLAG_FORCE_JUMP,TB_FALSE);
            player.SetAnim(SideActionPlayer::ANIM_JUMP);
        }else if( player.IsPressInput(SysInput::BUTTON_JUMP) ){
            TbPlaySound("jump");
            velo.Set(0.0f,player.GetJumpVelo());
            player.SetAnim(SideActionPlayer::ANIM_JUMP);
            m_JumpCount=1;
        }else if( 0 < m_JumpCount && m_JumpCount < JUMP_COUNT_MAX && player.IsOnInput(SysInput::BUTTON_JUMP) ) {
            TbFloat32 scale = (1-static_cast<TbFloat32>(m_JumpCount)/static_cast<TbFloat32>(JUMP_COUNT_MAX))*JUMP_VELO_COEF;
            velo.Add(0.0f,player.GetJumpVelo()*scale);
            ++m_JumpCount;
        }else{
            m_JumpCount = 0;
        }
    }
    else
    {
        if( m_BitArray.Test(FLAG_FORCE_JUMP) ){
            m_ForceJumpVelo.SetY(0.0f);
            m_BitArray.SetBit(FLAG_FORCE_JUMP,TB_FALSE);
        }
    }
    if( !TB_FLOAT32_EQUAL_ZERO(m_ForceJumpVelo.GetX()) ){
        velo.SetX( m_ForceJumpVelo.GetX() );
    }

    // 風の影響
    {
        velo += player.GetWind();
        player.ResetWind();
    }

    player.SetVelo(velo);

    if( player.GetPosY() > CMN_WINDOW_HEIGHT*5 ) {
        // 一人でも落ちたらリトライ
        player.GetStage()->RequestRetry();
//        player.RequestState(SideActionPlayer::STATE_NULL);
    }
}

/*!
 * イベント通知
 * @author teco
 */
void SideActionPlayerStateDefault::NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_JUMP == event ) {
        if( 0 == m_JumpCount ){
            if( param ){
                m_ForceJumpVelo = *reinterpret_cast<TbVector2*>(param);
            }else{
                m_ForceJumpVelo.Set( 0.0f ,  SIDE_ACTION_JUMP_VELO );
            }
            m_BitArray.SetBit(FLAG_FORCE_JUMP);
        }
    }else if( SIDE_ACTION_EVENT_WARP == event ){
        // 瞬間移動
        if( param ){
            m_RequestedPos = *reinterpret_cast<TbVector2*>(param);
        }
    }
    else if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        m_RequestedPos = player.GetInitPos();
    }
}

/*! 
 * 接触開始コールバック
 * @author teco
 */
void SideActionPlayerStateDefault::OnContactAdded(SideActionPlayer& player ,const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& info)
{
    if( normal.GetY() < 0.0f && player.GetVelo().GetY() < 0.0f ){
        player.SetVeloY(0.0f);
    }
}

/*!
 * ジャンプ可能
 * @author teco
 */
TbBool SideActionPlayerStateDefault::isEnableJump( SideActionPlayer& player ) const
{
    const ActionCollision* collision = player.GetCollision();
    TB_RESQUE_RET(collision,TB_FALSE);

    TbBool result = TB_FALSE;
    if( !collision->IsContacted(CMN_CONTACT_UP) && collision->IsContacted(CMN_CONTACT_DOWN) ){
        result = TB_TRUE;
    }
    if( m_JumpCount > 0 && m_JumpCount < JUMP_COUNT_MAX ){
        result = TB_TRUE;
    }
    if( player.IsEnableAirJump() ){
        result = TB_TRUE;
    }
    return result;
}

/*!
 * 後処理
 * @author teco
 */
void SideActionPlayerStateDefault::Post( SideActionPlayer& player , TbFloat32 deltatime )
{
    if( TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq()) ){
        return;
    }
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
    ActionCollision* collision = player.GetCollision();
    TbVector2 backup = collision->GetPos();
    if( stage->IsScrollable()) {
        if (m_RequestedPos.GetX() < stage->GetScroll()) {
            // スクロール補正
            m_RequestedPos.SetX( stage->GetScroll() );
        }
    }
    collision->SetPos(m_RequestedPos);
    if( !collision->GetScene()->IsCollidedByPushBackTarget( *collision ) ){
        player.SetPosForce(m_RequestedPos);
        player.ResetVelo();
        m_RequestedPos.Clear();
    }else{
        collision->SetPos(backup);
    }
}
