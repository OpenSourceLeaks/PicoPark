/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "side_action/stage/side_action_actor_manager.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/actor/side_action_player_dress.h"
#include "side_action/actor/side_action_big_box.h"
#include "side_action/actor/side_action_color_box.h"
#include "side_action/actor/side_action_color_block.h"
#include "side_action/actor/side_action_fall_box.h"
#include "side_action/actor/side_action_key.h"
#include "side_action/actor/side_action_move_wall.h"
#include "side_action/actor/side_action_step_enemy.h"
#include "side_action/actor/side_action_bowwow_enemy.h"
#include "side_action/actor/side_action_updown_enemy.h"
#include "side_action/actor/side_action_updown_lift.h"
#include "side_action/actor/side_action_lift.h"
#include "side_action/actor/side_action_check_point.h"
#include "side_action/actor/side_action_majority_controller.h"
#include "side_action/actor/side_action_weighted_lift.h"
#include "side_action/actor/side_action_variable_weighted_lift.h"
#include "side_action/actor/side_action_jump_stand.h"
#include "side_action/actor/side_action_jump_area.h"
#include "side_action/actor/side_action_bound_ball.h"
#include "side_action/actor/side_action_ball_pitcher.h"
#include "side_action/actor/side_action_key_box.h"
#include "side_action/actor/side_action_key_proxy_goal.h"
#include "side_action/actor/side_action_warp.h"
#include "side_action/actor/side_action_link.h"
#include "side_action/actor/side_action_switch.h"
#include "side_action/actor/side_action_dead_switch.h"
#include "side_action/actor/side_action_physics_ball.h"
#include "side_action/actor/side_action_physics_object.h"
#include "side_action/actor/side_action_seesaw.h"
#include "side_action/actor/side_action_timer.h"
#include "side_action/actor/side_action_watch.h"
#include "side_action/actor/side_action_coin.h"
#include "side_action/actor/side_action_marker.h"
#include "side_action/actor/side_action_title.h"
#include "side_action/actor/side_action_text.h"
#include "side_action/actor/side_action_balance.h"
#include "side_action/actor/side_action_judge_height.h"
#include "side_action/actor/side_action_judge_territory.h"
#include "side_action/actor/side_action_judge_breakout.h"
#include "side_action/actor/side_action_judge_stop_watch.h"
#include "side_action/actor/side_action_judge_battle_jump.h"
#include "side_action/actor/side_action_ghost.h"
#include "side_action/actor/side_action_map_rect.h"
#include "side_action/actor/side_action_collision_switch.h"
#include "side_action/actor/side_action_blink_block.h"
#include "side_action/actor/side_action_wind.h"
#include "side_action/actor/side_action_stop_watch.h"
#include "side_action/actor/side_action_coop_stop_watch.h"
#include "side_action/actor/side_action_distance_constraint.h"
#include "side_action/actor/side_action_tetris.h"
#include "side_action/actor/side_action_endless_tetris.h"
#include "side_action/actor/side_action_roulette.h"
#include "side_action/actor/side_action_traffic_light.h"
#include "side_action/actor/side_action_breakout_battle_ball.h"
#include "side_action/actor/side_action_rect.h"
#include "side_action/actor/side_action_player_gun.h"
#include "side_action/actor/side_action_player_magnet.h"
#include "side_action/actor/side_action_player_guard.h"
#include "side_action/actor/side_action_scroll_range.h"
#include "side_action/actor/side_action_thunder.h"
#include "side_action/actor/side_action_move_energy.h"
#include "side_action/actor/side_action_plane_obstacle.h"
#include "side_action/actor/side_action_endless_jump_score.h"
#include "side_action/actor/side_action_endless_scroll_score.h"
#include "side_action/actor/side_action_endless_obstacle.h"
#include "side_action/actor/side_action_endless_plane_obstacle.h"
#include "side_action/actor/side_action_score_stop_watch.h"
#include "side_action/actor/side_action_best_score.h"
#include "side_action/actor/side_action_ball_receiver.h"
#include "side_action/actor/side_action_warp_switch.h"
#include "side_action/actor/side_action_logo.h"
#include "side_action/actor/side_action_endless_thunder.h"
#include "side_action/stage/side_action_stage.h"
#include "breakout/actor/breakout_ball.h"
#include "root.h"
#include "common/cmn_util.h"
#include <graphics/font/tb_font_buffer_2d.h>

struct BreakoutBallDeadFunc
{
    BreakoutBallDeadFunc( SideActionPlayer* in ){
        player = in;
        player->Retain();
    }
    BreakoutBallDeadFunc( const BreakoutBallDeadFunc& src ){
        player = src.player;
        player->Retain();
    }
    ~BreakoutBallDeadFunc(){
        player->Release();
    }
    void operator()(){
        TbSint32 isEnableDeadState = 0;
        SideActionStage* stage = TbDynamicCast<SideActionStage*>(player->GetStage());
        if (stage) {
            SideActionGoal* goal = stage->GetGoal();
            if (goal && goal->IsOpened()) {
                // 既にゴールが開いていたら何もしない
                return;
            }
        }
        player->NotifyEvent(SIDE_ACTION_EVENT_DEAD, &isEnableDeadState);
    }
    SideActionPlayer* player;
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionActorManager::SideActionActorManager()
    : m_SpecialPlayerCount(0)
    , m_RouletteCount(0)
    , m_Goal(nullptr)
    , m_Ghost(nullptr)
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    for( TbUint32 i = 0; i < playerCount; ++i ){
        m_PlayerIndexList.PushBack(i);
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionActorManager::~SideActionActorManager()
{
    TbForIterator( ite , m_Player ){
        (*ite)->Release();
    }
    if( m_Goal ) {
        m_Goal->Release();
    }
    if (m_Ghost) {
        m_Ghost->Release();
    }
    m_Player.Clear();
}

/*!
 * アクター生成
 * @author teco
 * @since 2013.10.20
 */
CrActor* SideActionActorManager::CreateActor(const CrActorFactory::CreateParam& param , CrActionStage* stage)
{
    static const TbFloat32 BALL_OFFSET_Y = -120;
    TbVector2 initPos = param.pos;

    CrActionActor* actor = NULL;

    if( param.typeName == "Player" ){
        SideActionPlayer* player = createPlayer(&initPos,stage);
        if( !player ){
            return nullptr;
        }
#if 0
        if( 0 == m_Player.GetCount()%2 ){
            stage->CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HALOWEEN2);
        }else{
            stage->CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HALOWEEN1);
        }
#endif
        actor = player;

    }else if( param.typeName == "WarpGunPlayer" ){

        SideActionPlayer* player = createPlayer(&initPos, stage);
        if( !player ){
            return nullptr;
        }
        player->SetOffserUV(TbVector2(0.0f,CMN_TEX_UV(32.0f)));
        stage->CreateActor<SideActionPlayerWarpGun>(initPos,player);
        actor = player;
    }
    else if (param.typeName == "MagnetPlayer") {

        SideActionPlayer* player = createPlayer(&initPos, stage);
        if (!player) {
            return nullptr;
        }
        player->SetOffserUV(TbVector2(0.0f, CMN_TEX_UV(32.0f)));
        stage->CreateActor<SideActionPlayerMagnet>(initPos, player);
        actor = player;

    } else if (param.typeName == "GuardPlayer") {

        SideActionPlayer* player = createPlayer(&initPos, stage);
        if (!player) {
            return nullptr;
        }

        CmnDir dir = CMN_DIR_UP;
        if (1 < param.userData.GetCount()) {
            dir = static_cast<CmnDir>(param.userData.AsSint32(1));
        }
        stage->CreateActor<SideActionPlayerGuard>(initPos, player, dir);

        if (2 < param.userData.GetCount()) {
            dir = static_cast<CmnDir>(param.userData.AsSint32(2));
            stage->CreateActor<SideActionPlayerGuard>(initPos, player, dir);
        }
        if (3 < param.userData.GetCount()) {
            dir = static_cast<CmnDir>(param.userData.AsSint32(3));
            stage->CreateActor<SideActionPlayerGuard>(initPos, player, dir);
        }

        actor = player;

    }else if( param.typeName == "SleepPlayer" ){
        SideActionPlayer* player = createPlayer(&initPos, stage);
        if (!player) {
            return nullptr;
        }
        if (player->GetPlayerIndex() < Root::GetInstance().GetPlayerCount() / 2) {
            player->Sleep();
        }
        actor = player;

    }else if( param.typeName == "TitlePlayer" ){

        SideActionPlayer* player = createPlayer(&initPos, stage);
        if( !player ){
            return nullptr;
        }
        player->SetTitleMode();
        actor = player;
    }
    else if (param.typeName == "MainMenuPlayer") {

        SideActionPlayer* player = createPlayer(&initPos,stage);
        if (!player) {
            return nullptr;
        }
        player->SetMainMenuMode();
        actor = player;

    }else if( param.typeName == "BreakoutPlayer" ){

        // ボール生成
        TbVector2 ballPos = initPos;
        ballPos.Add( 0.0f , BALL_OFFSET_Y );
        TbStaticString64 name = TbStaticString64::Format("%s%s", param.name.GetBuffer() , "Ball");
        BreakoutBall* ball = stage->CreateActorWithName<BreakoutBall,TbBool>(ballPos, name.GetBuffer(),TB_FALSE);

        SideActionPlayerHeaddress::Type headType = SideActionPlayerHeaddress::TYPE_HELMET;
        if (SaveFile::GetInstance().IsVisiblePlayerNumber()) {
            headType = SideActionPlayerHeaddress::TYPE_NUMBER;
        }
        SideActionPlayer* player = createPlayer(&initPos, stage, headType , TB_TRUE );
        if( player ){
            // ヘルメット生成
            ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            actor = player;
        }else{
            if( TB_VERIFY(!m_Player.IsEmpty())) {
                player = m_Player.GetBack();
                player->AddLife();
                ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            }
            return nullptr;
        }

    }else if( param.typeName == "BattleBreakoutPlayer" ||
              param.typeName == "BattleBreakoutPlayer2"){

        // ボール生成
        TbVector2 ballPos = initPos;
        ballPos.Add( 0.0f , BALL_OFFSET_Y );

        TbStaticString32 ballName = param.name;
        ballName += "b";
        SideActionBreakoutBattleBall* ball = stage->CreateActorWithName<SideActionBreakoutBattleBall>(ballPos,ballName.GetBuffer());
        if( param.typeName == "BattleBreakoutPlayer2" ){
            ball->SetType(SideActionBreakoutBattleBall::TYPE_TERRITORY);
        }
        SideActionPlayerHeaddress::Type headType = SideActionPlayerHeaddress::TYPE_HELMET;
        if (SaveFile::GetInstance().IsVisiblePlayerNumber()) {
            headType = SideActionPlayerHeaddress::TYPE_NUMBER;
        }
        SideActionPlayer* player = createPlayer(&initPos, stage, headType, TB_TRUE);
        if( player ){
            // ヘルメット生成
            ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            actor = player;
        }else if( m_Player.GetCount()%2 == 0 ) {
            if( TB_VERIFY(!m_Player.IsEmpty())) {
                player = m_Player.GetBack();
                player->AddLife();
                ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            }
            return nullptr;
        }

    }else if( param.typeName == "SwitchPlayer" ){

        SideActionPlayer* player = createSpecialPlayer(&initPos);
        if( !player ){
            return nullptr;
        }
        player->SetSwitchInput();
        actor = player;

    }else if( param.typeName == "MultiPlayer" ){

        SideActionPlayer* player = createPlayer(&initPos, stage);
        actor = player;
        player->SetMultiPlayerInput(SIDE_ACTION_JUMP_VELO,3.0f); // 今は同じ
        player->SetJumpContinueCount(param.userData.AsSint32(1));
        player->SetEnableChangeColorWithJump(TB_TRUE);

    }else if( param.typeName == "MajorityPlayer" ){
        static TB_NDEBUG_CONST TbFloat32 MAJORITY_INPUT_RATE = 0.7f;

        // プレイヤー生成
        SideActionPlayer* player = createPlayer(&initPos, stage, SideActionPlayerHeaddress::TYPE_MAX);
        if( !player ){
            return nullptr;
        }
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        TbFloat32 rate = 1.0f;
        if( playerCount > 0 ) {
            rate = MAJORITY_INPUT_RATE * (static_cast<TbFloat32>(playerCount));
            rate = TbCeil(rate);
            rate /= (static_cast<TbFloat32>(playerCount));
            rate = TbClamp(rate,0.1f,1.0f);
        }
        player->SetMajorityInput(playerCount, rate);
        actor = player;

    }else if (param.typeName == "PlanePlayer") {
    
        SideActionPlayer* player = createPlayer(&initPos, stage);
        if (!player) {
            return nullptr;
        }
        player->RequestState(SideActionPlayer::STATE_PLANE);
        actor = player;

    }else if( param.typeName == "Title" ){
        SideActionTitle* title = new SideActionTitle();
        actor = title;
    }else if( param.typeName == "TallBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_TALL);
        actor = box;
    }else if( param.typeName == "BigBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_BIG);
        actor = box;
    }else if( param.typeName == "NormalBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox();
        actor = box;
    }else if( param.typeName == "SmallBox" ){
        // 巨大じゃない箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_SMALL);
        actor = box;
    }else if (param.typeName == "PushBox") {

        // 可変サイズ箱生成
        TbFloat32 width  = param.userData.AsFloat32(1);
        TbFloat32 height = param.userData.AsFloat32(2);
        TbRectF32Pod rect = { -width/2.0f , -height , width , height };
        SideActionBigBox* box = new SideActionBigBox(rect);
        actor = box;
    }
    else if (param.typeName == "ColorBox" ||
             param.typeName == "JumpColorBox")
    {
        // カラー箱生成
        TbFloat32 width = 1 < param.userData.GetCount() ? param.userData.AsFloat32(1) : 32.0f;
        TbFloat32 height = 2 < param.userData.GetCount() ? param.userData.AsFloat32(2) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbSint32 exceptPlayerIndex = -1;
        if (3 < param.userData.GetCount()) {
            exceptPlayerIndex = param.userData.AsSint32(3);
        }
        SideActionColorBox* box = new SideActionColorBox(rect , param.userData.AsSint32(0) , exceptPlayerIndex );
        if (param.typeName == "JumpColorBox") {
            box->SetEnableJump(TB_TRUE);
        }
        actor = box;
    }
    else if (param.typeName == "ForceColorBox") 
    {
        // カラー箱生成
        TbFloat32 width = 1 < param.userData.GetCount() ? param.userData.AsFloat32(1) : 32.0f;
        TbFloat32 height = 2 < param.userData.GetCount() ? param.userData.AsFloat32(2) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        SideActionColorBox* box = new SideActionColorBox(rect , param.userData.AsSint32(0) );
        actor = box;
    }
    else if (param.typeName == "FallBox")
    {
        SideActionFallBox* box = new SideActionFallBox();
        actor = box;

    } else if (param.typeName == "ColorBlock") {
        // カラー箱生成
        TbSint32 type = param.userData.AsSint32(0);
        TbFloat32 width = 1 < param.userData.GetCount() ? param.userData.AsFloat32(2) : 32.0f;
        TbFloat32 height = 2 < param.userData.GetCount() ? param.userData.AsFloat32(3) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbSint32 exceptPlayerIndex = -1;
        if (4 < param.userData.GetCount()) {
            exceptPlayerIndex = param.userData.AsSint32(4);
        }
        SideActionColorBlock* block = new SideActionColorBlock(rect , param.userData.AsSint32(1) , exceptPlayerIndex );
        actor = block;
    }else if (param.typeName == "ColorBlockConstraint") {
        SideActionColorBlockConstraint* constraint = new SideActionColorBlockConstraint();
        actor = constraint;

        TbFloat32 width = 0 < param.userData.GetCount() ? param.userData.AsFloat32(0) : 32.0f;
        TbFloat32 height = 1 < param.userData.GetCount() ? param.userData.AsFloat32(1) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbFloat32 distance = 2 < param.userData.GetCount() ? param.userData.AsFloat32(2) : 32.0f;

        constraint->SetDistance(distance);

        SideActionColorBlock* block1 = new SideActionColorBlock(rect,0,-1);
        SideActionColorBlock* block2 = new SideActionColorBlock(rect, 0, -1);
        TbVector2 pos1 = initPos;
        pos1.Add(64.0f,0.0f);
        stage->AddActor(block1, initPos);
        block1->Release();
        TbVector2 pos2 = pos1;
        pos1.Add(-64.0f, 0.0f);
        stage->AddActor(block2, pos2);
        block2->Release();

        constraint->AddBlock(block1);
        constraint->AddBlock(block2);

    }else if( param.typeName == "StepEnemy" ){
        // ステップエネミー
        SideActionStepEnemy* enemy = new SideActionStepEnemy();
        actor = enemy;
    }else if (param.typeName == "BowwowEnemy") {
        // 動きに反応して吹っ飛ばすエネミー
        SideActionBowwowEnemy* enemy = new SideActionBowwowEnemy();
        actor = enemy;
    }else if( param.typeName == "UpDownLift" ){
        // 上下移動リフト
        SideActionUpDownLift* lift = new SideActionUpDownLift();
        actor = lift;
    }else if( param.typeName == "Lift" ){
        // リフト
        SideActionLift* lift = new SideActionLift();
        actor = lift;
    }else if( param.typeName == "UpDownEnemy" ){
        // 上下移動エネミー
        actor = new SideActionUpDownEnemy();
    }else if( param.typeName == "Key" ){
        // 鍵
        if( !stage->FindActor(param.name.GetBuffer())){
            SideActionKey* key = new SideActionKey();
            TbFloat32 offsetY = static_cast<TbFloat32>(param.userData.AsSint32(0));
            TbFloat32 offsetX = static_cast<TbFloat32>(param.userData.AsSint32(1));
            initPos = key->CalcInitPos(initPos,TbVector2(offsetX, offsetY));
            actor = key;
        }
    }else if( param.typeName == "BreakoutKey" ){
        // 鍵(ブロック崩し用)
        actor = new SideActionKey( SideActionKey::TYPE_BREAKOUT );
    }else if( param.typeName == "Goal" ){
        // ゴール
        if( !TB_VERIFY(!m_Goal) ) {
            m_Goal->Release();
        }
        m_Goal = new SideActionGoal();
        m_Goal->Retain();
        actor = m_Goal;
    }else if( param.typeName == "CheckPoint" ){
        // チェックポイント 
        actor = new SideActionCheckPoint();
    }else if( param.typeName == "BreakoutBall" ){
        // ブロック崩し用ボール
        actor = new BreakoutBall(TB_TRUE);
    }else if( param.typeName == "MajorityController" ){
        SideActionPlayer* player = GetPlayer(param.userData.AsSint32(0));
        // 多数決用コントローラ
        if( TB_VERIFY(player) ){
            actor = new SideActionMajorityController(player);
        }
    }else if( param.typeName == "InvisibleWeightedLift" ){
        SideActionVariableWeightedLift* lift = new SideActionVariableWeightedLift();
        actor = lift;
    }else if( param.typeName == "WeightedLift" ){
        // 重さで動くリフト
        SideActionWeightedLift* lift = new SideActionWeightedLift();
        if(param.userData.AsSint32(0) != 0 ){
            // 高さ
            TbFloat32 height = param.userData.AsFloat32(0);
            // 高さへの人数別オフセット
            if(param.userData.AsSint32(2) != 0 ) {
                height += param.userData.AsFloat32(2) * Root::GetInstance().GetPlayerCount();
            }
            lift->SetHeight(height);
        }
        // 動く割合(100分率)
        if(param.userData.AsSint32(1) > 0 ){
            lift->SetMoveContactCountParcent(param.userData.AsSint32(1), param.userData.AsSint32(3));
        }
        // 戻らないか
        if (param.userData.AsSint32(4) > 0) {
            lift->SetEnableReturn(TB_FALSE);
        }
        if ( !TB_FLOAT32_EQUAL_ZERO(param.userData.AsFloat32(5)) ){
            TbFloat32 speed = TbMax<TbFloat32>(0.0f,Root::GetInstance().GetPlayerCount()- CMN_PLAYER_MIN);
            speed *= param.userData.AsFloat32(5);
            lift->AddSpeed(speed);
        }
        actor = lift;
    }else if( param.typeName == "WeightedLiftEx" || 
              param.typeName == "WeightedLiftEx2" )
    {
        // 重さで動くリフト
        SideActionWeightedLift::Type type = SideActionWeightedLift::TYPE_NORMAL;
        if (param.typeName == "WeightedLiftEx2") {
            type = SideActionWeightedLift::TYPE_ONE;
        }
        SideActionWeightedLift* lift = new SideActionWeightedLift(0.0f, type);
        if(param.userData.AsSint32(0) != 0 ){
            TbFloat32 height = param.userData.AsFloat32(0);
            TbSint32 offset = param.userData.AsSint32(3+(Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN));
            if( offset != 0 ) {
                height += offset;
            }
            lift->SetHeight(height);
        }
        if(param.userData.AsSint32(1) > 0 ){
            lift->SetMoveContactCountParcent(param.userData.AsSint32(1),0);
        }
        lift->SetSpeed(param.userData.AsFloat32(2));
        actor = lift;
    }else if( param.typeName == "MoveWall" ){
        // 動く壁
        TbFloat32 movWidth = 0.0f;
        TbFloat32 triggerWidth = 0.0f;
        TbFloat32 height = -1.0f;
        if( param.userData.AsSint32(0) != 0 ){
            movWidth = static_cast<TbFloat32>(param.userData.AsSint32(0));
        }
        if(param.userData.AsSint32(1) != 0 ){
            triggerWidth = static_cast<TbFloat32>(param.userData.AsSint32(1));
        }
        if ( 0.0f < param.userData.AsFloat32(2) ) {
            height = static_cast<TbFloat32>(param.userData.AsFloat32(2));
        }
        SideActionMoveWall* wall = new SideActionMoveWall( movWidth , triggerWidth , height);
        actor = wall;
    }else if( param.typeName == "JumpStand" ){
        TbVector2 jumpVelo( 0.0f , param.userData.AsFloat32(0) );
        SideActionJumpStand* stand = new SideActionJumpStand(jumpVelo);
        actor = stand;
    }
    else if (param.typeName == "JumpStandEx") {
        TbVector2 jumpVelo(param.userData.AsFloat32(0) , param.userData.AsFloat32(1));
        SideActionJumpStand* stand = new SideActionJumpStand(jumpVelo);
        actor = stand;
    }else if (param.typeName == "JumpArea") {
        SideActionJumpArea* area = new SideActionJumpArea();
        actor = area;
    }else if( param.typeName == "BoundBall" ){
        SideActionBoundBall* ball = new SideActionBoundBall();
        actor = ball;
    }else if( param.typeName == "BoundBallPitcher" ){
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_BOUND);
        actor = pitcher;
    }else if( param.typeName == "LaserBallPitcher" ){
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_LASER);
        actor = pitcher;
    }else if (param.typeName == "DeadBallPitcher") {
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_DEAD_LASER);
        actor = pitcher;
    }else if (param.typeName == "DeadBallPitcher") {
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_DEAD_LASER);
        actor = pitcher;
    }else if (param.typeName == "DeadBallPitcher") {
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_DEAD_LASER);
        actor = pitcher;
    }else if( param.typeName == "EndlessLaserBallPitcher" ){
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_ENDLESS_LASER);
        actor = pitcher;
    }else if( param.typeName == "PhysicsBallPitcher" ){
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_PHYSICS);
        actor = pitcher;
    }else if (param.typeName == "BallReceiver") {
        SideActionBallReceiver* receiver = new SideActionBallReceiver();
        actor = receiver;
    }else if( param.typeName == "SwitchMediator" ){
        actor = new SideActionSwitchMediator();
    }else if( param.typeName == "Switch" ){
        SideActionSwitch* sw = new SideActionSwitch(SideActionSwitch::TYPE_NORMAL);
        actor = sw;
        if( 0 < param.userData.AsSint32(0) ) {
            sw->SetEnableRepeat(TB_TRUE);
        }
        sw->SetId(param.userData.AsSint32(1));
        if (2 < param.userData.GetCount()) {
            const char* name = param.userData.AsString(2);
            sw->SetSubTarget(name);
        }
    }else if( param.typeName == "PhysicsSwitch" ){
        actor = new SideActionSwitch(SideActionSwitch::TYPE_PHYSICS);
    }else if( param.typeName == "DeadSwitch" ){
        actor = new SideActionDeadSwitch();
    }else if( param.typeName == "WakeSwitch" ){
        actor = new SideActionWakeSwitch();
    }else if (param.typeName == "JumpSwitch") {
        actor = new SideActionJumpSwitch();
    }else if (param.typeName == "ScaleSwitch") {
        actor = new SideActionScaleSwitch();
        SideActionStage* actionStage = TbDynamicCast<SideActionStage*>(stage);
        actionStage->SetEnablePlayerSortY();
    }else if (param.typeName == "DelaySwitch") {
        actor = new SideActionDelaySwitch();
    }else if (param.typeName == "WarpSwitch") {
        actor = new SideActionWarpSwitch();
    }else if( param.typeName == "BallBox" ){
        SideActionBallKeyBox* box = new SideActionBallKeyBox();
        TbFloat32 offsetX = static_cast<TbFloat32>(param.userData.AsSint32(0) * Root::GetInstance().GetPlayerCount());
        initPos.Add( offsetX , 0.0f );
        actor = box;
    }else if( param.typeName == "LaserKeyBox" ){
        SideActionLaserKeyBox* box = new SideActionLaserKeyBox();
        actor = box;
    }else if( param.typeName == "Bridge" ||
              param.typeName == "KeyBridge" )
    {
        enum
        {
            ARG_COUNT ,
            ARG_DIR_X ,
            ARG_DIR_Y ,
            ARG_OFFSET ,
            ARG_SIZE ,
            ARG_ENABLE_PUSH ,
        };
        TbSint32 count = param.userData.AsSint32(ARG_COUNT);
        TbVector2 dir( static_cast<TbFloat32>(param.userData.AsSint32(1)) , 
                       static_cast<TbFloat32>(param.userData.AsSint32(2)) );
        TbSint32 size = param.userData.AsSint32(ARG_SIZE);
        dir.Normalize();
        TbBool isEnablePush = TB_FALSE;
        if (param.userData.AsSint32(ARG_ENABLE_PUSH) > 0) {
            isEnablePush = TB_TRUE;
        }
        SideActionLink* bridge = new SideActionLink(SideActionLink::TYPE_OUT, 
                                                    count, size,dir , isEnablePush);

        TbSint32 initOffsetCount = 0;
        if(param.userData.AsSint32(ARG_OFFSET) > 0 ) {
            TbFloat32 lack = static_cast<TbFloat32>(CMN_PLAYER_COUNT - Root::GetInstance().GetPlayerCount());
            initOffsetCount = static_cast<TbSint32>(lack * static_cast<TbFloat32>(param.userData.AsSint32(3))*0.1f);
        }
        bridge->SetInitOffsetCount(initOffsetCount);
        if (param.typeName == "KeyBridge")
        {
            // 鍵取得で開く
            bridge->SetEnableOpenByKey(TB_TRUE);
        }
        actor = bridge;
    }else if( param.typeName == "Gate" ){
        TbVector2 dir(param.userData.AsFloat32(1) , param.userData.AsFloat32(2) );
        dir.Normalize();
        // 
        TbSint32 blockCount = param.userData.AsSint32(0);
        TbSint32 blockSize = param.userData.AsSint32(3);
        SideActionLink* bridge = new SideActionLink(SideActionLink::TYPE_IN, blockCount ,blockSize ,dir , TB_FALSE );
        actor = bridge;
    }else if( param.typeName == "Warp" || param.typeName == "WarpAll" ){
        TbFloat32 w = static_cast<TbFloat32>(param.userData.AsSint32(0));
        TbFloat32 h = static_cast<TbFloat32>(param.userData.AsSint32(1));
        TbFloat32 destX = static_cast<TbFloat32>(param.userData.AsSint32(2));
        TbFloat32 destY = static_cast<TbFloat32>(param.userData.AsSint32(3));
        TbFloat32 destOffsetX = static_cast<TbFloat32>(param.userData.AsSint32(4));
        TbFloat32 destOffsetY = static_cast<TbFloat32>(param.userData.AsSint32(5));
        SideActionWarp* warp = new SideActionWarp(w,h);
        warp->SetDestination(TbVector2(destX,destY));
        warp->SetDestOffset(TbVector2(destOffsetX,destOffsetY));
        if (param.typeName == "WarpAll") {
            warp->SetTargetAll(TB_TRUE);
        }
        actor = warp;
    }else if (param.typeName == "WarpInitPos" ||
              param.typeName == "WarpAllInitPos") 
    {
        TbFloat32 w = static_cast<TbFloat32>(param.userData.AsSint32(0));
        TbFloat32 h = static_cast<TbFloat32>(param.userData.AsSint32(1));
        SideActionWarp* warp = new SideActionWarp(w, h);
        warp->SetDestInitPos(TB_TRUE);
        if(param.typeName == "WarpAllInitPos") {
            warp->SetTargetAll(TB_TRUE);
        }
        actor = warp;
    }else if( param.typeName == "PhysicsBall" ){
        SideActionPhysicsBall* ball = new SideActionPhysicsBall();
        actor = ball;
    }else if( param.typeName == "PhysicsArea" ){
        SideActionPhysicsArea* area = new SideActionPhysicsArea();
        actor = area;
    }else if( param.typeName == "PhysicsRect" ){
        SideActionPhysicsRect* area = new SideActionPhysicsRect();
        actor = area;
    }else if( param.typeName == "Seesaw" ){
        SideActionSeesaw* seesaw = new SideActionSeesaw( static_cast<SideActionSeesaw::Type>(param.userData.AsSint32(0)) );
        if( param.userData.AsSint32(1) ) {
             SideActionSeesaw* parent = static_cast<SideActionSeesaw*>(stage->FindActor("SeesawParent"));
             if( TB_VERIFY(parent) ) {
                 seesaw->Attach( parent );
             }
        }
        actor = seesaw;
    }else if( param.typeName == "SeesawParent" ){
        SideActionSeesawParent* seesaw = new SideActionSeesawParent( static_cast<SideActionSeesaw::Type>(param.userData.AsSint32(0)) );
        seesaw->SetControllRangeX( static_cast<TbFloat32>(param.userData.AsSint32(1)) , static_cast<TbFloat32>(param.userData.AsSint32(2)) );
        actor = seesaw;
    }else if( param.typeName == "RetryTimer" ){
        SideActionTimer* timer = new SideActionTimer( SideActionTimer::TYPE_DOWN );
        timer->SetTimeUpAction(SideActionTimer::TIMEUP_ACTION_RETRY);
        actor = timer;
    }else if (param.typeName == "DeadTimer") {
        SideActionTimer* timer = new SideActionTimer(SideActionTimer::TYPE_DOWN);
        timer->SetTimeUpAction(SideActionTimer::TIMEUP_ACTION_DEAD);
        actor = timer;
    }else if( param.typeName == "SwitchTimer" ){
        SideActionTimer* timer = new SideActionTimer( SideActionTimer::TYPE_DOWN );
        timer->SetTimeUpAction(SideActionTimer::TIMEUP_ACTION_SWITCH);
        actor = timer;
    }else if( param.typeName == "StopWatch" ){
        SideActionStopWatch* timer = new SideActionStopWatch();
        actor = timer;
    }else if( param.typeName == "CoopStopWatch" ){
        TbSint32 index = (Root::GetInstance().GetPlayerCount()-2)+1;
        if( 0 <= index ) {
            SideActionCoopStopWatch* watch = new SideActionCoopStopWatch();
            TbFloat32 max = param.userData.AsFloat32(index) * static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount());
            watch->SetJudgeSec(param.userData.AsFloat32(0), max);
            actor = watch;
        }
    }else if( param.typeName == "Watch" ){
//        SideActionWatch* timer = new SideActionWatch();
//        actor = timer;
    }else if( param.typeName == "Coin" ){
        SideActionCoin* coin = new SideActionCoin();
        // コインオブサーバーが存在してたら監視対象として登録しておく
        if( param.userData.AsSint32(0) ) {
            SideActionCoinObserver* parent = static_cast<SideActionCoinObserver*>(stage->FindActor("CoinObserver"));
            if( TB_VERIFY(parent) ) {
                parent->AddCoin(coin);
            }
        }
        actor = coin;
    }else if( param.typeName == "CoinObserver" ){
        SideActionCoinObserver* coinObserver = new SideActionCoinObserver();
        actor = coinObserver;
#if 0
    }else if( param.typeName == "Marker" ){
        SideActionMarker* marker = new SideActionMarker(SideActionMarker::TYPE_NORMAL );
        actor = marker;
    }else if( param.typeName == "LeftMarker" ){
        SideActionMarker* marker = new SideActionMarker( SideActionMarker::TYPE_LEFT );
        actor = marker;
    }else if( param.typeName == "RightMarker" ){
        SideActionMarker* marker = new SideActionMarker( SideActionMarker::TYPE_RIGHT );
        actor = marker;
#endif
    }else if( param.typeName == "Text" ){
        SideActionText* text = new SideActionText();
        actor = text;
    }else if( param.typeName == "Balance" ){
        TbFloat32 interval = static_cast<TbFloat32>(param.userData.AsSint32(0));
        SideActionBalance* marker = new SideActionBalance(interval);
        actor = marker;
    }else if( param.typeName == "HeightJudge" ){
        SideActionJudgeHeight* judge = new SideActionJudgeHeight();
        actor = judge;
    }else if( param.typeName == "TerritoryJudge" ){
        SideActionJudgeTerritory* judge = new SideActionJudgeTerritory(stage->GetMap().GetTileWidth(), stage->GetMap().GetTileHeight());
        actor = judge;
    }else if( param.typeName == "BreakoutJudge" ){
        SideActionJudgeBreakout* judge = new SideActionJudgeBreakout(stage->GetMap().GetTileWidth(), stage->GetMap().GetTileHeight());
        actor = judge;
    }else if( param.typeName == "TerritoryBallJudge" ){
        SideActionJudgeBreakout* judge = new SideActionJudgeBreakout(stage->GetMap().GetTileWidth(), stage->GetMap().GetTileHeight());
        judge->SetType(SideActionJudgeBreakout::TYPE_TERRITORY);
        actor = judge;
    }else if (param.typeName == "StopWatchJudge") {
        SideActionJudgeStopWatch* judge = new SideActionJudgeStopWatch();
        actor = judge;
    }else if (param.typeName == "BattleJumpJudge") {
        SideActionJudgeBattleJump* judge = new SideActionJudgeBattleJump();
        actor = judge;
    }else if( param.typeName == "Ghost" ){
        SideActionGhost* ghost = new SideActionGhost();
        if (!m_Ghost) {
            m_Ghost = ghost;
            m_Ghost->Retain();
        }
        actor = ghost;
    }else if( param.typeName == "CollisionSwitch" ){
        TbFloat32 w = param.userData.AsFloat32(0);
        TbFloat32 h = param.userData.AsFloat32(1);
        const char* targetName = param.userData.AsString(2);
        SideActionCollisionSwitch* sw = new SideActionCollisionSwitch(w,h);
        sw->SetTagetName(targetName);
        actor = sw;
    }else if (param.typeName == "CollisionChangePlayer") {
        actor = new SideActionCollisionChangePlayer();
    }else if (param.typeName == "CollisionActorCreator") {
        actor = new SideActionCollisionActorCreator();
    }else if (param.typeName == "CollisionActorCreatorSilent") {
        SideActionCollisionActorCreator* creator = new SideActionCollisionActorCreator();
        creator->SetSilent();
        actor = creator;
    }else if (param.typeName == "CollisionConstraintMove") {
        actor = new SideActionCollisionConstraintMove();
    }else if( param.typeName == "Wind" ){
        SideActionWind* wind = new SideActionWind();
        initPos.Add(0.0f, param.userData.AsFloat32(0) * Root::GetInstance().GetPlayerCount());
        actor = wind;
    }else if( param.typeName == "DistanceConstraint" ){
        SideActionDistanceConstraint* constraint = new SideActionDistanceConstraint();
        actor = constraint;
    }else if( param.typeName == "Puzzle" ){
        SideActionTetris* tetris = new SideActionTetris();

        TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
        TbScriptLuaNode*    node   = script.GetVM(0)->GetNode(0);
        if( 2 < param.userData.GetCount() ) {
            tetris->SetNorma(param.userData.AsSint32(2));
        }
        tetris->InitializeByScript(param.userData.AsString(0));
        tetris->SetSwitchTarget(param.userData.AsString(1));
        actor = tetris;

    }
    else if (param.typeName == "EndlessPuzzle") {
        SideActionEndlessTetris* tetris = new SideActionEndlessTetris();
        actor = tetris;

    }else if (param.typeName == "MoveMeter") {

//        SideActionMoveMeter* meter = new SideActionMoveMeter();
//        actor = meter;

    }else if( param.typeName == "KeyGoal" ){

        // 鍵ゴール
        SideActionKeyProxyGoal* keyGoal = new SideActionKeyProxyGoal();
        actor = keyGoal;

    }else if( param.typeName == "RouletteManager" ){

        // ルーレット
        SideActionRouletteManager* roulette = new SideActionRouletteManager();
        roulette->SetMax( m_RouletteCount );
        
        actor = roulette;

    }else if( param.typeName == "Roulette" ){

        TbUint32 minPlayer = param.userData.AsSint32(0);
        if( minPlayer <= Root::GetInstance().GetPlayerCount() ){

            // ルーレット
            SideActionRoulette* roulette = new SideActionRoulette();
        
            // WORDを追加
            roulette->SetAnswer(param.userData.AsSint32(1));
            TbUint32 count = param.userData.GetCount();
            for( TbUint32 i = 2; i < count; ++i ) {
                roulette->AddCandidate(param.userData.AsString(i));
            }

            actor = roulette;
            ++m_RouletteCount;
        }

    }else if( param.typeName == "BlinkBlock" ){

        // 点滅ブロック
        SideActionBlinkBlock* block = new SideActionBlinkBlock();
        
        // WORDを追加
        actor = block;

    }else if( param.typeName == "TrafficLight" ){

        // 点滅ブロック
        SideActionTrafficLight* block = new SideActionTrafficLight();
        
        // WORDを追加
        actor = block;
    }
    else if (param.typeName == "MapRect" )
    {
        SideActionMapRect* block = new SideActionMapRect();
        actor = block;
    }
    else if (param.typeName == "Rect" || 
               param.typeName == "InvisibleRect" ||
               param.typeName == "SwitchRect" ||
               param.typeName == "DamageRect" ) 
    {
        TbUint32 playerOffsetCount = Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN;

        // 点滅ブロック
        TbDimensionF32 size(param.userData.AsFloat32(0), param.userData.AsFloat32(1));
        size.width  += playerOffsetCount * param.userData.AsFloat32(2);
        size.height += playerOffsetCount * param.userData.AsFloat32(3);
        if( 4 < param.userData.GetCount() ){
            initPos.Add( playerOffsetCount * param.userData.AsFloat32(4) ,
                         playerOffsetCount * param.userData.AsFloat32(5) );
        }
        if (size.width < 0) {
            initPos.Add(size.width,0.0f);
            size.width *= -1.0f;
        }
        TbRectF32 rect(0, -size.height, size.width, size.height);
        SideActionRect* target = new SideActionRect(rect);
        if (param.typeName == "SwitchRect") {
            target->SetType(SideActionRect::TYPE_SWITCH);
        }else if (param.typeName == "DamageRect") {
            target->SetType(SideActionRect::TYPE_DAMAGE);
        }else if (param.typeName == "InvisibleRect") {
            target->SetVisible(TB_FALSE);
            target->GetCollision()->SetActive(TB_TRUE);
        }
        actor = target;

    }else if (param.typeName == "PlaneObstacle"){

        actor = new SideActionPlaneObstacle();

    }else if (param.typeName == "ScrollLimitRange") {

        TbFloat32 width = param.userData.AsFloat32(0);
        TbBool isTriggerLeft = TB_FALSE;
        if (1 < param.userData.GetCount()) {
            isTriggerLeft = param.userData.AsSint32(1);
        }
        actor = new SideActionScrollLimitRange(width, isTriggerLeft);

    }else if (param.typeName == "Thunder") {

        actor = new SideActionThunder();

    }else if (param.typeName == "EndlessThunder") {

        actor = new SideActionEndlessThunder();

    }else if (param.typeName == "MoveEnergy") {
        
        actor = new SideActionMoveEnergy();

    }else if (param.typeName == "EndlessJumpScore"){

        actor = new SideActionEndlessJumpScore();

    }else if (param.typeName == "EndlessScrollScore") {

        actor = new SideActionEndlessScrollScore();

    }else if (param.typeName == "StopWatchScore") {

        actor = new SideActionScoreStopWatch();

    }else if (param.typeName == "EndlessObstacle") {

        actor = new SideActionEndlessObstacle();

    }else if (param.typeName == "EndlessPlaneObstacle") {

        actor = new SideActionEndlessPlaneObstacle();

    }else if (param.typeName == "BestScore") {

        actor = new SideActionBestScore();

    }else if (param.typeName == "Logo") {

        actor = new SideActionLogo();

    }else{
        TB_ASSERT_MSG_FORMAT(0,"Invalid Name (%s)", param.name.GetBuffer());
    }
    if( actor ) {
        actor->SetName(param.name.GetBuffer());
        actor->Setup(param.userData);
        stage->AddActor(actor,initPos);
        actor->Release();
    }
    return actor;
}

/*!
 * プレイヤー生成
 * @author teco
 */
SideActionPlayer* SideActionActorManager::createPlayer( TbVector2* initPos, CrActionStage* stage , SideActionPlayerHeaddress::Type type , TbBool isEnableHeadCollision )
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    if( IsEnableLimitPlayer() &&
        m_Player.GetCount() >= playerCount )
    {
        return NULL;
    }

    // プレイヤー生成
    TbUint32 playerIndex = m_Player.GetCount();
    if (SeqManager::GetInstance().GetCurrentSequence()->GetType() != SEQ_MAIN_MENU)
    {
        playerIndex = m_PlayerIndexList[playerIndex%m_PlayerIndexList.GetCount()];
    }

    SideActionPlayer* player = new SideActionPlayer(playerIndex);
    player->Retain();
    TB_VERIFY(m_Player.PushBack(player));

    // チェックポイントチェック
    TbVector2 checkPos;
    TbFloat32 checkPosSec = 0.0f;
    if( Root::GetInstance().GetGlobalData().GetCheckPoint(checkPos,checkPosSec, stage->GetId()) )
    {
        if( initPos ){
            stage->SetPlaySec(checkPosSec);
            *initPos = checkPos;
            initPos->Add(0.0f,-(player->GetPlayerIndex()*48.0f));
            TbFloat32 scroll = TbMax(0.0f,checkPos.GetX() - 180.0f);
            stage->SetScroll(scroll, 0);
        }
    }

    if (type != SideActionPlayerHeaddress::TYPE_MAX && initPos ) {
        stage->CreateActor<SideActionPlayerHeaddress>(*initPos, player, type , isEnableHeadCollision);
    }

    return player;
}

/*!
 * 特殊プレイヤー生成
 * @author teco
 */
SideActionPlayer* SideActionActorManager::createSpecialPlayer( TbVector2* initPos )
{
    // プレイヤー生成
    TbUint32 playerIndex = m_Player.GetCount() % CMN_PLAYER_MAX;
    if( m_SpecialPlayerCount >= Root::GetInstance().GetPlayerCount() ) {
        return nullptr;
    }
    SideActionPlayer* player = new SideActionPlayer(CMN_PLAYER_MAX-1);
    player->Retain();
    TB_VERIFY(m_Player.PushBack(player));
    ++m_SpecialPlayerCount;

    return player;
}

