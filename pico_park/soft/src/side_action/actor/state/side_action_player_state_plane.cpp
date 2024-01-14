/*!
 * プレイヤー
 * @author teco
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_plane.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/actor/side_action_player_plane.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"
#include <crayon/map/cr_action_map.h>
#include <crayon/input/cr_input.h>

#include <sound/tb_sound_manager.h>

static TB_NDEBUG_CONST TbFloat32 WALK_SPEED_SCALE = 1.5f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerStatePlane::SideActionPlayerStatePlane()
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionPlayerStatePlane::~SideActionPlayerStatePlane()
{
}

/*!
 * 初期化
 * @author teco
 */
void SideActionPlayerStatePlane::Initialize(SideActionPlayer& player)
{
    player.SetAnim(SideActionPlayer::ANIM_PLANE);
    player.SetWalkSpeedScale(WALK_SPEED_SCALE);
    player.GetCollision()->SetUserDataU32(SIDE_ACTION_COLLISION_U32_DISABLE_INERTIA,TB_TRUE);
}

/*! 
 * ロジック更新
 * @author teco
 */
void SideActionPlayerStatePlane::Update( SideActionPlayer& player , TbFloat32 deltatime )
{
    const CrActionCollision* collision = player.GetCollision();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
    CrActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    TbVector2 velo = player.GetVelo();

    // 移動
    velo.Set(stage->CalcAutoScrollSpeed(),0.0f);
    if( player.IsOnInput(CrInput::BUTTON_RIGHT) ) {
        player.LookRight();
        TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, player.GetWalkVelo());
        velo.Add(v,0.0f);
    }else if( player.IsOnInput(CrInput::BUTTON_LEFT) ) {
        if( !player.GetCollision(0)->IsContacted(CR_CONTACT_LEFT) ){
            TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, -player.GetWalkVelo());
            // 画面左端から消えないように
            velo.Add(v, 0.0f);
        }
    }
    if (player.IsOnInput(CrInput::BUTTON_UP)) {
        TbFloat32 v = -player.GetWalkVelo();
        velo.Add(0.0f,v);
    }else if (player.IsOnInput(CrInput::BUTTON_DOWN)) {
        TbFloat32 v = player.GetWalkVelo();
        velo.Add(0.0f, v);
    }

    // 自動スクロール専用ルール
    // 左端に消えると死ぬ
    if (stage->IsAutoScroll()) 
    {
        TbVector2 drawPos = player.GetDrawPos();
        TbFloat32 speed = stage->CalcAutoScrollSpeed();
        TbFloat32 exptectedX = drawPos.GetX() + velo.GetX();
        if (exptectedX - speed < 0.0f) {
            // 画面左端から消えないように
            TbVector2 v(TbMax(0.0f, -drawPos.GetX()) , 0 ) ;
            velo.SetX( v.GetX() );
            // 隣接しているやつも一緒に移動させる
            CrActionActor::MoveContactOption option;
            option.targetCollisionLayerMask = TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER);
            player.MoveContactActor(TbVector2(-drawPos.GetX(),0.0f),CR_CONTACT_RIGHT, option);
        }
        // 中心が消えるとゲームオーバー
        if (drawPos.GetX()+player.GetWidth()/2.0f - velo.GetX() < 0.0f) {
            player.RequestState(SideActionPlayer::STATE_DEAD);
        }
    }
    if (stage->GetFailFallY() < player.GetPosY() || player.GetPosY() < -stage->GetWindowHeight()*0.5f ) {
        // 一人でも落ちたらリトライ
        if (TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq())) {
            stage->RequestScreenQuake();
            player.RequestState(SideActionPlayer::STATE_DEAD);
            TbPlaySound("hit");
        }
    }

#if 0
    // マップと衝突すると死亡する
    if (player.GetCollision()->IsContactedMap()) {
        player.RequestState(SideActionPlayer::STATE_DEAD);
        stage->CreateActor<SideActionPlayerPlane>(player.GetPosXY(),&player);
        velo.Clear();
    }
#endif

    player.SetVelo(velo);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerStatePlane::NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param )
{
    TbSint32 result = 0;
    if( SIDE_ACTION_EVENT_WARP == event ){
        // 瞬間移動
        if( param ){
            m_RequestedPos = *reinterpret_cast<TbVector2*>(param);
        }
    }
    else if (SIDE_ACTION_EVENT_WARP_INIT_POS == event) {
        m_RequestedPos = player.GetInitPos();
    }
    else if (SIDE_ACTION_EVENT_SET_VISIBLE == event) {
        if( param ){
            TbBool* visible = reinterpret_cast<TbBool*>(param);
            if (*visible) {
                if (TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq())) {
                    player.SetVisible(TB_TRUE);
                }else {
                    player.Resume();
                    m_BitArray.SetBit(FLAG_REQUEST_VISIBLE);
                }
            }else {
                player.SetVisible(TB_FALSE);
            }
        }
    }
    else if( SIDE_ACTION_EVENT_SPEED_UP == event )
    {
        if (param) {
            TbFloat32 scale = *reinterpret_cast<TbFloat32*>(param);
            player.SetWalkSpeedScale(player.GetWalkSpeedScale()*scale);
        }
    }
    else if (SIDE_ACTION_EVENT_DEAD == event) {
        if ( 0 < player.GetPreLife() )
        {
            SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
            if( !player.IsImmoral() && !stage->IsPlayerUndead() )
            {
                stage->CreateActor<SideActionPlayerPlane>(player.GetPosXY(), &player);
                player.SetVelo(TbVector2::ZERO);
            }
        }
    }
    return result;
}

/*!
 * 後処理
 * @author teco
 */
void SideActionPlayerStatePlane::Post( SideActionPlayer& player , TbFloat32 deltatime )
{
    if( TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq()) ){
        return;
    }
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
    CrActionCollision* collision = player.GetCollision();
    TbVector2 backup = collision->GetPos();
    if( stage->IsScrollable()) {
        if (m_RequestedPos.GetX() < stage->GetScrollX()) {
            // スクロール補正
            m_RequestedPos.SetX( stage->GetScrollX() );
        }
    }
    collision->SetPos(m_RequestedPos);
    if( !collision->GetScene()->IsCollidedByPushBackTarget( *collision ) ){
        player.SetPosForce(m_RequestedPos);
        player.ResetVelo();
        m_RequestedPos.Clear();
        if (m_BitArray.Test(FLAG_REQUEST_VISIBLE)) {
            player.SetVisible(TB_TRUE);
            m_BitArray.ResetBit(FLAG_REQUEST_VISIBLE);
        }
    }else{
        collision->SetPos(backup);
    }
}
