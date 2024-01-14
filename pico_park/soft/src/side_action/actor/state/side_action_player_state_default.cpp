/*!
 * プレイヤー
 * @author teco
 */

#include "fwd.h"
#include "side_action/actor/state/side_action_player_state_default.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"
#include <crayon/map/cr_action_map.h>
#include <crayon/input/cr_input.h>

#include <sound/tb_sound_manager.h>

static TB_NDEBUG_CONST TbFloat32 NOTIFY_VELO_SCALE = 1.0f;
static TB_NDEBUG_CONST TbFloat32 ENABLE_JUMP_SEC = 0.07f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionPlayerStateDefault::SideActionPlayerStateDefault()
    : m_ForceJumpVelo()
    , m_JumpFrame(0)
    , m_JumpCount(0)
    , m_EnableJumpSec(0.0f)
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
    const CrActionCollision* collision = player.GetCollision();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(player.GetActionStage());
    CrActionMap& map = stage->GetMap();
    TB_RESQUE(collision);

    TbVector2 velo = player.GetVelo();
    if (m_BitArray.Test(FLAG_CATCHED))
    {
        // 掴まれたら自分からは動かない
        m_JumpFrame = 0;
        m_JumpCount = 0;
        m_ForceJumpVelo.SetY(0.0f);
        m_BitArray.SetBit(FLAG_FORCE_JUMP, TB_FALSE);
        m_BitArray.SetBit(FLAG_FORCE_JUMP_FOR_SWITCH, TB_FALSE);
        return;
    }

    // 左右移動
    if(player.IsEnableWalk()){
        velo.SetX(0.0f);
    }else{
        // どこかに着地するまで空気抵抗のみ
        velo.SetX(velo.GetX()*0.98f);
    }
    if( player.IsOnInput(CrInput::BUTTON_RIGHT) ) {
        player.LookRight();
        TbUint32 attribute = 0;
        TbUint32 isContactRight = collision->IsContacted(CR_CONTACT_RIGHT,TB_TRUE,&attribute);
        if( player.IsEnableWalk() && 0 == (attribute&SIDE_ACTION_COLLISION_ATTR_NOT_MOVE) )
        {
            TbVector2 drawPos = player.GetDrawPos();
            if (!player.IsMainMenuMode()) {
                TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, player.GetWalkVelo());
                velo.SetX(v);
            }else{
                velo.SetX(player.GetWalkVelo());
            }
        }
        player.SetAnim(isContactRight ? SideActionPlayer::ANIM_PUSH_WALK : SideActionPlayer::ANIM_WALK);
    }else if( player.IsOnInput(CrInput::BUTTON_LEFT) ) {
        player.LookLeft();
        TbUint32 attribute = 0;
        TbUint32 isContactLeft = collision->IsContacted(CR_CONTACT_LEFT, TB_TRUE, &attribute);
        if( player.IsEnableWalk() && 0 == (attribute&SIDE_ACTION_COLLISION_ATTR_NOT_MOVE))
        {
            if( !player.IsMainMenuMode()){
                TbFloat32 v = stage->CalcScreenEndWalkVelo(&player, -player.GetWalkVelo());
                // 画面左端から消えないように
                velo.SetX(v);
            }else{
                velo.SetX(-player.GetWalkVelo());
            }
        }
        player.SetAnim(isContactLeft ? SideActionPlayer::ANIM_PUSH_WALK : SideActionPlayer::ANIM_WALK);
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
    if (stage->IsAutoScroll() || !stage->IsEnableScrollNormalSpeedMax() ) 
    {
        TbVector2 drawPos = player.GetDrawPos();
        TbFloat32 speed = stage->CalcAutoScrollSpeed();
        TbFloat32 exptectedX = drawPos.GetX() + velo.GetX();
        if (exptectedX - speed < 0.0f) {
            // 画面左端から消えないように
            velo.Add(-(exptectedX-speed),0.0f);
            // 隣接しているやつも一緒に移動させる
            CrActionActor::MoveContactOption option;
            option.targetCollisionLayerMask = TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER);
            player.MoveContactActor(TbVector2(-drawPos.GetX(),0.0f),CR_CONTACT_RIGHT, option);
        }
        // 中心が消えるとゲームオーバー
        if (drawPos.GetX()+player.GetWidth()/2.0f - velo.GetX() < 0.0f) {
            TbPlaySound("hit");
            player.GetStage()->RequestScreenQuake();
            player.RequestState(SideActionPlayer::STATE_DEAD);
        }
    }

    // 自由落下
    if( player.IsFloating() ) {
        // 自由落下
        TbVector2 gravityVelo = player.CalcGravity(velo);
        velo += gravityVelo;
    }else{
        if( SideActionPlayer::ANIM_JUMP == player.GetAnim() ){
            player.SetAnim(SideActionPlayer::ANIM_DEFAULT);
            player.NotifyEvent(SIDE_ACTION_EVENT_RESET_JUMP,nullptr);
        }
        velo.SetY(0.0f);
        if( !m_BitArray.Test(FLAG_FORCE_JUMP) ){
            m_ForceJumpVelo.Clear();
        }
        player.SetEnableWalk(TB_TRUE);

        // 多少の段差は超える
        CrContactFace stepFace = CR_CONTACT_INVALID;
        if( collision->IsContacted(CR_CONTACT_RIGHT) ){
            stepFace = CR_CONTACT_RIGHT;
        }else if( collision->IsContacted(CR_CONTACT_LEFT) ){
            stepFace = CR_CONTACT_LEFT;
        }
        if( stepFace != CR_CONTACT_INVALID && !TB_FLOAT32_EQUAL_ZERO(velo.GetX()) ){
            CrActionCollision* target[CrActionCollision::CONTACT_GET_MAX];
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
        m_JumpFrame = 0;
    }

    if(updateEnableJump(player,deltatime) )
    {
        TbBool isPress = player.IsPressInput(CrInput::BUTTON_JUMP);
        if (0 < player.GetJumpContinueCount() && player.GetJumpContinueCount() <= m_JumpCount) {
            isPress = TB_FALSE;
        }
        if(isPress && !m_BitArray.Test(FLAG_FORCE_JUMP) ){
            m_EnableJumpSec = 0.0f;
            ++m_JumpCount;
            TbPlaySound("jump");
            player.SetAnim(SideActionPlayer::ANIM_JUMP);
            if (player.GetCollision()->IsContacted(CR_CONTACT_UP))
            {
                CrActionCollision* collision = player.GetCollision();
                player.NotifyJump(collision, player.GetJumpVelo()*NOTIFY_VELO_SCALE , SIDE_ACTION_EVENT_JUMP_OBJECT);
            }
            else
            {
                TbFloat32 veloY = player.GetJumpVelo();
                m_JumpFrame = 1;
                if (velo.GetY() < player.GetJumpVelo())
                {
                    veloY = velo.GetY();
                    m_JumpFrame = SIDE_ACTION_PLAYER_JUMP_COUNT_MAX;
                }
                velo.Set(0.0f, veloY);
                // 色変更
                if (player.IsEnableChangeColorWithJump()) {
                    TbUint32 playerIndex = (player.GetPlayerIndex() + 1) % Root::GetInstance().GetPlayerCount();
                    player.SetPlayerIndex(playerIndex);
                }
            }
        }else if( 0 < m_JumpFrame && m_JumpFrame < SIDE_ACTION_PLAYER_JUMP_COUNT_MAX && player.IsOnInput(CrInput::BUTTON_JUMP) ) {
            TbFloat32 scale = (1-static_cast<TbFloat32>(m_JumpFrame)/static_cast<TbFloat32>(SIDE_ACTION_PLAYER_JUMP_COUNT_MAX))*SIDE_ACTION_PLAYER_JUMP_VELO_COEF;
            velo.Add(0.0f,player.GetJumpVelo()*scale);
            ++m_JumpFrame;
        }else{
            m_JumpFrame = 0;
        }
        if( 0 == m_JumpFrame){
            if (m_BitArray.Test(FLAG_FORCE_JUMP)) {
                TbPlaySound("jump");
                velo.SetY(m_ForceJumpVelo.GetY());
                m_ForceJumpVelo.SetY(0.0f);
                player.SetAnim(SideActionPlayer::ANIM_JUMP);
                m_BitArray.SetBit(FLAG_FORCE_JUMP, TB_FALSE);
            }
        } else {
            m_BitArray.SetBit(FLAG_FORCE_JUMP, TB_FALSE);
            m_BitArray.SetBit(FLAG_FORCE_JUMP_FOR_SWITCH, TB_FALSE);
        }
    }
    else
    {
        if( m_BitArray.Test(FLAG_FORCE_JUMP) ){
            if( m_BitArray.Test(FLAG_FORCE_JUMP_FOR_SWITCH) ) {
                TbPlaySound("jump");
                velo.SetY(m_ForceJumpVelo.GetY());
                player.SetAnim(SideActionPlayer::ANIM_JUMP);
            }
            m_ForceJumpVelo.SetY(0.0f);
            m_BitArray.SetBit(FLAG_FORCE_JUMP,TB_FALSE);
            m_BitArray.SetBit(FLAG_FORCE_JUMP_FOR_SWITCH,TB_FALSE);
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

    TbFloat32 failUpY = stage->GetFailUpY();
    TbBool isFail = player.GetPosY() > stage->GetFailFallY();
    if (!isFail && failUpY < 0.0f ) {
        isFail = player.GetPosY() < failUpY;
    }
    if( isFail ) {
        // 一人でも落ちたらリトライ
        if (TB_FLOAT32_EQUAL_ZERO(m_RequestedPos.LengthSq())) {
            stage->RequestScreenQuake();
            stage->RequestRetry();
            player.RequestState(SideActionPlayer::STATE_NULL);
            TbPlaySound("hit");
        }else {
            player.ResetVelo();
            player.ResetAccel();
        }
//        player.RequestState(SideActionPlayer::STATE_NULL);
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerStateDefault::NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param )
{
    TbSint32 result = 0;
    if( SIDE_ACTION_EVENT_JUMP == event ||
        SIDE_ACTION_EVENT_JUMP_FROM_SWITCH == event ) 
    {
        if( 0 == m_JumpFrame ){
            if( param ){
                m_ForceJumpVelo = *reinterpret_cast<TbVector2*>(param);
            }else{
                m_ForceJumpVelo.Set( 0.0f ,  SIDE_ACTION_JUMP_VELO );
            }
            m_BitArray.SetBit(FLAG_FORCE_JUMP);
            if (SIDE_ACTION_EVENT_JUMP_FROM_SWITCH == event) {
                m_BitArray.SetBit(FLAG_FORCE_JUMP_FOR_SWITCH);
            }
        }
    }else if( SIDE_ACTION_EVENT_WARP == event ){
        // 瞬間移動
        if( param ){
            m_RequestedPos = *reinterpret_cast<TbVector2*>(param);
        }
        // 鍵に通知
        player.GetStage()->NotifyEvent("Key",SIDE_ACTION_EVENT_RETURN,&player);
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
    else if (SIDE_ACTION_EVENT_CATCHED == event ) {
        if (param)
        {
            TbSint32 isOn = *reinterpret_cast<TbSint32*>(param);
            m_BitArray.SetBit(FLAG_CATCHED,isOn);
            player.SetVelo(TbVector2());
            result = 1;
        }
    }
    else if(SIDE_ACTION_EVENT_SCALE == event )
    {
        if (param)
        {
            TbFloat32 scaleVelo = *reinterpret_cast<TbFloat32*>(param);
            if (0.0f < scaleVelo)
            {
                CrActionCollision* collision = player.GetCollision();
                if ( collision )
                {
                    TbRectF32 before = collision->GetGlobalRect();
                    scaleVelo = player.AddScale(scaleVelo);
                    if (0.0f < scaleVelo)
                    {
                        TbRectF32 after = collision->GetGlobalRect();
                        TbFloat32 movY = after.height - before.height;
                        TbFloat32 movX = (after.width - before.width)/2.0f;
                        CrActionActor::MoveContactOption option;
                        option.targetCollisionLayerMask = TB_BIT(SIDE_ACTION_COLLISION_LAYER_PLAYER);
                        if (TB_VERIFY(0.0f < movY))
                        {
                            player.MoveContactActor(TbVector2(0.0f, -movY),CR_CONTACT_UP, option);
                        }
                        if (TB_VERIFY(0.0f < movX))
                        {
                            player.MoveContactActor(TbVector2(movX, 0.0f), CR_CONTACT_RIGHT, option);
                            player.MoveContactActor(TbVector2(-movX, 0.0f), CR_CONTACT_LEFT, option);
                        }
                    }
                }
            }
            else
            {
                player.AddScale(scaleVelo);
            }
        }
    }
    else if( SIDE_ACTION_EVENT_SPEED_UP == event )
    {
        if (param) {
            TbFloat32 scale = *reinterpret_cast<TbFloat32*>(param);
            TbFloat32 speedScale = TbMin(2.0f,player.GetWalkSpeedScale()*scale);
            player.SetWalkSpeedScale(speedScale);
        }
    }
    return result;
}

/*! 
 * 接触開始コールバック
 * @author teco
 */
void SideActionPlayerStateDefault::OnContactAdded(SideActionPlayer& player ,const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info)
{
    if( normal.GetY() < 0.0f && player.GetVelo().GetY() < 0.0f ){
        if (info.collision && info.collision->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER ) {
            TbUint32 recursiveMax = info.collision->GetContactRecursiveMax(CR_CONTACT_UP)+1;
            TbFloat32 veloY = (player.GetVelo().GetY());
            veloY *= NOTIFY_VELO_SCALE;
            veloY /= recursiveMax;
            player.NotifyJump(info.collision, veloY, veloY, SIDE_ACTION_EVENT_JUMP_OBJECT);
        }
        if (info.map)
        {
//            TbPlaySound("head");
        }
        player.SetVeloY(0.0f);
    }
}

/*!
 * ジャンプ可能
 * @author teco
 */
TbBool SideActionPlayerStateDefault::updateEnableJump( SideActionPlayer& player , TbFloat32 deltaSec )
{
    const CrActionCollision* collision = player.GetCollision();
    TB_RESQUE_RET(collision,TB_FALSE);

    TbBool result = TB_FALSE;
    if( collision->IsContacted(CR_CONTACT_DOWN) ){
        result = TB_TRUE;
        m_EnableJumpSec = ENABLE_JUMP_SEC;
        m_JumpCount = 0;
    }else if ( 0.0f < m_EnableJumpSec ) {
        m_EnableJumpSec -= deltaSec;
        result = TB_TRUE;
    }
    if( m_JumpFrame > 0 && m_JumpFrame < SIDE_ACTION_PLAYER_JUMP_COUNT_MAX ){
        result = TB_TRUE;
    }
    if( player.IsEnableAirJump() ){
        result = TB_TRUE;
    }
    return result;
}

static TB_NDEBUG_CONST TbFloat32 PLAYER_RANGE_OFFSET = 16.0f;

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
    CrActionCollision* collision = player.GetCollision();
    TbVector2 backup = collision->GetPos();
    if( stage->IsScrollable()) {
        if (m_RequestedPos.GetX() < (stage->GetScrollX() - PLAYER_RANGE_OFFSET)) {
            // スクロール補正
            m_RequestedPos.SetX( stage->GetScrollX()+PLAYER_RANGE_OFFSET);
        }
    }
    collision->SetPos(m_RequestedPos);
    if( !collision->GetScene()->IsCollidedByPushBackTarget( *collision ) ){
        collision->ClearContactCollision();
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

// 前景描画
void SideActionPlayerStateDefault::OnDrawText(
    SideActionPlayer& player,
    const TbMatrix& poseMatrix,
    TbSpriteMaterial* material)
{
    if ( 0 < player.GetJumpContinueCount() )
    {
        TbUint32 restCount = player.GetJumpContinueCount() - m_JumpCount;
        TbVector2 drawPos = player.GetDrawPos();
        TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

        // ヒント
        fontBuffer->Printf(
            static_cast<TbSint32>(drawPos.GetX()),
            static_cast<TbSint32>(drawPos.GetY() - 96.0f), "%u" , restCount );

        fontBuffer->PopSetting();
        fontBuffer->Flush();

    }
}
