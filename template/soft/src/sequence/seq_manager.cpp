/*!
 * ルート
 * @author teco
 */

#include "fwd.h"
#include "seq_manager.h"
#include "seq_main.h"
#include "root.h"
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <framework/sub/tb_framework_simple.h>

const char SETTING_FILE_PATH[] = "seq_setting.lua";

/*!
 * コンストラクタ
 * @author teco
 */
SeqManager::SeqManager()
    : m_CurrentSeq(NULL)
    , m_RequestSeq(NULL)
#if SEQ_IS_ENABLE_DEBUG_MENU
    , m_DebugMenu(NULL)
    , m_TypeItem(NULL)
    , m_ArgItem()
    , m_StartButtonItem(NULL)
#endif
{
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
}

/*! 
 * 更新
 * @author teco
 * @since 2013.09.06
 */
void SeqManager::Update()
{
    if( m_RequestSeq && m_RequestSeq->IsReadyForUpdate() ) {
        TbFrameworkSimple::FadeIn(0.0f);
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
}

/*!
 * 現在のシーケンス設定
 * @author teco
 * @since 2013.09.06
 */
TbResult SeqManager::RequestSequence( SeqType type , TbSint32 arg1 , TbSint32 arg2 )
{
    if( m_RequestSeq ) {
        TB_SAFE_DELETE(m_RequestSeq);
    }
    m_RequestSeq = createSeq(type,arg1,arg2);
    if( m_RequestSeq ){
        if( m_CurrentSeq ){
            m_CurrentSeq->RequestFinish();
        }
        return TB_S_OK;
    }
    return TB_E_INVALIDARG;
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
        { SEQ_MAIN        , "Main" } ,
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(alias)==SEQ_MAX);
    m_TypeItem->SetAliasList( alias , TB_ARRAY_COUNT_OF(alias) );

    // 引数
    for( TbUint32 i = 0; i < ARG_MAX; ++i )
    {
        TbStaticString32 name;
        name.SetFormatedString("Arg%u",i+1);
        m_ArgItem[i] = m_DebugMenu->CreateItemS32(name.GetBuffer());
        m_ArgItem[i]->SetInitValue(0);
    }

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
SeqBase* SeqManager::createSeq( SeqType type , TbSint32 arg1 , TbSint32 arg2 ) 
{
    SeqBase* result = NULL;
    switch( type )
    {
    case SEQ_MAIN:
        result = new SeqMain(arg1);
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
                        m_ArgItem[0]->GetValue(),
                        m_ArgItem[1]->GetValue());
    }
#endif
}