/*!
 * ルート
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_manager.h"
#include "seq_title.h"
#include "seq_stage.h"
#include "seq_stage_select.h"
#include "seq_ending.h"
#include "seq_main_menu.h"
#include "seq_world_select.h"
#include "root.h"
#include <framework/debug/menu/tb_debug_menu_all.h>

#if CMN_ENABLE_BUILD_GIANT_INPUT
    const char SETTING_FILE_PATH[] = "seq_setting_giant.lua";
#elif CMN_ENABLE_BUILD_TETRIS
    const char SETTING_FILE_PATH[] = "seq_setting_tetris.lua";
#elif CMN_ENABLE_BUILD_8BITCAFE
    const char SETTING_FILE_PATH[] = "seq_setting_8bitcafe.lua";
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
    TbFill(m_StageType,CMN_STAGE_TYPE_BEST_TIME);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.05
 */
SeqManager::~SeqManager()
{
    Reset();
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
        node->TryToUint32(m_ActiveStageCount,"activeStageCount");
        if( TbScriptLuaNode::RESULT_OK == node->GoToChild("stageList") ){
            node->TryToTableLength(m_StageCount);
            TB_ASSERT(0<=m_ActiveStageCount&&m_ActiveStageCount<=m_StageCount);
            for( TbUint32 i = 0; i < m_StageCount; ++i ){
                const char* name = NULL;
                node->GoToChild(i);
                if( TbScriptLuaNode::RESULT_OK == node->TryToStringByIndex(&name,static_cast<TbSint32>(0)) ){
                    m_StageName[i] = name;
                }
                TbSint32 type;
                if( TbScriptLuaNode::RESULT_OK == node->TryToSint32ByIndex(type,1) ){
                    m_StageType[i] = static_cast<CmnStageType>(type);
                }
                
                node->GoToParent(1);
            }
        }
    }
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
}

/*! 
 * 更新
 * @author teco
 * @since 2013.09.06
 */
void SeqManager::Update()
{
    if( m_RequestSeq && m_RequestSeq->IsReadyForUpdate() ) {
        if ( !m_CurrentSeq ) {
            m_CurrentSeq = m_RequestSeq;
            m_RequestSeq->SetEnableUpdate();
            m_RequestSeq = NULL;
        } else if( m_CurrentSeq->IsFinished() ) {
            TB_SAFE_KILL(m_CurrentSeq);
            m_CurrentSeq = m_RequestSeq;
            m_RequestSeq->SetEnableUpdate();
            m_RequestSeq = NULL;
        }
#if SEQ_IS_ENABLE_DEBUG_MENU
        m_CurrentSeq->OnCreateDebugMenu(m_DebugMenu);
#endif
    }
    if(m_Fade ){
        m_Fade->Update();
    }
}

/*!
 * 現在のシーケンス設定
 * @author teco
 * @since 2013.09.06
 */
TbResult SeqManager::RequestSequence( SeqType type , TbSint32 arg )
{
    if( m_RequestSeq ) {
        TB_SAFE_DELETE(m_RequestSeq);
    }
    m_RequestSeq = createSeq(type,arg);
    if( m_RequestSeq ){
        if( m_CurrentSeq ){
            m_CurrentSeq->RequestFinish();
        }
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
}

/*!
 * BGM生成
 * @author teco
 */
TbSoundSource* SeqManager::CreateBgm(const char* bgm)
{
    TbSoundId id = TbSoundManager::GetInstance().GetSoundId(bgm);
    if (m_Bgm && m_Bgm->GetSoundId() == id) {
        // 同じだったら何もしない
        ++m_BgmRefCount;
        return m_Bgm;
    }
    TB_RESQUE_RET(!m_Bgm,nullptr);
    m_Bgm = new TbSoundSource(id);
    m_BgmRefCount = 1;
    if (m_Bgm->IsError()) {
        TB_SAFE_DELETE(m_Bgm);
    }
    else {
        m_Bgm->SetEnableLoop(TB_TRUE);
    }
    return m_Bgm;
}

/*!
* BGM取得
* @author teco
*/
TbSoundSource* SeqManager::GetBgm()
{
    return m_Bgm;
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
        TB_SAFE_DELETE(m_Bgm);
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
const char* SeqManager::GetStageName( TbSint32 stageIndex ) const
{
    if( 0 <= stageIndex && stageIndex <TB_ARRAY_COUNT_OF(m_StageName) ){
        return m_StageName[stageIndex].GetBuffer();
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
        if( m_StageName[i] == name ){
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
        { SEQ_TITLE        , "Title" } ,
        { SEQ_STAGE_SELECT , "StageSelect" } ,
        { SEQ_STAGE        , "Stage" } ,
        { SEQ_ENDING       , "Ending" } ,
        { SEQ_MAIN_MENU    , "MainMenu" } ,
        { SEQ_WORLD_SELECT , "WorldSelect" } ,
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
    TbPrintf("Create Sequence(%d,%d)\n",type,arg);
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
        if( 0 <= arg && arg < static_cast<TbSint32>(m_StageCount) ) {
            SeqStage* stage = new SeqStage(arg,m_StageName[arg].GetBuffer());
            if( arg == m_StageCount-1 ){
                stage->SetLastStage();
            }
            result = stage;
        }else{
            if( arg < 0 ) {
                arg = 100 + arg;
            }
            TbStaticString32 name = TbStaticString32::Format("stage_%03d",arg);
            result = new SeqStage(arg,name.GetBuffer());
        }
        break;
    case SEQ_ENDING:
        result = new SeqEnding();
        break;
    case SEQ_MAIN_MENU:
        result = new SeqMainMenu();
        break;
    case SEQ_WORLD_SELECT:
        result = new SeqWorldSelect(arg);
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