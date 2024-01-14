/*!
 * ステージシーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_stage.h"
#include "seq_manager.h"
#include "seq_world_select.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "puzzle/stage/puzzle_tetris_stage.h"
#include "puzzle/stage/puzzle_dr_stage.h"
#include "puzzle/stage/puzzle_vs_dr_stage.h"
#include "puzzle/stage/puzzle_vs_tetris_stage.h"
#include "breakout/stage/breakout_stage.h"
#include <crayon/input/cr_input.h>
#include "common/cmn_util.h"
#include "root.h"

#include <base/io/tb_file_all.h>
#include <base/util/tb_bind.h>
#include <graphics/render/3dapi/tb_render_device.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>
#include <graphics/render/tb_draw_2d.h>

static const TbFloat32 FIRST_SEC = 2.0f;
static const TbFloat32 IN_SEC = 1.0f;
static const TbFloat32 END_SEC = 4.0f;

static const TbRectF32Pod ICON_RECT[] = 
{
    {0.0f,},
    {-22.0f , -24.0f , 44.0f , 48.0f},
    {-24.0f , -24.0f , 48.0f , 48.0f},
    {-24.0f , -24.0f , 48.0f , 48.0f},
    {-24.0f , -24.0f , 48.0f , 48.0f},
    {-22.0f , -24.0f , 44.0f , 48.0f},

    {-24.0f , -24.0f , 48.0f , 48.0f},
    {-24.0f , -24.0f , 48.0f , 48.0f},
    {-24.0f , -24.0f , 48.0f , 48.0f},
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ICON_RECT)==SeqStage::STAGE_TYPE_MAX);

static const TbRectF32Pod ICON_UV[] = 
{
    { 0.0f, },
    { CMN_TEX_UV_RECT(32*5+5,8,22,24) },
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(32*5+5,8,22,24) },

    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ICON_UV)==SeqStage::STAGE_TYPE_MAX);


/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStage::SeqStage( TbSint32 id )
    : Super(SEQ_STAGE,"SeqStage")
    , m_Stage(nullptr)
    , m_Id(id < 0 ? 100+id : id)
    , m_Phase()
    , m_ConfirmWindow(UiMessageWindow::TYPE_YESNO)
    , m_NextSeq(SEQ_MAX)
    , m_NextSeqArg(0)
{
    m_Phase = PHASE_FIRST;
    setupStage();
    m_Pause.SetDecidedCallback(TbCreateMemFunc(this,&Self::onDecidedPause));
    m_Pause.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
    m_Pause.SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
    if ( WorldManager::GetInstance().IsLastStageInAllWorld(id) ) {
        m_BitArray.SetBit(FLAG_LAST_STAGE);
    }
//    m_BitArray.SetBit(FLAG_LAST_STAGE);
    if ( isBattleStage() ) {
        m_Pause.SetBattleMode();
        m_ConfirmWindow.SetPos(TbVector2(CMN_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 2.0f));
        m_ConfirmWindow.SetAlign(TB_UI_ALIGN_X_CENTER, TB_UI_ALIGN_Y_CENTER);
        m_ConfirmWindow.SetMessage("FINISH BATTLE?");
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStage::~SeqStage()
{
    TB_SAFE_DELETE(m_Stage);
}

/*!
 * ポーズ切り替え
 * @author teco
 * @since 2014.12.27
 */
void SeqStage::setPause( TbBool isPause , TbSint32 playerIndex ) 
{
    m_BitArray.SetBit(FLAG_PAUSE,isPause);
    if( isPause ){
        // ポーズ
        Root::GetInstance().SetEnableTranslucent(TB_TRUE );
        // ポーズメニュー
        if(0 <= playerIndex){
            m_Pause.SetPlayerBit(TB_BIT(playerIndex));
        }
        m_Pause.Open();
    }else{
        Root::GetInstance().SetEnableTranslucent(TB_FALSE );
    }
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.07
 */
void SeqStage::OnUpdate()
{
#if TB_IS_DEBUG && TB_PLATFORM_IS_WIN
    if( TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsOn(TB_KEY_F) && TbKeyboard::IsPress(TB_KEY_O) )
    {
        TbStaticString256 path = getStageScriptPath();
        CmnUtil::OpenTextFileByEditor(path.GetBuffer(), TB_FILE_PATH_ID_PRESET_ROOT);
    }
#endif

#if TB_IS_DEBUG // オートテスト用
    if(Root::GetInstance().IsAutoTest()){
        WorldManager& worldManager = WorldManager::GetInstance();
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
        if (3.0f <= m_Phase.timer) {
            TbSint32 nextStageId = worldManager.GetNextStage(m_Id);
            
            if (nextStageId < 0) {
                if (playerCount < CMN_PLAYER_COUNT - 1) {
                    Root::GetInstance().SetPlayerCount(playerCount + 1);
                    nextStageId = worldManager.GetWorldInfo(0)->stage[0].id;
                    if (worldManager.IsWorldStage(m_Id)) {
                        nextStageId = worldManager.GetWorldInfo(0)->stage[0].id;
                    }
                    else if (worldManager.IsBattleStage(m_Id)) {
                        nextStageId = worldManager.GetBattleStageInfo(0)->stage;
                    }
                    else if (worldManager.IsEndlessStage(m_Id)) {
                        nextStageId = worldManager.GetEndlessStageInfo(0)->stage;
                    }
                }
            }

            if (0 <= nextStageId) {
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE, nextStageId);
            }else {
                Root::GetInstance().SetPlayerCount(CMN_PLAYER_MIN);
                if (worldManager.IsWorldStage(m_Id)) {
                    nextStageId = worldManager.GetBattleStageInfo(0)->stage;
                    SeqManager::GetInstance().RequestSequence(SEQ_STAGE, nextStageId);
                }else if (worldManager.IsBattleStage(m_Id)) {
                    nextStageId = worldManager.GetEndlessStageInfo(0)->stage;
                    SeqManager::GetInstance().RequestSequence(SEQ_STAGE, nextStageId);
                }else if (worldManager.IsEndlessStage(m_Id)) {
                    SeqManager::GetInstance().RequestSequence(SEQ_TITLE, 0);
                }
            }

        }
    }
#endif

    if( !m_Pause.IsOpened() ){
        if( CrInput::GetBool(CR_INPUT_ID_BOOL_SYSTEM_PAUSE) ){
            m_BitArray.SetBit(FLAG_PAUSE,!m_BitArray.Test(FLAG_PAUSE));
            return;
        }
    }
    if( m_BitArray.Test(FLAG_PAUSE) ){
        return;
    }
    m_Phase.timer += 1.0f/60.0f;

    switch( m_Phase.val )
    {
    case PHASE_FIRST:
        {
            const char* bgm = "bgm";
            WorldManager& worldManager = WorldManager::GetInstance();
            static const TbSint32 BGM_SHOOT_TABLE[] =
            {
                41 ,
                43 ,
                55 ,
            };
            if( TbEnd(BGM_SHOOT_TABLE) != TbFind(BGM_SHOOT_TABLE,m_Id) )
            {
                bgm = "bgm_shoot";
            }
            else if( worldManager.IsWorldStage(m_Id) )
            {
                if( worldManager.GetWorldFromStageId(m_Id,nullptr) == 0 )
                {
                    bgm = "bgm_tutorial";
                }
            }
            CreateBgm(bgm);
            FadeIn(0.0f);
            ++m_Phase;
            break;
        }
    case PHASE_FIRST+1:
        {
            if (!IsReadyBgm()) {
                break;
            }
            if( m_Phase.timer >= FIRST_SEC ) {
                PlayBgm();
                m_Phase = PHASE_STAGE;
            }
        }
        break;
    case PHASE_STAGE:
        if( m_Stage ){
            SaveFile::GetInstance().IncrementPlayCount(m_Id,Root::GetInstance().GetPlayerCount());
            m_Stage->SetActive(TB_TRUE);
            updateStage();
        }
        ++m_Phase;
        break;
    case PHASE_STAGE+1:
#if TB_IS_DEBUG
        if (TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_E)) {
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_EDIT, m_Id);
        }
        if (TbKeyboard::IsOn(TB_KEY_CONTROL_L) && TbKeyboard::IsPress(TB_KEY_C)) {
//            TbPlaySound("draw", 1.0f);
            m_Phase = PHASE_CLEAR;
        }
#endif
        if( CrInput::GetBool(CR_INPUT_ID_BOOL_RETRY ) ){
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id);
        }else if( CrInput::GetBool(CR_INPUT_ID_BOOL_FORCE_NEXT) ){
            if( !m_BitArray.Test(FLAG_LAST_STAGE) ){
                // 次のステージへ
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id+1);
            }else{
                // タイトル画面へ
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE);
            }
        }else{
#if !CMN_ENABLE_BUILD_GIANT_INPUT
            if( isEnablePause() )
            {
                for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_MAX; ++playerIndex ){
                    if( !CrInput::IsPress(CrInput::BUTTON_START,playerIndex) ) {
                        continue;
                    }
                    setPause(TB_TRUE,playerIndex);
                    return;
                }
            }
#endif
            updateStage();
        }
        break;
    case PHASE_CLEAR:
        if( m_Phase.timer >= END_SEC ){
            SaveFile::GetInstance().IncrementClearCount(m_Id,Root::GetInstance().GetPlayerCount());
            
            TbFloat32 fadeSec = 0.5f;
            if(m_BitArray.Test(FLAG_LAST_STAGE)){
                fadeSec = 4.0f;
            }
            FadeOut(fadeSec);
            FadeOutBgm(fadeSec);
            ++m_Phase;
        }
        break;
    case PHASE_CLEAR+1:
        {
            if (Root::GetInstance().IsBusyUserData()) {
                break;
            }
            if (IsFading()) {
                break;
            }
#if CMN_ENABLE_BUILD_OLD
            // ステージ
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT,m_Id+1);
#elif CMN_ENABLE_BUILD_GIANT_INPUT
            if( !m_BitArray.Test(FLAG_LAST_STAGE) ){
                // ステージ
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id+1);
            }else{
                // タイトル画面へ
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE);
            }
#else
            if (m_BitArray.Test(FLAG_LAST_STAGE)) {
                ++m_Phase;
            }else{
                WorldManager& worldManager = WorldManager::GetInstance();
                TbBool isLastStageInWorld = worldManager.IsLastStageInWorld(m_Id);
                TbSint32 resumeStage = m_Id;
                TbBool isFirstClear = (1 == SaveFile::GetInstance().GetTotalClearCount(m_Id));
                if (!isLastStageInWorld && isFirstClear) {
                    resumeStage = worldManager.GetNextStage(m_Id);
                }
                SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, resumeStage);
            }
#endif
        }

        break;
    case PHASE_CLEAR+2:
        {
            if(Root::GetInstance().IsBusyUserData()){
                break;
            }
            SeqManager::GetInstance().RequestSequence(SEQ_ENDING, 0);
        }
        break;

    case PHASE_GAMEOVER:
        if( m_Phase.timer >= END_SEC ){
#if CMN_ENABLE_BUILD_OLD
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT);
#elif CMN_ENABLE_BUILD_GIANT_INPUT
            SeqManager::GetInstance().RequestSequence(SEQ_TITLE);
#else
            SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, m_Id);
#endif
        }
        break;

    case PHASE_RETRY:
        {
            TbFloat32 fadeSec = 0.5f;
            FadeOut(fadeSec);
            FadeOutBgm(fadeSec);
            ++m_Phase;
        }
        break;
    case PHASE_RETRY+1:
        {
            if (IsFading()) {
                break;
            }
            // 同じシーケンスを再リクエスト
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE, m_Id);
        }
        break;

    case PHASE_CONFIRM_FINISH:
        m_ConfirmWindow.SetPlayerBit(m_Pause.GetPlayerBit());
        m_ConfirmWindow.Open();
        ++m_Phase;
        break;
    case PHASE_CONFIRM_FINISH+1:
        if (m_ConfirmWindow.IsOpened()) {
            break;
        }
        if( m_ConfirmWindow.GetResult() == UiMessageWindow::RESULT_YES) {
            m_Phase = PHASE_CHANGE_SEQ;
        } else {
            setPause(TB_TRUE,-1);
            m_Phase = PHASE_STAGE+1;
        }
        break;
    case PHASE_CHANGE_SEQ:
        {
            TB_ASSERT(m_NextSeq!=SEQ_MAX);
            TbFloat32 fadeSec = 0.5f;
            FadeOut(fadeSec);
            FadeOutBgm(fadeSec);
            ++m_Phase;
        }
        break;
    case PHASE_CHANGE_SEQ+1:
        {
            if( IsFading() ){
                break;
            }
            Root::GetInstance().SetEnableTranslucent(TB_FALSE);
            SeqManager::GetInstance().RequestSequence(m_NextSeq,m_NextSeqArg);
        }
        break;
    }
}

/*!
 * 描画関数
 * @author teco
 * @since 2013.10.27
 */
void SeqStage::OnDraw() 
{
    if ( m_Phase.val < PHASE_STAGE ){
        // ステージ番号
        TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        
        TbSint32 stageIndex = m_Id;
#if !CMN_ENABLE_BUILD_OLD
        WorldManager& worldManager = WorldManager::GetInstance();
        TbSint32 index = -1;
        TbStaticString64 text;
        if (worldManager.IsBattleStage(m_Id, &index))
        {
            text.SetFormatedString("ROUND %d",BattleManager::GetInstance().GetRound()+1);
        }
        else if (worldManager.IsEndlessStage(m_Id, &index))
        {
            const EndlessStageInfo* info = worldManager.GetEndlessStageInfo(index);
            if( info ){
                text.SetFormatedString("%s", info->name.GetBuffer());
            }
        }
        else
        {
            TbSint32 world = worldManager.GetWorldFromStageId(m_Id,&index);
            const WorldInfo* info = worldManager.GetWorldInfo(world);
            if (info)
            {
//                text.SetFormatedString("LEVEL %d", info->stage[index].number);
                text.SetFormatedString("LEVEL %d-%d", world+1 , index+1 );
            }
            else
            {
                text.SetFormatedString("LEVEL %d", stageIndex + 1);
            }
        }
        // TbSint32 world = WorldManager::GetInstance().GetWorldFromStageId(m_Id,&stageIndex);
        fontBuffer->Printf(
            static_cast<TbSint32>(CMN_WINDOW_WIDTH / 2),
            static_cast<TbSint32>(CMN_WINDOW_HEIGHT / 3), text.GetBuffer());
#else
        fontBuffer->Printf( static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                            static_cast<TbSint32>(CMN_WINDOW_HEIGHT/3),"STAGE - %d", stageIndex +1);
#endif
        fontBuffer->PopSetting();

        // ステージアイコン
        m_PlayerMaterial.BeginDraw();
        TbRectF32 rect = ICON_RECT[m_Type];
        rect = rect * 2.0f;
        rect.x += CMN_WINDOW_WIDTH/2;
        rect.y += CMN_WINDOW_HEIGHT/2;
        m_PlayerMaterial.Draw(rect, ICON_UV[m_Type],CmnUtil::GetPlayerColor(0),TbMatrix::IDENTITY);
        m_PlayerMaterial.EndDraw();
    }else{
        if( m_Stage ){
            m_Stage->Draw();
        }
        switch( m_Phase.val )
        {
        case PHASE_CLEAR:
        case PHASE_GAMEOVER:
            {
                TbFloat32 posX = TbMin( 1.0f ,  m_Phase.timer/IN_SEC );
                posX *= posX + CMN_WINDOW_WIDTH/2;
                TbFloat32 posY = CMN_WINDOW_HEIGHT/2;
                const char* test = m_Phase.val == PHASE_CLEAR ? "CLEAR" : "THANK YOU FOR PLAYING";
#if 0
                if( 20 == m_Stage->GetId() ) {
                    test = "HAPPY 8BIT CAFE";
                }else{
                }
#endif
                TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
                fontBuffer->PushSetting();
                fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
                fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
                fontBuffer->SetEnableBorder(TB_TRUE);
                // よくないけど今回はこれで
                SideActionStage* saStage = TbDynamicCast<SideActionStage*>(m_Stage);
                if(saStage){
                    fontBuffer->SetColor(saStage->GetClearFontColor());
                }
                fontBuffer->Printf( static_cast<TbSint32>(posX),
                                    static_cast<TbSint32>(CMN_WINDOW_HEIGHT/2),test);
                fontBuffer->PopSetting();
            }
        }
    }
}

// ステージのスクリプトパス取得
TbStaticString256 SeqStage::getStageScriptPath()
{
    SeqManager& seqManager = SeqManager::GetInstance();
    TbStaticString32 scriptName = seqManager.GetStageName(m_Id);
    const char* dirName = seqManager.GetStageDir(m_Id);

    // 個別luaを読み込み
    TbStaticString32 dir;
    if (dirName && dirName[0]) {
        dir += dirName;
        dir += "/";
    }
    else {
        dir += CMN_STAGE_DIR;
    }

    TbStaticString256 path = TbStaticString256::Format("stage/%s%s.lua", dir.GetBuffer(), scriptName.GetBuffer());
    return path;
}

/*!
 * ステージセットアップ
 * @author teco
 * @since 2013.10.27
 */
void SeqStage::setupStage()
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);

    // 共通luaを読み込み
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");

    SeqManager& seqManager = SeqManager::GetInstance();
    TbStaticString32 name = seqManager.GetStageName(m_Id);
    TbStaticString256 path = getStageScriptPath();
    res = node->DoFileSync(path.GetBuffer());
    if( TB_VERIFY( res == TbScriptLuaNode::RESULT_OK ) )
    {
        TbSint32 type = 0;
        TbSint32 width = 0;
        node->GoToAbsolutely(name+".type");
        if( !TB_VERIFY(TbScriptLuaNode::RESULT_OK==node->TryToSint32(type)) ) {
            return;
        }

        m_Type = static_cast<StageType>(type);
        node->GoToAbsolutely(name);
        switch( type )
        {
        case STAGE_TYPE_SIDE_ACTION:
            {
                m_Stage = new SideActionStage(node);
            }
            break;
        case STAGE_TYPE_PUZZLE_TETRIS: // テトリス
            {
                m_Stage = new PuzzleTetrisStage(node,PuzzleTetrisStage::BLOCK_TYPE_4);
            }
            break;
        case STAGE_TYPE_PUZZLE_TETRIS3: // 3個テトリス
            {
                m_Stage = new PuzzleTetrisStage(node,PuzzleTetrisStage::BLOCK_TYPE_3);
            }
            break;
        case STAGE_TYPE_PUZZLE_DR: // ドクマリ
            {
                m_Stage = new PuzzleDrStage(node);
            }
            break;
        case STAGE_TYPE_PUZZLE_VS_DR: // 対戦ドクマリ
            {
                m_Stage = new PuzzleVsDrStage(node);
            }
            break;
        case STAGE_TYPE_PUZZLE_VS_TETRIS: // 対戦テトリス
            {
                m_Stage = new PuzzleVsTetrisStage(node,PuzzleVsTetrisStage::TYPE_4);
            }
            break;
        case STAGE_TYPE_PUZZLE_VS_TETRIS3: // 対戦3個テトリス
            {
                m_Stage = new PuzzleVsTetrisStage(node,PuzzleVsTetrisStage::TYPE_3);
            }
            break;
        case STAGE_TYPE_BREAKOUT:
            {
                m_Stage = new BreakoutStage(node);
            }
            break;
        }
        if( m_Stage ) {
            m_Stage->SetId(m_Id);
        }
    }
}

/*!
 * ステージ更新
 * @author teco
 * @since 2013.10.27
 */
void SeqStage::updateStage()
{
    if( m_Stage ){
        m_Stage->Update();


#if TB_IS_DEBUG
        if(Root::GetInstance().IsAutoTest()){
            return;
        }
#endif

        // リクエストチェック
        WorldManager& worldManager = WorldManager::GetInstance();
        if(worldManager.IsWorldStage(m_Stage->GetId()) )
        {
            // ワールドステージの時だけ有効
            if( m_Stage->IsRequestedAny() ){
                if( m_Stage->IsRequestedClear() ){  // クリア
                    // ベストタイムか
                    TbBool isBestTime = SaveFile::GetInstance().SetBestMillSec(
                                            m_Id,
                                            Root::GetInstance().GetPlayerCount(),
                                            m_Stage->GetPlayMillSec());
                    m_BitArray.SetBit(FLAG_BEST_TIME,isBestTime);
                    m_Phase = PHASE_CLEAR;
//                    FadeOutBgm(0.016f);

                    SideActionStage* saStage = TbDynamicCast<SideActionStage*>(m_Stage);
                    if (saStage->IsDarkness()) {
                        saStage->ResetDarkness();
                    }

//                    TbPlaySound("whistle", 2.0f);
                }else if( m_Stage->IsRequestedGameOver() ){ // ゲームオーバー
                    m_Phase = PHASE_GAMEOVER;
                }else if( m_Stage->IsRequestedRetry() ){
                    m_Phase = PHASE_RETRY;
                }
            }
        }
    }
}

/*! 
 * ポーズ画面の選択時
 * @author teco
 * @since 2014.12.27
 */
void SeqStage::onDecidedPause( UiPauseWindow::Menu result )
{
    setPause(TB_FALSE);
    switch( result )
    {
    case UiPauseWindow::MENU_RETURN_GAME:
        break;
    case UiPauseWindow::MENU_RETURN_STAGE_SELECT:
        {
            SeqManager& seqManager = SeqManager::GetInstance();
#if CMN_ENABLE_BUILD_OLD
            requestNextSeq(SEQ_STAGE_SELECT, m_Id);
#else
            if( isBattleStage() ) {
                requestNextSeq(SEQ_BATTLE_SELECT, m_Id);
            }else if( isEndlessStage() ) {
                requestNextSeq(SEQ_ENDLESS_SELECT, m_Id);
            }else {
                requestNextSeq(SEQ_WORLD_SELECT, m_Id);
            }
#endif
        }
        break;
    case UiPauseWindow::MENU_RETURN_MAIN_MENU:
        requestNextSeq(SEQ_MAIN_MENU, 0);
        if( isBattleStage()) {
            m_Phase = PHASE_CONFIRM_FINISH;
        }
        break;
    case UiPauseWindow::MENU_RETURN_TITLE:
        requestNextSeq(SEQ_TITLE, 1);
        if (isBattleStage()) {
            m_Phase = PHASE_CONFIRM_FINISH;
        }
        break;
    case UiPauseWindow::MENU_RETRY:
        requestNextSeq(SEQ_STAGE, m_Id);
        break;
    case UiPauseWindow::MENU_JOIN:
        {
            // プレイヤー数を増やした上でリトライ
            TbUint32 newPlayerIndex = Root::GetInstance().GetPlayerCount();
            TbUint32 newPlayerPadIndex = CrInput::GetPlayerPadIndex(TbBitMostSignificant(m_Pause.GetPlayerBit()));
            CrInput::SetPlayerPadIndex(newPlayerIndex,newPlayerPadIndex);
            Root::GetInstance().SetPlayerCount(newPlayerIndex+1);
            requestNextSeq(SEQ_STAGE, m_Id);
        }
        break;
    }
}

/*! 
 * エンドレスステージか
 * @author teco
 */
TbBool SeqStage::isEndlessStage() const
{
    WorldManager& worldManager = WorldManager::GetInstance();
    return worldManager.IsEndlessStage(m_Id,nullptr);
}

/*! 
 * バトルステージか
 * @author teco
 * @since 2014.12.27
 */
TbBool SeqStage::isBattleStage() const
{
    WorldManager& worldManager = WorldManager::GetInstance();
    return worldManager.IsBattleStage(m_Id, nullptr);
}

/*! 
 * ポーズ可能か
 * @author teco
 */
TbBool SeqStage::isEnablePause() const
{
    SideActionStage* saStage = TbDynamicCast<SideActionStage*>(m_Stage);
    if (saStage)
    {
        // プレイヤーがサスペンド状態だったら出せなくする
        if (isBattleStage()) {
            if (saStage->GetPlayer(0))
            {
                if (saStage->GetPlayer(0)->IsSuspended()) {
                    return TB_FALSE;
                }
            }
        }
        return saStage->IsEnablePause();
    }
    return TB_TRUE;
}

/*! 
 * 次シーケンス設定
 * @author teco
 */
void   SeqStage::requestNextSeq(SeqType seq, TbUint32 arg) 
{
    Root::GetInstance().SetEnableTranslucent(TB_TRUE);
    m_NextSeq = seq;
    m_NextSeqArg = arg;
    m_Phase = PHASE_CHANGE_SEQ;
}
