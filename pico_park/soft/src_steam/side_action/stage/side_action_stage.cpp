/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/actor/side_action_player_dress.h"
#include "side_action/actor/side_action_big_box.h"
#include "side_action/actor/side_action_color_box.h"
#include "side_action/actor/side_action_color_block.h"
#include "side_action/actor/side_action_key.h"
#include "side_action/actor/side_action_move_wall.h"
#include "side_action/actor/side_action_step_enemy.h"
#include "side_action/actor/side_action_updown_enemy.h"
#include "side_action/actor/side_action_updown_lift.h"
#include "side_action/actor/side_action_lift.h"
#include "side_action/actor/side_action_check_point.h"
#include "side_action/actor/side_action_majority_controller.h"
#include "side_action/actor/side_action_weighted_lift.h"
#include "side_action/actor/side_action_jump_stand.h"
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
#include "side_action/actor/side_action_ghost.h"
#include "side_action/actor/side_action_collision_switch.h"
#include "side_action/actor/side_action_blink_block.h"
#include "side_action/actor/side_action_wind.h"
#include "side_action/actor/side_action_stop_watch.h"
#include "side_action/actor/side_action_coop_stop_watch.h"
#include "side_action/actor/side_action_distance_constraint.h"
#include "side_action/actor/side_action_tetris.h"
#include "side_action/actor/side_action_launch_pad.h"
#include "side_action/actor/side_action_roulette.h"
#include "side_action/actor/side_action_traffic_light.h"
#include "side_action/actor/side_action_breakout_battle_ball.h"
#include "side_action/actor/side_action_rect.h"
#include "side_action/actor/side_action_player_gun.h"
#include "side_action/actor/side_action_scroll_range.h"
#include "breakout/actor/breakout_ball.h"
#include "root.h"

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
        player->NotifyEvent(SIDE_ACTION_EVENT_DEAD,&isEnableDeadState);
    }
    SideActionPlayer* player;
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionStage::SideActionStage( TbScriptLuaNode* node )
    : Super(node,SIDE_ACTION_ACTOR_LAYER_MAX,new SideActionCollisionScene())
    , m_ClearedPlayerCount(0)
    , m_SpecialPlayerCount(0)
    , m_RouletteCount(0)
    , m_Goal(NULL)
    , m_AutoScrollSpeed(1.0f)
    , m_ScrollLimit(-1.0f)
    , m_RequestScroll(-1.0f)
{
    // プロパティ取得
    {
        TbUint32 isScrollable = 0;
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(isScrollable, "autoScroll")) {
            m_BitArray.SetBit(FLAG_AUTO_SCROLL, isScrollable);
            m_BitArray.SetBit(FLAG_SCROLLABLE, isScrollable);
            node->TryToFloat32(m_AutoScrollSpeed, "autoScrollSpeed");
            TbFloat32 autoScrollSpeedOffset;
            if(TbScriptLuaNode::RESULT_OK == node->TryToFloat32(autoScrollSpeedOffset, "autoScrollSpeedOffset")){
                m_AutoScrollSpeed += autoScrollSpeedOffset * (Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN);
            }
        }
        else if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(isScrollable, "scrollable")) {
            m_BitArray.SetBit(FLAG_SCROLLABLE, isScrollable);
        }
        TbFloat32 scale = 1.0f;
        if(TbScriptLuaNode::RESULT_OK == node->TryToFloat32(scale, "scale") )
        {
            SetScale(scale);
            TbMatrix pose;
            pose.Scale(scale, scale, scale);
            SetPoseMatrix(pose);
            GetSpriteScene().SetPoseMatrix(pose);
            Root::GetInstance().GetLocalFont()->SetPoseMatrix(pose);
        }
        TbFloat32 scroll = 0;
        if( TbScriptLuaNode::RESULT_OK==node->TryToFloat32(scroll,"scroll") ){
            SetScroll(scroll);
            GetMap().SetScroll(scroll);
        }
        TbUint32 limitPlayer = 0;
        if( TbScriptLuaNode::RESULT_OK==node->TryToUint32(limitPlayer,"limitPlayer") ){
            m_BitArray.SetBit(FLAG_LIMIT_PLAYER_COUNT,limitPlayer);
        }
   }

    // コリジョンのレイヤー設定
    ActionCollisionScene& collisionScene = GetCollisionScene();
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_PLAYER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_ENEMY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER, SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_KEY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_OTHER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_GOAL,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_TO_PLAYER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_TO_PLAYER,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT,SIDE_ACTION_COLLISION_LAYER_KEY_TARGET,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT_2, SIDE_ACTION_COLLISION_LAYER_KEY_TARGET, TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_ENEMY,SIDE_ACTION_COLLISION_LAYER_ENEMY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_KEY,SIDE_ACTION_COLLISION_LAYER_GOAL,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_KEY,SIDE_ACTION_COLLISION_LAYER_KEY_TARGET,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_OTHER,TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionStage::~SideActionStage()
{
    GetActorScene().Clear();

    TbForIterator( ite , m_Player ){
        (*ite)->Release();
    }
    if( m_Goal ) {
        m_Goal->Release();
    }
    m_Player.Clear();
}

/*!
 * 更新
 * @author teco
 * @since 2013.10.02
 */
void SideActionStage::OnUpdate( TbFloat32 deltatime )
{
    if( !IsActive() ){
        return;
    }

    // 物理シーン更新
    UpdartePhysicsScene(deltatime);

    // コリジョンシーン
    UpdateCollisionScene();

    // スクロール更新
    updateScroll();

    // 状態遷移チェック
    updateClear();
    if( isCleared() ){
        RequestClear();
    }else if( isGameOver() ) {
        RequestRetry();
    }
}

/*!
 * 描画
 * @author Miyake_Shunske
 * @since 2015.01.27
 */ 
void SideActionStage::OnDraw( TbFloat32 deltatime )
{
    Super::OnDraw(deltatime);
#if 0
    TbForIterator( ite , m_Player )
    {
        if( !(*ite)->IsGameOver() ) {
            TbVector4 pos = (*ite)->GetPos();
            TbSint32 x = pos.GetX();
            TbSint32 y = pos.GetY();
            Root::GetInstance().GetFont()->Printf(x,
                                                  y-100.0f,
                                                  "%d,%d",
                                                  x,
                                                  y);
        }
    }
#endif
}

/*!
 * 描画
 * @author Miyake_Shunske
 * @since 2015.01.27
 */ 
void SideActionStage::OnDrawFront( TbFloat32 deltatime )
{
    Super::OnDrawFront(deltatime);
#if 0
    TbUint32 minute = GetPlaySec()/60.0f;
    TbUint32 sec    = static_cast<TbUint32>(GetPlaySec())%60;
    TbFontBuffer2d* font = Root::GetInstance().GetLocalFont();
    font->PushSetting();
    font->Printf(8,8,"TIME %02d:%02d",minute,sec);
    font->PopSetting();
    font->Flush();
#endif
}

/*!
 * 画面端を移動可能か
 * @author teco
 * @since 2013.10.20
 */
TbFloat32 SideActionStage::CalcScreenEndWalkVelo(SideActionPlayer* player, TbFloat32 velo )
{
#if 1
    if (TB_FLOAT32_EQUAL_ZERO(velo)) {
        return 0.0f;
    }
    TbFloat32 x = player->GetDrawPos().GetX();

    // スクロールしない場合、またｈオートスクローの場合は端の外に出ない
    if ( !m_BitArray.Test(FLAG_SCROLLABLE) || 
          m_BitArray.Test(FLAG_AUTO_SCROLL) || 
          (0.0f <= m_ScrollLimit && m_ScrollLimit < GetScroll()+velo) )
    {
        if(x + velo < 0.0f ){
            return -x;
        }
        if( GetWindowWidth() < x +velo ){
            return GetWindowWidth()- x;
        }
        return velo;
    }

    // 一人の時
    if( m_Player.GetCount() <= 1 ){
        return velo;
    }

    // スクロールできる時は、左端キャラと右端キャラが画面の外に出ないように
    TbFloat32 playerRightX = -TB_FLOAT32_MAX;
    TbFloat32 playerLeftX = TB_FLOAT32_MAX;
    for (PlayerList::Iterator ite = m_Player.Begin(), endIte = m_Player.End();
        ite != endIte; ++ite)
    {
        TbFloat32 x = (*ite)->GetDrawPos().GetX();
        if (playerRightX < x) {
            playerRightX = x;
        }
        if (x < playerLeftX) {
            playerLeftX = x;
        }
    }

    TbFloat32 length = playerRightX - playerLeftX;
    TbFloat32 rest = GetWindowWidth() - length;

    if (velo < 0.0f) {
        if (x + velo < playerLeftX) {
            TbFloat32 tmp = (x + velo) - playerLeftX;
            return TbMax( -rest, tmp );
        }
    }
    else {
        if (playerRightX < x + velo) {
            TbFloat32 tmp = (x + velo) - playerRightX;
            return TbMin( rest , velo);
        }
    }
#endif
    return velo;
}

/*!
 * アクター生成
 * @author teco
 * @since 2013.10.20
 */
void SideActionStage::OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData& userData)
{
    static const TbFloat32 BALL_OFFSET_Y = -78;

    TB_RESQUE(name);
    TbVector2 initPos = pos;
    TbRawString typeNameStr(typeName);

    ActionActor* actor = NULL;
    if( typeNameStr == "Player" ){

        SideActionPlayer* player = createPlayer(&initPos);
        if( !player ){
            return;
        }
        if( userData.AsSint32(0) == 1 ) {
            player->LookLeft();
        }

#if 0
        if( 0 == m_Player.GetCount()%2 ){
            CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HALOWEEN2);
        }else{
            CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HALOWEEN1);
        }
#endif

        actor = player;

    }else if( typeNameStr == "WarpGunPlayer" ){

        SideActionPlayer* player = createPlayer(&initPos);
        if( !player ){
            return;
        }
        if( userData.AsSint32(0) == 1 ) {
            player->LookLeft();
        }
        player->SetOffserUV(TbVector2(0.0f,CMN_TEX_UV(32.0f)));

        CreateActor<SideActionPlayerWarpGun>(initPos,player);
        actor = player;

    }else if( typeNameStr == "SleepPlayer" ){
        SideActionPlayer* player = createPlayer(&initPos);
        if (!player) {
            return;
        }
        if (userData.AsSint32(0) == 1) {
            player->LookLeft();
        }
        if (player->GetPlayerIndex() < Root::GetInstance().GetPlayerCount() / 2) {
            player->Sleep();
        }
        actor = player;

    }else if( typeNameStr == "TitlePlayer" ){

        SideActionPlayer* player = createPlayer(&initPos);
        if( !player ){
            return;
        }
        if( userData.AsSint32(0) == 1 ) {
            player->LookLeft();
        }
        player->SetTitleMode();
        actor = player;
    }
    else if (typeNameStr == "MainMenuPlayer") {

        SideActionPlayer* player = createPlayer(&initPos);
        if (!player) {
            return;
        }
        player->SetMainMenuMode();
        actor = player;

    }else if( typeNameStr == "BreakoutPlayer" ){

        // ボール生成
        TbVector2 ballPos = initPos;
        ballPos.Add( 0.0f , BALL_OFFSET_Y );
        BreakoutBall* ball = CreateActor<BreakoutBall>(ballPos);

        SideActionPlayer* player = createPlayer(&initPos);
        if( player ){
            if( userData.AsSint32(0) == 1 ) {
                player->LookLeft();
            }
            // ヘルメット生成
            CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HELMET);
            ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            actor = player;
        }else{
            if( TB_VERIFY(!m_Player.IsEmpty())) {
                player = m_Player.GetBack();
                player->AddLife();
                ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            }
            return;
        }

    }else if( typeNameStr == "BattleBreakoutPlayer" ||
              typeNameStr == "BattleBreakoutPlayer2"){

        // ボール生成
        TbVector2 ballPos = initPos;
        ballPos.Add( 0.0f , BALL_OFFSET_Y );

        TbStaticString32 ballName = name;
        ballName += "b";
        SideActionBreakoutBattleBall* ball = CreateActorWithName<SideActionBreakoutBattleBall>(ballPos,ballName.GetBuffer());
        if( typeNameStr == "BattleBreakoutPlayer2" ){
            ball->SetType(SideActionBreakoutBattleBall::TYPE_TERRITORY);
        }

        SideActionPlayer* player = createPlayer(&initPos);
        if( player ){
            if( userData.AsSint32(0) == 1 ) {
                player->LookLeft();
            }
            // ヘルメット生成
            CreateActor<SideActionPlayerHeaddress>(initPos,player,SideActionPlayerHeaddress::TYPE_HELMET);
            ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            actor = player;
            ball->SetPlayerIndex( player->GetPlayerIndex() );
        }else if( m_Player.GetCount()%2 == 0 ) {
            if( TB_VERIFY(!m_Player.IsEmpty())) {
                player = m_Player.GetBack();
                player->AddLife();
                ball->SetDeadCallback( BreakoutBallDeadFunc(player) );
            }
            return;
        }

    }else if( typeNameStr == "SwitchPlayer" ){

        SideActionPlayer* player = createSpecialPlayer(&initPos);
        if( !player ){
            return;
        }
        player->SetSwitchInput();
        if( userData.AsSint32(0) == 1 ) {
            player->LookLeft();
        }
        actor = player;

    }else if( typeNameStr == "MultiPlayer" ){

        static const TbSint32 DIRECTION_START_INDEX = 0;
        static const TbSint32 JUMP_VELO_START_INDEX = DIRECTION_START_INDEX+1;
        static const TbSint32 WALK_VELO_START_INDEX = JUMP_VELO_START_INDEX+CMN_PLAYER_MAX;

        SideActionPlayer* player = createPlayer(&initPos);
        if( !player ){
            return;
        }
        if( userData.AsSint32(DIRECTION_START_INDEX) == 1 ) {
            player->LookLeft();
        }
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        player->SetMultiPlayerInput(userData.AsFloat32(JUMP_VELO_START_INDEX+playerCount-1),
                                    userData.AsFloat32(WALK_VELO_START_INDEX+playerCount-1));
        actor = player;

        CreateActor<SideActionPlayerBackdress>(initPos,player,SideActionPlayerBackdress::TYPE_POWERED);

    }else if( typeNameStr == "MajorityPlayer" ){
        static TB_NDEBUG_CONST TbFloat32 MAJORITY_INPUT_RATE = 0.7f;

        // プレイヤー生成
        SideActionPlayer* player = createPlayer(&initPos);
        if( !player ){
            return;
        }
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        TbFloat32 input_rate = 1.0f;
        if( playerCount > 0 ) {
            input_rate = MAJORITY_INPUT_RATE * (static_cast<TbFloat32>(playerCount)/10.0f);
            input_rate = TbClamp(input_rate,0.1f,1.0f);
        }
        player->SetMajorityInput(userData.AsSint32(0),input_rate);
        actor = player;
    }else if( typeNameStr == "Title" ){
        SideActionTitle* title = new SideActionTitle();
        actor = title;
    }else if( typeNameStr == "TallBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_TALL);
        box->SetPushCountMax(userData.AsSint32(0));
        if( userData.AsSint32(1) > 0 ){
            box->SetHint(TB_TRUE);
        }
        actor = box;
    }else if( typeNameStr == "BigBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_BIG);
        box->SetPushCountMax(userData.AsSint32(0));
        actor = box;
    }else if( typeNameStr == "NormalBox" ){
        // 巨大箱生成
        SideActionBigBox* box = new SideActionBigBox();
        box->SetPushCountMax(userData.AsSint32(0));
        actor = box;
    }else if( typeNameStr == "SmallBox" ){
        // 巨大じゃない箱生成
        SideActionBigBox* box = new SideActionBigBox(SideActionBigBox::TYPE_SMALL);
        box->SetPushCountMax(userData.AsSint32(0));
        actor = box;
    }else if (typeNameStr == "PushBox") {
        // 可変サイズ箱生成
        TbFloat32 width  = userData.AsFloat32(1);
        TbFloat32 height = userData.AsFloat32(2);
        TbRectF32Pod rect = { -width/2.0f , -height , width , height };
        SideActionBigBox* box = new SideActionBigBox(rect);
        box->SetPushCountMax(userData.AsSint32(0));
        actor = box;
    }
    else if (typeNameStr == "ColorBox") 
    {
        // カラー箱生成
        TbFloat32 width = 1 < userData.GetCount() ? userData.AsFloat32(1) : 32.0f;
        TbFloat32 height = 2 < userData.GetCount() ? userData.AsFloat32(2) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbSint32 exceptPlayerIndex = -1;
        if (3 < userData.GetCount()) {
            exceptPlayerIndex = userData.AsSint32(3);
        }
        SideActionColorBox* box = new SideActionColorBox(rect , userData.AsSint32(0) , exceptPlayerIndex );
        actor = box;
    }
    else if (typeNameStr == "ForceColorBox") 
    {
        // カラー箱生成
        TbFloat32 width = 1 < userData.GetCount() ? userData.AsFloat32(1) : 32.0f;
        TbFloat32 height = 2 < userData.GetCount() ? userData.AsFloat32(2) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        SideActionColorBox* box = new SideActionColorBox(rect , userData.AsSint32(0) );
        actor = box;
    } else if (typeNameStr == "ColorBlock") {
        // カラー箱生成
        TbSint32 type = userData.AsSint32(0);
        TbFloat32 width = 1 < userData.GetCount() ? userData.AsFloat32(2) : 32.0f;
        TbFloat32 height = 2 < userData.GetCount() ? userData.AsFloat32(3) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbSint32 exceptPlayerIndex = -1;
        if (4 < userData.GetCount()) {
            exceptPlayerIndex = userData.AsSint32(4);
        }
        SideActionColorBlock* block = new SideActionColorBlock(rect , userData.AsSint32(1) , exceptPlayerIndex );
        actor = block;
    }else if (typeNameStr == "ColorBlockConstraint") {
        SideActionColorBlockConstraint* constraint = new SideActionColorBlockConstraint();
        actor = constraint;

        TbFloat32 width = 0 < userData.GetCount() ? userData.AsFloat32(0) : 32.0f;
        TbFloat32 height = 1 < userData.GetCount() ? userData.AsFloat32(1) : 32.0f;
        TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
        TbFloat32 distance = 2 < userData.GetCount() ? userData.AsFloat32(2) : 32.0f;

        constraint->SetDistance(distance);

        SideActionColorBlock* block1 = new SideActionColorBlock(rect,0,-1);
        SideActionColorBlock* block2 = new SideActionColorBlock(rect, 0, -1);
        TbVector2 pos1 = initPos;
        pos1.Add(64.0f,0.0f);
        AddActor(block1, initPos);
        block1->Release();
        TbVector2 pos2 = pos1;
        pos1.Add(-64.0f, 0.0f);
        AddActor(block2, pos2);
        block2->Release();

        constraint->AddBlock(block1);
        constraint->AddBlock(block2);

    }else if( typeNameStr == "StepEnemy" ){
        // ステップエネミー
        SideActionStepEnemy* enemy = new SideActionStepEnemy();
        if( userData.AsSint32(0) < 0 ) {
            enemy->GoToLeft();
        }
        actor = enemy;
    }else if( typeNameStr == "UpDownLift" ){
        // 上下移動リフト
        SideActionUpDownLift* lift = new SideActionUpDownLift();
        TbSint32 height = userData.AsSint32(0);
        if( userData.AsSint32(1) != 0 ){
            initPos.Add(0.0f,userData.AsFloat32(1) * Root::GetInstance().GetPlayerCount());
            height +=userData.AsSint32(1) * Root::GetInstance().GetPlayerCount();
        }
        if( height != 0 ){
            lift->SetHeight(static_cast<TbFloat32>(height));
        }
        actor = lift;
    }else if( typeNameStr == "Lift" ){
        // リフト
        SideActionLift* lift = new SideActionLift();
        actor = lift;
    }else if( typeNameStr == "UpDownEnemy" ){
        // 上下移動エネミー
        actor = new SideActionUpDownEnemy();
    }else if( typeNameStr == "Key" ){
        // 鍵
        SideActionKey* key = new SideActionKey();
        initPos = key->CalcInitPos(initPos,static_cast<TbFloat32>(userData.AsSint32(0)));
        if( userData.AsSint32(1) > 0 ){
            key->SetHint(TB_TRUE);
        }
        if( userData.AsSint32(2) > 0 ){
            key->SetActive(TB_FALSE);
        }
        if( userData.AsString(3) ){
            key->SetSwitchTarget(userData.AsString(3));
        }
        actor = key;
    }else if( typeNameStr == "BreakoutKey" ){
        // 鍵(ブロック崩し用)
        actor = new SideActionKey( SideActionKey::TYPE_BREAKOUT );
    }else if( typeNameStr == "Goal" ){
        // ゴール
        if( !TB_VERIFY(!m_Goal) ) {
            m_Goal->Release();
        }
        m_Goal = new SideActionGoal();
        if( userData.AsSint32(0) > 0 ){
            m_Goal->SetHint(TB_TRUE);
        }
        m_Goal->Retain();
        actor = m_Goal;
    }else if( typeNameStr == "CheckPoint" ){
        // チェックポイント 
        actor = new SideActionCheckPoint();
    }else if( typeNameStr == "BreakoutBall" ){
        // ブロック崩し用ボール
        actor = new BreakoutBall();
    }else if( typeNameStr == "MajorityController" ){
        SideActionPlayer* player = GetPlayer(userData.AsSint32(0));
        // 多数決用コントローラ
        if( TB_VERIFY(player) ){
            actor = new SideActionMajorityController(player);
        }
    }else if( typeNameStr == "WeightedLift" ){
        // 重さで動くリフト
        SideActionWeightedLift* lift = new SideActionWeightedLift();
        if( userData.AsSint32(0) != 0 ){
            TbFloat32 height = userData.AsFloat32(0);
            if( userData.AsSint32(2) != 0 ) {
                height += userData.AsFloat32(2) * Root::GetInstance().GetPlayerCount();
            }
            lift->SetHeight(height);
        }
        if( userData.AsSint32(1) > 0 ){
            lift->SetMoveContactCountParcent(userData.AsSint32(1));
        }
        actor = lift;
    }else if( typeNameStr == "WeightedLiftEx" ){
        // 重さで動くリフト
        SideActionWeightedLift* lift = new SideActionWeightedLift();
        if( userData.AsSint32(0) != 0 ){
            TbFloat32 height = userData.AsFloat32(0);
            TbSint32 offset = userData.AsSint32(3+(Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN));
            if( offset != 0 ) {
                height += offset;
            }
            lift->SetHeight(height);
        }
        if( userData.AsSint32(1) > 0 ){
            lift->SetMoveContactCountParcent(userData.AsSint32(1));
        }
        lift->SetSpeed(userData.AsFloat32(2));
        actor = lift;
    }else if( typeNameStr == "MoveWall" ){
        // 動く壁
        TbFloat32 movWidth = 0.0f;
        TbFloat32 triggerWidth = 0.0f;
        TbFloat32 height = -1.0f;
        if( userData.AsSint32(0) != 0 ){
            movWidth = static_cast<TbFloat32>(userData.AsSint32(0));
        }
        if( userData.AsSint32(1) != 0 ){
            triggerWidth = static_cast<TbFloat32>(userData.AsSint32(1));
        }
        if ( 0.0f < userData.AsFloat32(2) ) {
            height = static_cast<TbFloat32>(userData.AsFloat32(2));
        }
        SideActionMoveWall* wall = new SideActionMoveWall( movWidth , triggerWidth , height);
        actor = wall;
    }else if( typeNameStr == "JumpStand" ){
        TbFloat32 jumpVelo  = static_cast<TbFloat32>(userData.AsSint32(0));
        SideActionJumpStand* stand = new SideActionJumpStand(jumpVelo);
        actor = stand;
    }else if( typeNameStr == "BoundBall" ){
        SideActionBoundBall* ball = new SideActionBoundBall();
        actor = ball;
    }else if( typeNameStr == "BoundBallPitcher" ){
        TbFloat32 deg  = static_cast<TbFloat32>(userData.AsSint32(0));
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_BOUND,deg);
        TbUint32 speedIndex = Root::GetInstance().GetPlayerCount()-1;
        if( userData.AsSint32(1) > 0 && speedIndex > 0 ){
            TbFloat32 speed = userData.AsFloat32(speedIndex)*0.1f;
            pitcher->SetSpeed(speed);
        }
        actor = pitcher;
    }else if( typeNameStr == "LaserBallPitcher" ){
        TbFloat32 deg  = userData.AsFloat32(0);
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_LASER,deg);
        TbFloat32 speed = userData.AsFloat32(1);
        pitcher->SetSpeed(speed);
        actor = pitcher;
    }else if( typeNameStr == "PhysicsBallPitcher" ){
        TbFloat32 deg  = static_cast<TbFloat32>(userData.AsSint32(0));
        SideActionBallPitcher* pitcher = new SideActionBallPitcher(SideActionBallPitcher::TYPE_PHYSICS,deg);
        TbUint32 speedIndex = Root::GetInstance().GetPlayerCount()-1;
        if( userData.AsSint32(1) > 0 && speedIndex > 0 ){
            TbFloat32 speed = userData.AsFloat32(speedIndex)*0.1f;
            pitcher->SetSpeed(speed);
        }
        actor = pitcher;
    }else if( typeNameStr == "Switch" ){
        SideActionSwitch* sw = new SideActionSwitch(SideActionSwitch::TYPE_NORMAL);
        actor = sw;
        if( 0 < userData.AsSint32(0) ) {
            sw->SetEnableRepeat(TB_TRUE);
        }
        sw->SetId(userData.AsSint32(1));
    }else if( typeNameStr == "PhysicsSwitch" ){
        actor = new SideActionSwitch(SideActionSwitch::TYPE_PHYSICS);
    }else if( typeNameStr == "DeadSwitch" ){
        actor = new SideActionDeadSwitch();
    }else if( typeNameStr == "WakeSwitch" ){
        actor = new SideActionWakeSwitch();
    }else if( typeNameStr == "BallBox" ){
        SideActionBallKeyBox* box = new SideActionBallKeyBox();
        TbFloat32 offsetX = static_cast<TbFloat32>(userData.AsSint32(0) * Root::GetInstance().GetPlayerCount());
        initPos.Add( offsetX , 0.0f );
        actor = box;
    }else if( typeNameStr == "LaserKeyBox" ){
        SideActionLaserKeyBox* box = new SideActionLaserKeyBox();
        if( userData.AsString(0)){
            TbUint32 speedIndex = TbMax<TbUint32>(1,Root::GetInstance().GetPlayerCount()-1);
            box->SetLevelUpTarget( userData.AsString(0) , userData.AsFloat32(speedIndex) );
        }
        actor = box;
    }else if( typeNameStr == "Bridge" ){
        TbVector2 dir( static_cast<TbFloat32>(userData.AsSint32(1)) , static_cast<TbFloat32>(userData.AsSint32(2)) );
        dir.Normalize();
        SideActionLink* bridge = new SideActionLink(SideActionLink::TYPE_OUT,userData.AsSint32(0), userData.AsSint32(4),dir);

        TbSint32 initOffsetCount = 0;
        if( userData.AsSint32(3) > 0 ) {
            TbFloat32 lack = static_cast<TbFloat32>(CMN_PLAYER_COUNT - Root::GetInstance().GetPlayerCount());
            initOffsetCount = static_cast<TbSint32>(lack * static_cast<TbFloat32>(userData.AsSint32(3))*0.1f);
        }
        bridge->SetInitOffsetCount(initOffsetCount);
        actor = bridge;
    }else if( typeNameStr == "Gate" ){
        TbVector2 dir( userData.AsFloat32(1) , userData.AsFloat32(2) );
        dir.Normalize();
        SideActionLink* bridge = new SideActionLink(SideActionLink::TYPE_IN,userData.AsSint32(0),0,dir);
        actor = bridge;
    }else if( typeNameStr == "Warp" || typeNameStr == "WarpAll" ){
        TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(0));
        TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(1));
        TbFloat32 destX = static_cast<TbFloat32>(userData.AsSint32(2));
        TbFloat32 destY = static_cast<TbFloat32>(userData.AsSint32(3));
        TbFloat32 destOffsetX = static_cast<TbFloat32>(userData.AsSint32(4));
        TbFloat32 destOffsetY = static_cast<TbFloat32>(userData.AsSint32(5));
        SideActionWarp* warp = new SideActionWarp(w,h);
        warp->SetDestination(TbVector2(destX,destY));
        warp->SetDestOffset(TbVector2(destOffsetX,destOffsetY));
        if (typeNameStr == "WarpAll") {
            warp->SetTargetAll(TB_TRUE);
        }
        actor = warp;
    }else if (typeNameStr == "WarpInitPos") {
        TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(0));
        TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(1));
        SideActionWarp* warp = new SideActionWarp(w, h);
        warp->SetDestInitPos(TB_TRUE);
        actor = warp;
    }else if( typeNameStr == "PhysicsBall" ){
        SideActionPhysicsBall* ball = new SideActionPhysicsBall();
        actor = ball;
    }else if( typeNameStr == "PhysicsArea" ){
        SideActionPhysicsArea* area = new SideActionPhysicsArea();
        TbFloat32 x = static_cast<TbFloat32>(userData.AsSint32(0));
        TbFloat32 y = static_cast<TbFloat32>(userData.AsSint32(1));
        TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(2));
        TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(3));
        area->SetArea( TbRectF32( x , y , w , h ) );
        actor = area;
    }else if( typeNameStr == "PhysicsRect" ){
        SideActionPhysicsRect* area = new SideActionPhysicsRect();
        TbFloat32 x = static_cast<TbFloat32>(userData.AsSint32(0));
        TbFloat32 y = static_cast<TbFloat32>(userData.AsSint32(1));
        TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(2));
        TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(3));
        area->SetRect( TbRectF32( x , y , w , h ) );
        actor = area;
    }else if( typeNameStr == "Seesaw" ){
        SideActionSeesaw* seesaw = new SideActionSeesaw( static_cast<SideActionSeesaw::Type>(userData.AsSint32(0)) );
        if( userData.AsSint32(1) ) {
             SideActionSeesaw* parent = static_cast<SideActionSeesaw*>(FindActor("SeesawParent"));
             if( TB_VERIFY(parent) ) {
                 seesaw->Attach( parent );
             }
        }
        actor = seesaw;
    }else if( typeNameStr == "SeesawParent" ){
        SideActionSeesawParent* seesaw = new SideActionSeesawParent( static_cast<SideActionSeesaw::Type>(userData.AsSint32(0)) );
        seesaw->SetControllRangeX( static_cast<TbFloat32>(userData.AsSint32(1)) , static_cast<TbFloat32>(userData.AsSint32(2)) );
        actor = seesaw;
    }else if( typeNameStr == "RetryTimer" ){
        SideActionTimer* timer = new SideActionTimer( SideActionTimer::TYPE_DOWN );
        timer->SetTimeUpAction(SideActionTimer::TIMEUP_ACTION_RETRY);
        TbFloat32 sec = userData.AsFloat32(0);
        if( 1 < userData.GetCount() ) {
            sec += userData.AsFloat32(1) * Root::GetInstance().GetPlayerCount();
        }
        if( 0.0f < sec ) {
            timer->SetSec( sec );
        }
        actor = timer;
    }else if( typeNameStr == "SwitchTimer" ){
        SideActionTimer* timer = new SideActionTimer( SideActionTimer::TYPE_DOWN );
        timer->SetTimeUpAction(SideActionTimer::TIMEUP_ACTION_SWITCH);
        if( userData.AsSint32(0) > 0 ) {
            timer->SetSec( userData.AsFloat32(0) );
        }
        actor = timer;
    }else if( typeNameStr == "StopWatch" ){
        SideActionStopWatch* timer = new SideActionStopWatch();
        if( userData.AsSint32(0) > 0 ) {
            timer->SetInitSec( userData.AsFloat32(0) );
        }
        if( userData.AsString(1) ){
            timer->SetParentName(userData.AsString(1));
        }
        actor = timer;
    }else if( typeNameStr == "CoopStopWatch" ){
        TbSint32 index = (Root::GetInstance().GetPlayerCount()-2)+1;
        if( 0 <= index ) {
            SideActionCoopStopWatch* watch = new SideActionCoopStopWatch();
            TbFloat32 max = userData.AsFloat32(index) * static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount());
            watch->SetJudgeSec(userData.AsFloat32(0),max);
            actor = watch;
        }
    }else if( typeNameStr == "Watch" ){
//        SideActionWatch* timer = new SideActionWatch();
//        actor = timer;
    }else if( typeNameStr == "Coin" ){
        SideActionCoin* coin = new SideActionCoin();
        // コインオブサーバーが存在してたら監視対象として登録しておく
        if( userData.AsSint32(0) ) {
            SideActionCoinObserver* parent = static_cast<SideActionCoinObserver*>(FindActor("CoinObserver"));
            if( TB_VERIFY(parent) ) {
                parent->AddCoin(coin);
            }
        }
        actor = coin;
    }else if( typeNameStr == "CoinObserver" ){
        SideActionCoinObserver* coinObserver = new SideActionCoinObserver();
        actor = coinObserver;
#if 0
    }else if( typeNameStr == "Marker" ){
        SideActionMarker* marker = new SideActionMarker(SideActionMarker::TYPE_NORMAL );
        actor = marker;
    }else if( typeNameStr == "LeftMarker" ){
        SideActionMarker* marker = new SideActionMarker( SideActionMarker::TYPE_LEFT );
        actor = marker;
    }else if( typeNameStr == "RightMarker" ){
        SideActionMarker* marker = new SideActionMarker( SideActionMarker::TYPE_RIGHT );
        actor = marker;
#endif
    }else if( typeNameStr == "Text" ){
        SideActionText* text = new SideActionText();
        text->SetText(userData.AsString(0));
        if( 2 <= userData.GetCount() ){
            text->SetFontSize(userData.AsSint32(1));
        }
        actor = text;
    }else if( typeNameStr == "Balance" ){
        TbFloat32 interval = static_cast<TbFloat32>(userData.AsSint32(0));
        SideActionBalance* marker = new SideActionBalance(interval);
        actor = marker;
    }else if( typeNameStr == "HeightJudge" ){
        SideActionJudgeHeight* judge = new SideActionJudgeHeight();
        actor = judge;
    }else if( typeNameStr == "TerritoryJudge" ){
        SideActionJudgeTerritory* judge = new SideActionJudgeTerritory(GetMap().GetTileWidth(),GetMap().GetTileHeight());
        actor = judge;
    }else if( typeNameStr == "BreakoutJudge" ){
        SideActionJudgeBreakout* judge = new SideActionJudgeBreakout(GetMap().GetTileWidth(),GetMap().GetTileHeight());
        actor = judge;
    }else if( typeNameStr == "TerritoryBallJudge" ){
        SideActionJudgeBreakout* judge = new SideActionJudgeBreakout(GetMap().GetTileWidth(),GetMap().GetTileHeight());
        judge->SetType(SideActionJudgeBreakout::TYPE_TERRITORY);
        actor = judge;
    }else if( typeNameStr == "Ghost" ){
        SideActionGhost* ghost = new SideActionGhost();
        TbFloat32 borderRate = userData.AsFloat32(0);
        TbFloat32 border = static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount()) * borderRate;
        ghost->SetMoveBorder(border);
        if( userData.AsSint32(1) ){
            ghost->LookLeft();
        }
        actor = ghost;
    }else if( typeNameStr == "CollisionSwitch" ){
        TbFloat32 w = userData.AsFloat32(0);
        TbFloat32 h = userData.AsFloat32(1);
        const char* targetName = userData.AsString(2);
        SideActionCollisionSwitch* sw = new SideActionCollisionSwitch(w,h);
        sw->SetTagetName(targetName);
        actor = sw;
    }else if( typeNameStr == "Wind" ){
        SideActionWind* wind = new SideActionWind();
        initPos.Add(0.0f,userData.AsFloat32(0) * Root::GetInstance().GetPlayerCount());

        TbSint32 index = TbMax<TbSint32>(1,Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN+1);
        TbFloat32 power = userData.AsFloat32(index);
        wind->SetPower(power);

        actor = wind;
    }else if( typeNameStr == "DistanceConstraint" ){
        SideActionDistanceConstraint* constraint = new SideActionDistanceConstraint();
        TbUint32 distanceIndex       = ((Root::GetInstance().GetPlayerCount()-2))*2;
        TbUint32 gravityTensionIndex = distanceIndex+1;
        if (userData.GetCount() <= distanceIndex) {
            distanceIndex = 0;
        }
        if (userData.GetCount() <= gravityTensionIndex) {
            gravityTensionIndex = 1;
        }

        TbFloat32 distance = userData.AsFloat32(distanceIndex);
        constraint->SetDistance(distance);
        if( gravityTensionIndex < userData.GetCount() ){
            TbFloat32 gravityTension = userData.AsFloat32(gravityTensionIndex);
            constraint->SetGracityTension(gravityTension);
        }
        actor = constraint;
    }else if( typeNameStr == "Puzzle" ){
        SideActionTetris* tetris = new SideActionTetris();

        TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
        TbScriptLuaNode*    node   = script.GetVM(0)->GetNode(0);
        if( 2 < userData.GetCount() ) {
            tetris->SetNorma(userData.AsSint32(2));
        }
        tetris->InitializeByScript(userData.AsString(0));
        tetris->SetSwitchTarget(userData.AsString(1));
        actor = tetris;

    }else if( typeNameStr == "LaunchPad" ){
        
        SideActionLaunchPad* pad = new SideActionLaunchPad(-5.0f);
        actor = pad;

    }else if( typeNameStr == "KeyGoal" ){

        // 鍵ゴール
        SideActionKeyProxyGoal* keyGoal = new SideActionKeyProxyGoal();
        actor = keyGoal;

    }else if( typeNameStr == "RouletteManager" ){

        // ルーレット
        SideActionRouletteManager* roulette = new SideActionRouletteManager();
        roulette->SetMax( m_RouletteCount );
        
        actor = roulette;

    }else if( typeNameStr == "Roulette" ){

        TbUint32 minPlayer = userData.AsSint32(0);
        if( minPlayer <= Root::GetInstance().GetPlayerCount() ){

            // ルーレット
            SideActionRoulette* roulette = new SideActionRoulette();
        
            // WORDを追加
            roulette->SetAnswer(userData.AsSint32(1));
            TbUint32 count = userData.GetCount();
            for( TbUint32 i = 2; i < count; ++i ) {
                roulette->AddCandidate(userData.AsString(i));
            }

            actor = roulette;
            ++m_RouletteCount;
        }

    }else if( typeNameStr == "BlinkBlock" ){

        // 点滅ブロック
        SideActionBlinkBlock* block = new SideActionBlinkBlock();
        
        // WORDを追加
        actor = block;

    }else if( typeNameStr == "TrafficLight" ){

        // 点滅ブロック
        SideActionTrafficLight* block = new SideActionTrafficLight();
        
        // WORDを追加
        actor = block;

    } else if (typeNameStr == "Rect") {

        TbUint32 playerOffsetCount = Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN;

        // 点滅ブロック
        TbDimensionF32 size(userData.AsFloat32(0), userData.AsFloat32(1));
        size.width  += playerOffsetCount * userData.AsFloat32(2);
        size.height += playerOffsetCount * userData.AsFloat32(3);
        TbRectF32 rect(0, -size.height, size.width, size.height);
        SideActionRect* target = new SideActionRect(rect);
        actor = target;

    }
    else if (typeNameStr == "ScrollLimitRange") {

        TbFloat32 width = userData.AsFloat32(0);
        actor = new SideActionScrollLimitRange(width);

    }else{
        TB_ASSERT_MSG_FORMAT(0,"Invalid Name (%s)",name);
    }
    if( actor ) {
        actor->SetName(name);
        AddActor(actor,initPos);
        actor->Release();
    }
}


/*!
 * スクロール更新
 * @author teco
 * @since 2013.10.19
 */
void SideActionStage::updateScroll()
{
    static TbFloat32 SCROLL_BEGIN_X = 0.3f;
    static TbFloat32 SCROLL_BEGIN_WEIGHT = 0.6f;
    static TbFloat32 SCROLL_END_X = 0.5f;

    // プレイヤーの左右端位置を計算する
    TbFloat32 playerRightX = -TB_FLOAT32_MAX;
    TbFloat32 playerLeftX = TB_FLOAT32_MAX;
    TbFloat32 rightVelo = 0.0f;
    TbFloat32 leftVelo = 0.0f;
    for (PlayerList::Iterator ite = m_Player.Begin(), endIte = m_Player.End();
        ite != endIte; ++ite)
    {
        TbFloat32 x = (*ite)->GetPosX();
        TbFloat32 vx = x - (*ite)->GetPrePos().GetX();
        if (playerRightX < x) {
            playerRightX = x;
        }
        if (x < playerLeftX) {
            playerLeftX = x;
        }
        if (vx < 0.0f) {
            leftVelo = TbMin(leftVelo, vx);
        }
        else if (0.0f < vx) {
            rightVelo = TbMax(rightVelo, vx);
        }
    }
    m_PlayerRangeX.begin = playerLeftX;
    m_PlayerRangeX.end = playerRightX;

    if( !m_BitArray.Test(FLAG_SCROLLABLE) ){
        return;
    }

#if 1
    ActionMap& map = GetMap();
    TbFloat32 windowWidth = GetWindowWidth();
    TbFloat32 mapWidth = static_cast<TbFloat32>(map.GetWidth());
    TbFloat32 right = GetScroll() + windowWidth;

    TbFloat32 scrollBeginBorderR = GetScroll() + windowWidth*SCROLL_BEGIN_X;
    TbFloat32 scrollEndBorderR = GetScroll() + windowWidth*SCROLL_END_X;
       
    TbFloat32 scrollBeginBorderL = GetScroll() + windowWidth*(1.0f-SCROLL_BEGIN_X);
    TbFloat32 scrollEndBorderL = GetScroll() + windowWidth*(1.0f-SCROLL_END_X);

    TbFloat32 scrollMaxR = mapWidth - right; // 可能なスクロール量
    TbFloat32 scrollMaxL = -GetScroll(); // 可能なスクロール量

    // スクロール量
    TbFloat32 scroll = 0.0f;

    if (m_BitArray.Test(FLAG_AUTO_SCROLL))
    {
        // 自動スクロール
        TbBool isEnable = TB_TRUE;

        // 死亡ステートに入っている人がいたら停止
        for (PlayerList::ConstIterator ite = m_Player.Begin(), endIte = m_Player.End();
            ite != endIte;
            ++ite)
        {
            if ((*ite)->GetState() == SideActionPlayer::STATE_DEAD) {
                isEnable = TB_FALSE;
                break;
            }
        }
        if(isEnable )
        {
            TbFloat32 right = GetScroll() + windowWidth;
            TbFloat32 scrMax = mapWidth - right; // 可能なスクロール量
                                                    // スクロールの限界値を超えないように
            scroll = TbMin(scrMax, m_AutoScrollSpeed);
        }
    }
    else
    // まず一番右にいるキャラの位置でスクロールが可能かどうか決める
    {
        if( 0.0f <= m_RequestScroll )
        {
            static TB_NDEBUG_CONST TbFloat32 REQ_AUTO_SCROLL = 3.0f;
            if( GetScroll() < m_RequestScroll ) {
                TbFloat32 scr = REQ_AUTO_SCROLL;
                scroll = TbMin(scr, m_RequestScroll- GetScroll() )+0.01f;
            }else{
                m_RequestScroll = -1.0f;
            }
        }
        else
        {
            // 右スクロール
            if( playerRightX >= scrollEndBorderR ) {
                TbFloat32 scale = 1.0f+1.0f*(playerRightX-scrollEndBorderR)/(windowWidth-(windowWidth*SCROLL_END_X));
                TbFloat32 scr = TbMax( 0.0f , rightVelo * scale);
                scroll += scr;
            }else if( playerRightX > scrollBeginBorderR ) {
                scroll += TbMax( 0.0f , (rightVelo) * SCROLL_BEGIN_WEIGHT );
            }

            // 左スクロール
            if (playerLeftX <= scrollEndBorderL) {
                TbFloat32 scale = 1.0f + 1.0f*(scrollEndBorderL-playerLeftX) / (scrollEndBorderL);
                TbFloat32 scr = 0.0f;
                scr = TbMin(0.0f, leftVelo * scale);
                scroll += scr;
            }
            else if (playerLeftX > scrollBeginBorderL) {
                scroll += TbMin(0.0f, leftVelo * SCROLL_BEGIN_WEIGHT);
            }
        }

        // スクロール
        if( !TB_FLOAT32_EQUAL_ZERO(scroll) ) {

            // スクロールの限界値を超えないように
            scroll = TbClamp(scroll , scrollMaxL, scrollMaxR);

            // 左端と右端にいるキャラが見切れないように調整
            TbFloat32 leftOffset = playerLeftX - GetScroll();
            scroll = TbMin( leftOffset , scroll );
            TbFloat32 rightOffset = playerRightX - (windowWidth+GetScroll());
            scroll = TbMax( rightOffset , scroll );
            if (0.0f <= m_ScrollLimit) {
                TbFloat32 limit = m_ScrollLimit - GetScroll();
                scroll = TbMin(limit, scroll);
            }
        }
    }

    AddScroll(scroll);
    GetMap().SetScroll(GetScroll());
#endif
}

/*!
 * ステージクリア更新
 * @author teco
 * @since 2013.10.23
 */
void SideActionStage::updateClear()
{
    m_ClearedPlayerCount = 0;
    // 全プレイヤーがクリア状態になったかどうか
    for( PlayerList::ConstIterator ite = m_Player.Begin() , endIte = m_Player.End();
         ite != endIte;
         ++ite ) 
    {
        if( (*ite)->IsCleared() ) {
            ++m_ClearedPlayerCount;
        }
    }
}

/*!
 * ゲームオーバー
 * @author teco
 * @since 2013.10.27
 */
TbBool SideActionStage::isGameOver() const
{
    if( m_BitArray.Test(FLAG_DISABLE_GAMEOVER) ){
        return TB_FALSE;
    }

    TbBool result = TB_TRUE;
    for( PlayerList::ConstIterator ite = m_Player.Begin() , endIte = m_Player.End();
         ite != endIte;
         ++ite ) 
    {
        if( !(*ite)->IsGameOver() ) {
            result = TB_FALSE;
            break;
        }
    }
    if( IsRequestedGameOver() ){
        result = TB_TRUE;
    }
    return result;
}

/*!
 * プレイヤー生成
 * @author teco
 */
SideActionPlayer* SideActionStage::createPlayer( TbVector2* initPos )
{
    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    if( m_BitArray.Test(FLAG_LIMIT_PLAYER_COUNT) &&
        m_Player.GetCount() >= playerCount )
    {
        return NULL;
    }

    // プレイヤー生成
    TbUint32 playerIndex = m_Player.GetCount();
    SideActionPlayer* player = new SideActionPlayer(playerIndex);
    player->Retain();
    TB_VERIFY(m_Player.PushBack(player));

    // チェックポイントチェック
    TbVector2 checkPos;
    TbFloat32 checkPosSec = 0.0f;
    if( Root::GetInstance().GetGlobalData().GetCheckPoint(checkPos,checkPosSec,GetId(),player->GetPlayerIndex()) ){
        if( initPos ){
            SetPlaySec(checkPosSec);
            *initPos = checkPos;
        }
    }
    return player;
}

/*!
 * 特殊プレイヤー生成
 * @author teco
 */
SideActionPlayer* SideActionStage::createSpecialPlayer( TbVector2* initPos )
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

