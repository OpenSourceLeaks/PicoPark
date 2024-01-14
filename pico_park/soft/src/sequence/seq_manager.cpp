/*!
 * ルート
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_manager.h"
#include "world_manager.h"
#include "seq_title.h"
#include "seq_stage.h"
#include "seq_stage_select.h"
#include "seq_ending.h"
#include "seq_ending_trial.h"
#include "seq_endless_select.h"
#include "seq_main_menu.h"
#include "seq_world_select.h"
#include "seq_battle_select.h"
#include "seq_battle_result.h"
#include "seq_stage_edit.h"
#include "root.h"
#include "common/cmn_util.h"
#include <framework/debug/menu/tb_debug_menu_all.h>

#if CMN_ENABLE_BUILD_GIANT_INPUT
    const char SETTING_FILE_PATH[] = "seq_setting_giant.lua";
#elif CMN_ENABLE_BUILD_TETRIS
    const char SETTING_FILE_PATH[] = "seq_setting_tetris.lua";
#elif CMN_ENABLE_BUILD_ALPHA
    const char SETTING_FILE_PATH[] = "seq_setting_alpha.lua";
#elif CMN_ENABLE_BUILD_ARCADE
    const char SETTING_FILE_PATH[] = "seq_setting_arcade.lua";
#else
    const char SETTING_FILE_PATH[] = "seq_setting.lua";
#endif

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.05
 */
SeqManager::SeqManager()
    : m_CurrentSeq(NULL)
    , m_RequestSeq(NULL)
    , m_StageCount(0)
    , m_ActiveStageCount(0)
    , m_BgmRefCount(0)
    , m_Bgm(nullptr)
    , m_Fade(nullptr)
#if SEQ_IS_ENABLE_DEBUG_MENU
    , m_DebugMenu(NULL)
    , m_TypeItem(NULL)
    , m_ArgItem(NULL)
    , m_StartButtonItem(NULL)
#endif
{
    TbFillNull(m_CacheSeq);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.05
 */
SeqManager::~SeqManager()
{
    Reset();
    TB_ASSERT(!m_Bgm);
}

/*!
 * 初期化
 * @author teco
 * @since 2014.10.13
 */
void SeqManager::Initialize()
{
    // シーケンス設定取得
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
    
    TbScriptLuaNode::Result res = node->DoFileSync(SETTING_FILE_PATH);
    if( TB_VERIFY( res == TbScriptLuaNode::RESULT_OK ) )
    {
        enum
        {
            PARAM_NAME , 
            PARAM_TYPE , 
            PARAM_DIR ,
            PARAM_MANY
        };
        node->TryToUint32(m_ActiveStageCount,"activeStageCount");
        if( TbScriptLuaNode::RESULT_OK == node->GoToChild("stageList") ){
            node->TryToTableLength(m_StageCount);
            TB_ASSERT(0<=m_ActiveStageCount&&m_ActiveStageCount<=m_StageCount);
            for( TbUint32 i = 0; i < m_StageCount; ++i ){
                const char* name = NULL;
                node->GoToChild(i);
                TbUint32 length = 0;
                node->TryToTableLength(length);
                if( TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name,static_cast<TbSint32>(PARAM_NAME)) ){
                    m_StageInfo[i].name = name;
                }
                TbSint32 type;
                if( TbScriptLuaNode::RESULT_OK == node->TryToSint32ByIndex(type, PARAM_TYPE) ){
                    m_StageInfo[i].type = static_cast<CmnStageType>(type);
                }
                if (TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name, PARAM_DIR)) {
                    m_StageInfo[i].dir = name;
                }
                if ( 3 < length && TbScriptLuaNode::RESULT_OK == node->GoToChild(PARAM_MANY))
                {
                    TbSint32 border = 0;
                    if (TbScriptLuaNode::RESULT_OK == node->TryToSint32ByIndex(border, 0)) {
                        m_StageInfo[i].manyBorder = border;
                    }
                    if (TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name, 1)) {
                        m_StageInfo[i].manyName = name;
                    }
                    node->GoToParent(1);
                }
                node->GoToParent(1);
            }
            node->GoToParent(1);
        }
    }
    WorldManager::GetInstance().Initialize(node);
    m_Fade = new CmnFade();
}

/*!
 * リセット
 * @author teco
 * @since 2014.06.14
 */
void SeqManager::Reset()
{
    TB_SAFE_KILL(m_RequestSeq);
    TB_SAFE_KILL(m_CurrentSeq);
    TB_SAFE_DELETE(m_Fade);
    ClearCacheSeq();
}

/*! 
 * 更新
 * @author teco
 * @since 2013.09.06
 */
void SeqManager::Update()
{
    if( m_RequestSeq && m_RequestSeq->IsReadyForUpdate() ) {
        if ( !m_CurrentSeq || IsCacheSeq(m_CurrentSeq)) {
            m_CurrentSeq = m_RequestSeq;
            m_CurrentSeq->SetPause(TB_FALSE);
            m_CurrentSeq->SetEnableUpdate();
            m_RequestSeq = NULL;
        } else if( m_CurrentSeq->IsFinished() ) {
            TB_SAFE_KILL(m_CurrentSeq);
            m_CurrentSeq = m_RequestSeq;
            m_CurrentSeq->SetPause(TB_FALSE);
            m_CurrentSeq->SetEnableUpdate();
            m_RequestSeq = NULL;
        }
#if SEQ_IS_ENABLE_DEBUG_MENU
        if (!m_RequestSeq) {
            m_CurrentSeq->OnCreateDebugMenu(m_DebugMenu);
        }
#endif
    }
    if(m_Fade ){
        m_Fade->Update();
    }
    m_BgmFade.Update(1/60.0f);
}

/*!
 * 現在のシーケンス設定
 * @author teco
 * @since 2013.09.06
 */
TbResult SeqManager::RequestSequence( SeqType type , TbSint32 arg )
{
    if( m_RequestSeq ) {
        TB_SAFE_KILL(m_RequestSeq);
    }
    if (m_CacheSeq[type]) {
        // キャッシュチェック
        if (TB_VERIFY(type == SEQ_MAIN_MENU))
        {
            // 今はメインメニューのみ許容
            m_RequestSeq = m_CacheSeq[type];
        }
        m_CacheSeq[type] = nullptr;
    }
    else {
        m_RequestSeq = createSeq(type, arg);
    }
    m_RequestUserData = UserData();
    if( m_RequestSeq ){
        if( m_CurrentSeq ){
            if (IsCacheSeq(m_CurrentSeq)) {
                m_CurrentSeq->SetPause(TB_TRUE);
            }else{
                m_CurrentSeq->RequestFinish();
            }
        }
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

/*!
 * シーケンス遷移リクエスト用ユーザーデータ設定
 * @author teco
 */
void SeqManager::SetRequestUserDataU32(TbUint32 param, TbUint32 index )
{
    TB_RESQUE(index==0);
    m_RequestUserData.paramU32 = param;
}

/*!
 * シーケンス遷移リクエスト用ユーザーデータ取得
 * @author teco
 */
TbUint32 SeqManager::GetRequestUserDataU32(TbUint32 index) const
{
    TB_RESQUE_RET(index == 0,0);
    return m_RequestUserData.paramU32;
}

/*!
 * キャッシュ用シーケンス追加
 * @author teco
 */
void     SeqManager::AddCacheSeq(SeqBase* seq)
{
    TB_RESQUE(seq);
    SeqType type = seq->GetType();
    TB_RESQUE(!m_CacheSeq[type]);
    if (seq) {
        m_CacheSeq[type] = seq;
        seq->Reset();
    }
}

/*!
 * キャッシュシーケンスクリア
 * @author teco
 */
void     SeqManager::ClearCacheSeq()
{
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_CacheSeq); ++i)
    {
        TB_SAFE_KILL(m_CacheSeq[i]);
    }
}

/*!
 * キャッシュ用シーケンスか
 * @author teco
 */
TbBool    SeqManager::IsCacheSeq(const SeqBase* seq) const
{
    TB_RESQUE_RET(seq,TB_FALSE);
    SeqType type = seq->GetType();
    return m_CacheSeq[type] == seq;
}

/*!
 * BGM生成
 * @author teco
 */
TbSoundSourceHandle* SeqManager::CreateBgm(const char* bgm)
{
    TbSoundId id = TbSoundManager::GetInstance().GetSoundId(bgm);
    if (m_Bgm && m_Bgm->GetSoundId() == id) {
        // 同じだったら何もしない
        ++m_BgmRefCount;
        return m_Bgm;
    }
    TB_RESQUE_RET(!m_Bgm,nullptr);
    m_Bgm = new TbSoundSourceHandle(id);
    m_BgmRefCount = 1;
    if (m_Bgm->IsError()) {
        TB_SAFE_DELETE(m_Bgm);
    }
    else {
        m_Bgm->SetEnableLoop(TB_TRUE);
    }
    if (m_Bgm)
    {
        m_BgmFade.ResetSource();
        m_BgmFade.AddSource(m_Bgm);
    }
    return m_Bgm;
}

/*!
* BGM取得
* @author teco
*/
TbSoundSourceHandle* SeqManager::GetBgm()
{
    return m_Bgm;
}

/*!
 * BGM音量設定
 * @author teco
 */
void SeqManager::SetBgmVolume(TbFloat32 volume)
{
    if (m_Bgm)
    {
        m_Bgm->SetVolume(volume);
    }
}

/*!
 * BGM破棄
 * @author teco
 */
void SeqManager::DeleteBgm()
{
    if( 0 < m_BgmRefCount ){
        --m_BgmRefCount;
    }
    if( 0 == m_BgmRefCount ){
        m_Bgm->SetVolume(0.0f);
        TB_SAFE_DELETE(m_Bgm);
        m_BgmFade.ResetSource();
    }
}

/*!
 * フェードイン
 * @author teco
 */
void SeqManager::FadeIn(TbFloat32 sec)
{
    if(m_Fade){
        m_Fade->FadeIn(sec);
    }
}

/*!
 * フェードアウト
 * @author teco
 */
void SeqManager::FadeOut(TbFloat32 sec)
{
    if(m_Fade){
        m_Fade->FadeOut(sec);
    }
}

/*!
 * BGMフェードアウト
 * @author teco
 */
void SeqManager::FadeOutBgm(TbFloat32 sec)
{
    m_BgmFade.Fade(0.0f,sec);
}

/*!
 * フェード中か
 * @author teco
 */
TbBool SeqManager::IsFading() const
{
    if(m_Fade){
        return m_Fade->IsFading();
    }
    return TB_FALSE;
}

/*!
 * ステージ名取得
 * @author teco
 */
TbStaticString32 SeqManager::GetStageName( TbSint32 stageIndex ) const
{
    const char* stageName = nullptr;
    if( 0 <= stageIndex && stageIndex < m_StageCount ){
        stageName = m_StageInfo[stageIndex].name.GetBuffer();
        if (0 < m_StageInfo[stageIndex].manyBorder) {
            if (m_StageInfo[stageIndex].manyBorder <= Root::GetInstance().GetPlayerCount())
            {
                stageName = m_StageInfo[stageIndex].manyName.GetBuffer();
            }
        }
    }
    TbStaticString32 result;
    if (stageName) {
        result = stageName;
    }else {
        result = TbStaticString32::Format("stage_%03d", stageIndex);
    }
    return result;
}

/*!
 * ステージフォルダ取得
 * @author teco
 */
const char* SeqManager::GetStageDir(TbSint32 stageIndex) const
{
    if (0 <= stageIndex && stageIndex <m_StageCount) {
        return m_StageInfo[stageIndex].dir.GetBuffer();
    }
    return nullptr;
}

/*!
 * ステージ番号取得
 * @author teco
 */
TbSint32 SeqManager::GetStageIndex( const char* name ) const
{
    for( TbUint32 i = 0; i < m_StageCount; ++i ){
        if(m_StageInfo[i].name == name ){
            return i;
        }
    }
    return -1;
}

void SeqManager::CreateDebugMenu( TbDebugMenu* parent )
{
#if SEQ_IS_ENABLE_DEBUG_MENU
    TbDebugMenuManager* menuManager = TbDebugMenuManager::GetInstance();
    if( !menuManager ){
        return;
    }
    if( parent ) {
        m_DebugMenu = parent->CreateItemMenu("Sequence");
    }else{
        m_DebugMenu = menuManager->CreateMenu("Sequence");
    }

    // タイプ
    m_TypeItem = m_DebugMenu->CreateItemAlias("Type");
    TbDebugMenuItemAlias::AliasInfo alias[] = 
    {
        { SEQ_TITLE          , "Title" } ,
        { SEQ_STAGE_SELECT   , "StageSelect" } ,
        { SEQ_STAGE          , "Stage" } ,
        { SEQ_ENDING         , "Ending" } ,
        { SEQ_MAIN_MENU      , "MainMenu" } ,
        { SEQ_WORLD_SELECT   , "WorldSelect" } ,
        { SEQ_BATTLE_SELECT  , "BattleSelect" } ,
        { SEQ_BATTLE_RESULT  , "BattleResult" } ,
        { SEQ_ENDLESS_SELECT , "EndlessSelect" } ,
        { SEQ_STAGE_EDIT     , "StageEdit" } ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(alias)==SEQ_MAX);
    m_TypeItem->SetAliasList( alias , TB_ARRAY_COUNT_OF(alias) );

    // 引数
    m_ArgItem = m_DebugMenu->CreateItemS32("Arg");
    m_ArgItem->SetInitValue(0);

    // 引数
    m_StartButtonItem = m_DebugMenu->CreateItemButton("Start");
    m_StartButtonItem->SetClickedCallback( TbCreateMemFunc(this,&Self::onClickedStart) );
#endif
}

/*!
 * シーケンス生成
 * @author teco
 * @since 2013.09.06
 */
SeqBase* SeqManager::createSeq( SeqType type , TbSint32 arg ) 
{
    // プレイヤー数が0人設定だったら最大設定に切り替える
    if( Root::GetInstance().GetPlayerCount() == 0 ){
        Root::GetInstance().SetPlayerCount(CMN_PLAYER_MAX);
    }
    SeqBase* result = NULL;
//    TbPrintf("Create Sequence(%d,%d)\n",type,arg);
    switch( type )
    {
    case SEQ_TITLE:
        result = new SeqTitle(arg==1||arg==3,arg<2);
        break;
    case SEQ_STAGE_SELECT:
        {
            TbSint32 index = TbClamp<TbSint32>(arg,0,m_ActiveStageCount-1);
            result = new SeqStageSelect(index,m_ActiveStageCount);
        }
        break;
    case SEQ_STAGE:
        result = new SeqStage(arg);
        break;
    case SEQ_ENDING:
#if 0 || CMN_ENABLE_BUILD_EVENT
        result = new SeqEndingTrial();
#else
        result = new SeqEnding();
#endif
        break;
    case SEQ_MAIN_MENU:
        result = new SeqMainMenu();
        break;
    case SEQ_WORLD_SELECT:
        result = new SeqWorldSelect(arg);
        break;
    case SEQ_BATTLE_SELECT:
        result = new SeqBattleSelect(arg);
        break;
    case SEQ_BATTLE_RESULT:
        result = new SeqBattleResult(arg);
        break;
    case SEQ_ENDLESS_SELECT:
        result = new SeqEndlessSelect(arg);
        break;
    case SEQ_STAGE_EDIT:
        result = new SeqStageEdit(arg);
        break;
    }
    return result;
}

/*!
 * スタート開始
 * @author teco
 * @since 2013.09.07
 */
void SeqManager::onClickedStart()
{
#if SEQ_IS_ENABLE_DEBUG_MENU
    if( m_ArgItem && m_TypeItem ){
        RequestSequence(static_cast<SeqType>(m_TypeItem->GetValue()),
                        m_ArgItem->GetValue());
    }
#endif
}