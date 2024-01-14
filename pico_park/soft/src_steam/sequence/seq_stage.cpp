/*!
 * ステージシーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_stage.h"
#include "seq_manager.h"
#include "side_action/stage/side_action_stage.h"
#include "puzzle/stage/puzzle_tetris_stage.h"
#include "puzzle/stage/puzzle_dr_stage.h"
#include "puzzle/stage/puzzle_vs_dr_stage.h"
#include "puzzle/stage/puzzle_vs_tetris_stage.h"
#include "breakout/stage/breakout_stage.h"
#include "system/sys_input.h"
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
    { CMN_TEX_UV_RECT(32*5+5,3,22,24) },
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(0,352,12,12) } ,
    { CMN_TEX_UV_RECT(32*5+5,3,22,24) },

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
SeqStage::SeqStage( TbSint32 id , const char* scriptName )
    : Super(SEQ_STAGE,"SeqStage")
    , m_Stage(nullptr)
    , m_Id(id)
    , m_Phase()
{
    m_Phase = PHASE_FIRST;
    setupStage(scriptName);
    m_Pause.SetDecidedCallback(TbCreateMemFunc(this,&Self::onDecidedPause));
    m_Pause.SetPos(TbVector2(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/2.0f));
    m_Pause.SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);

    // 最後のIDでラストか暫定判定
    if (m_Id == SeqManager::GetInstance().GetActiveStageCount() - 1) {
        SetLastStage();
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
void SeqStage::setPause( TbBool isPause , TbUint32 playerIndex ) 
{
    m_BitArray.SetBit(FLAG_PAUSE,isPause);
    if( isPause ){
        // ポーズ
        Root::GetInstance().SetEnableTranslucent(TB_TRUE );
        // ポーズメニュー
        m_Pause.SetPlayerBit(TB_BIT(playerIndex));
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
    if( !m_Pause.IsOpened() ){
        if( SysInput::GetBool(SYS_INPUT_ID_BOOL_SYSTEM_PAUSE) ){
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
            CreateBgm("bgm");
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
            Root::GetInstance().GetGlobalData().IncrementPlayCount(m_Id);
            m_Stage->SetActive(TB_TRUE);
        }
        ++m_Phase;
        break;
    case PHASE_STAGE+1:
        if( SysInput::GetBool(SYS_INPUT_ID_BOOL_RETRY ) ){
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id);
        }else if( SysInput::GetBool(SYS_INPUT_ID_BOOL_FORCE_NEXT) ){
            if( !m_BitArray.Test(FLAG_LAST_STAGE) ){
                // 次のステージへ
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id+1);
            }else{
                // タイトル画面へ
                SeqManager::GetInstance().RequestSequence(SEQ_TITLE);
            }
        }else{
#if !CMN_ENABLE_BUILD_GIANT_INPUT
            for( TbUint32 playerIndex = 0; playerIndex < CMN_PLAYER_MAX; ++playerIndex ){
                if( !SysInput::IsPress(SysInput::BUTTON_START,playerIndex) ) {
                    continue;
                }
                setPause(TB_TRUE,playerIndex);
                return;
            }
#endif
            updateStage();
        }
        break;
    case PHASE_CLEAR:
        if( m_Phase.timer >= END_SEC ){
            Root::GetInstance().GetGlobalData().IncrementClearCount(m_Id);
            Root::GetInstance().GetGlobalData().SetWorldStageClearFlag(0,m_Id);
            Root::GetInstance().SaveUserData();
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
            if (!m_BitArray.Test(FLAG_LAST_STAGE)) {
                SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT,m_Id+1);
            }else{
                FadeOut(1.0f);
                ++m_Phase;
            }
#endif
        }
        break;
    case PHASE_CLEAR+1:
        {
            if( IsFading() ){
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
            SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, m_Id+1);
#endif
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
        fontBuffer->Printf( static_cast<TbSint32>(CMN_WINDOW_WIDTH/2),
                            static_cast<TbSint32>(CMN_WINDOW_HEIGHT/3),"STAGE - %d",m_Id+1);
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
                if( 20 == m_Stage->GetId() ) {
                    test = "HAPPY 8BIT CAFE";
                }else{
                }

                TbFontBuffer2d* fontBuffer = Root::GetInstance().GetFont();
                fontBuffer->PushSetting();
                fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
                fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
                // 超暫定
                if (m_Stage->GetId() == 3 ) {
                    fontBuffer->SetColor(TbColor::White());
                }
                fontBuffer->Printf( static_cast<TbSint32>(posX),
                                    static_cast<TbSint32>(CMN_WINDOW_HEIGHT/2),test);
                fontBuffer->PopSetting();
            }
        }
    }
}

/*!
 * ステージセットアップ
 * @author teco
 * @since 2013.10.27
 */
void SeqStage::setupStage( const char* scriptName )
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);

    // 共通luaを読み込み
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");

    // 個別luaを読み込み
    TbStaticString128 name = scriptName;
    TbStaticString128 path = TbStaticString128::Format("stage/" CMN_STAGE_DIR "%s.lua",name.GetBuffer());
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

        // リクエストチェック
        if( m_Stage->IsRequestedAny() ){
            if( m_Stage->IsRequestedClear() ){  // クリア
                // ベストタイムか
                TbBool isBestTime = Root::GetInstance().GetGlobalData().SetBestMillSec(
                                        m_Id,
                                        Root::GetInstance().GetPlayerCount(),
                                        m_Stage->GetPlayMillSec());
                m_BitArray.SetBit(FLAG_BEST_TIME,isBestTime);
                m_Phase = PHASE_CLEAR;
            }else if( m_Stage->IsRequestedGameOver() ){ // ゲームオーバー
                m_Phase = PHASE_GAMEOVER;
            }else if( m_Stage->IsRequestedRetry() ){
                // 同じシーケンスを再リクエスト
                SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id);
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
#if CMN_ENABLE_BUILD_OLD
        SeqManager::GetInstance().RequestSequence(SEQ_STAGE_SELECT, m_Id);
#else
        SeqManager::GetInstance().RequestSequence(SEQ_WORLD_SELECT, m_Id + 1);
#endif
        break;
    case UiPauseWindow::MENU_RETURN_TITLE:
        SeqManager::GetInstance().RequestSequence(SEQ_TITLE,1);
        break;
    case UiPauseWindow::MENU_RETRY:
        SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id);
        break;
    case UiPauseWindow::MENU_JOIN:
        {
            // プレイヤー数を増やした上でリトライ
            TbUint32 newPlayerIndex = Root::GetInstance().GetPlayerCount();
            TbUint32 newPlayerPadIndex = SysInput::GetPlayerPadIndex(TbBitMostSignificant(m_Pause.GetPlayerBit()));
            SysInput::SetPlayerPadIndex(newPlayerIndex,newPlayerPadIndex);
            Root::GetInstance().SetPlayerCount(newPlayerIndex+1);
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE,m_Id);
        }
        break;
    }
}
