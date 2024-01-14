/*!
 * @file
 * @brief タスク管理クラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/task/tb_task_manager.h"
#include "base/task/tb_task.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @param taskMax タスク最大数
 * @author Miyake Shunsuke
 */
TbTaskManager::TbTaskManager( const TbUint32 taskLineCount )
    : m_BitArray()
    , m_AddTaskNodeTop( NULL )
    , m_TaskLineInfo( taskLineCount )
    , m_TaskLineInfoSystem()
{
    LineInfo info;
    m_TaskLineInfo.PushBack(taskLineCount,info);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbTaskManager::~TbTaskManager()
{
    //追加タスクを削除しておく
    if( m_AddTaskNodeTop ){
        TbTaskNode* node = m_AddTaskNodeTop;
        TbTaskNode* next = NULL;
        do {
            next = node->next;
            TB_DELETE node;
            node = next;
        } while( node != m_AddTaskNodeTop );
        m_AddTaskNodeTop = NULL;
    }

    // 自殺志願登録されているアスクはここで削除
    killTask();

    // 残っているタスクノードを削除
    TbUint32 userLineCount = m_TaskLineInfo.GetCount();
    for( TbUint32 i = 0; i < userLineCount; ++i ){
        deleteTaskNode( static_cast<TbTaskLine>(i) );
    }
    for( TbUint32 i = TB_TASK_LINE_SYSTEM_BEGIN; i < TB_TASK_LINE_SYSTEM_END; ++i ){
        deleteTaskNode( static_cast<TbTaskLine>(i) );
    }
}

/*!
 * 毎フレーム処理
 * @author Miyake Shunsuke
 */
void TbTaskManager::Update()
{
    //追加タスクを実際に、ここで追加
    if( m_AddTaskNodeTop ){
        TbTaskNode* node = m_AddTaskNodeTop;
        TbTaskNode* next = NULL;
        do {
            next = node->next;
            node->prev = NULL;
            node->next = NULL;
            addTaskNode(node);
            node = next;
        } while( node != m_AddTaskNodeTop );
        m_AddTaskNodeTop = NULL;
    }

    // 自殺ラインに入っているタスクを削除
    killTask();
}

// 自殺タスクを明示的に殺す
void TbTaskManager::Kill()
{
    Update();
}

/*!
 * タスク実行
 * @param task    Line 実行タスクライン
 * @author Miyake Shunsuke
 */
void TbTaskManager::ExecuteTask(TbTaskLine taskLine)
{
    LineInfo* lineInfo = getLineInfo( taskLine );
    if( !lineInfo || !lineInfo->isActive ){
        return;
    }

    TbTaskNode* removeTaskNode = NULL;
    TbTaskNode* taskNode = lineInfo->top.child;
    TbTaskNode* parentNode = NULL;

    // ルートのみ次を持たないのでそこまで処理するまでループ
    while(taskNode){ 
        
        // 実行可能状態であれば実行
        if(taskNode->task && taskNode->func && ( taskNode->flagBit & TbTaskNode::FLAG_ACTIVE ) && !( taskNode->flagBit & TbTaskNode::FLAG_REMOVE ) ){
            (taskNode->task->*(taskNode->func))();
        }
        
        // もしも除去依頼が来ていたら設定
        if( taskNode->flagBit & TbTaskNode::FLAG_REMOVE ){
            removeTaskNode = taskNode;
        }

        // 次のタスク
        parentNode = taskNode->parent;
        if( !removeTaskNode && taskNode->child && 
            ( taskNode->flagBit & TbTaskNode::FLAG_ACTIVE ) && 
            ( taskNode->flagBit & TbTaskNode::FLAG_ENABLE_EXEC_CHILD ) )
        {
            // 削除予定でなくて子供がいれば子タスク
            taskNode = taskNode->child;
        }else if(taskNode->next != taskNode->parent->child ){ // 次タスク
            taskNode = taskNode->next;
        }else if( parentNode && parentNode->parent && 
                  parentNode->next != parentNode->parent->child ){ // 親タスク
            taskNode = taskNode->parent->next;
        }else{
            taskNode = NULL;
        }

        // 実際にはここでタスクラインから削除
        if( removeTaskNode ){
            deleteTaskNode( removeTaskNode );
            removeTaskNode = NULL;
        }
    }
}

/*!
 * タスク追加
 * @param task タスク
 * @param taskLine 追加タスクライン
 * @param priority プライオリティ
 * @param parent 親タスク
 * @return タスクノード
 * @author Miyake Shunsuke
 */
TbTaskNode* TbTaskManager::addTask( TbTask* task , TbTaskLine taskLine , TbTaskFuncType func , TbTask* parent )
{
    // 不正なタスクライン、不正なタスク
    LineInfo* lineInfo = getLineInfo( taskLine );
    if( !lineInfo ){
        return NULL;
    }

    //親がいない
    if( parent && !parent->ExsitsTaskLine(taskLine) ){
        TB_ASSERT(!"parent don't have taskLine");
        return NULL;
    }

    //task無いor設定済み
    if( !task || task->ExsitsTaskLine(taskLine))
    {
        TB_ASSERT(!"parent have already taskLine");
        return NULL;
    }
    
    /// ノード確保できない
    TbTaskNode* taskNode = createTaskNode();
    if(!taskNode)
    {
        return NULL;
    }

    taskNode->flagBit |= TbTaskNode::FLAG_RESERVE;
    taskNode->flagBit |= TbTaskNode::FLAG_ACTIVE;
    taskNode->line = taskLine;
    taskNode->func = func;

    TbTaskNode* parentNode = NULL;
    if(parent){
        parentNode = parent->getTaskNode(taskLine);
    }else{
        parentNode = &lineInfo->top;
    }

    taskNode->task = task;

    if(parentNode){
        taskNode->parent = parentNode;
    }

    //追加用タスクラインに追加
    if(m_AddTaskNodeTop){
        taskNode->prev = m_AddTaskNodeTop->prev;
        taskNode->next = m_AddTaskNodeTop;
        m_AddTaskNodeTop->prev->next = taskNode;
        m_AddTaskNodeTop->prev = taskNode;
    }else{
        m_AddTaskNodeTop = taskNode;
        m_AddTaskNodeTop->next = m_AddTaskNodeTop;
        m_AddTaskNodeTop->prev = m_AddTaskNodeTop;
    }

    ++lineInfo->count;
    return taskNode;
}

    
/*!
 * 登録タスク数取得
 * @param taskline タスクライン
 * @author Miyake Shunsuke
 */
TbUint32 TbTaskManager::GetTaskCount(TbTaskLine taskLine)
{
    LineInfo* lineInfo = getLineInfo( taskLine );
    if( lineInfo ){
        return lineInfo->count;
    }
    return 0;
}

/*!
 * TaskLine別有効設定
 * @param taskline タスクライン
 * @author Miyake Shunsuke
 */
void TbTaskManager::SetActiveTaskLine(TbTaskLine taskLine , TbBool isActive )
{
    LineInfo* lineInfo = getLineInfo( taskLine );
    if( lineInfo ){
        lineInfo->isActive = isActive;
    }
}

/*!
 * TaslLine別有効チェック
 * @param taskline タスクライン
 * @author Miyake Shunsuke
 */
TbBool TbTaskManager::IsActiveTaskLine(TbTaskLine taskLine )
{
    LineInfo* lineInfo = getLineInfo( taskLine );
    return lineInfo && lineInfo->isActive;
}

/*!
 * タスクノード生成
 * @author Miyake Shunsuke
 */
TbTaskNode* TbTaskManager::createTaskNode()
{
    return TB_NEW TbTaskNode();
}

/*!
 * タスクライン上の前ノード削除
 * @author Miyake_Shunsuke
 * @since 2014.08.15
 */
void TbTaskManager::deleteTaskNode( TbTaskLine taskLine )
{
    LineInfo* lineInfo = getLineInfo( taskLine );
    if( !lineInfo ){
        return;
    }
    TbTaskNode* node = NULL;
    TbTaskNode* next = lineInfo->top.child;
    if( next ){
        do {
            node = next;
            next = node->next;
            // ノード削除
            deleteTaskNode( node );
        }while( next != node );
    }
}

/*!
 * タスクノード削除
 * @param taskNode タスクノード
 * @author Miyake Shunsuke
 */
void TbTaskManager::deleteTaskNode( TbTaskNode* node )
{
    if(!node){
        //TbAssert(TB_FALSE,"[deleteTaskNode]node is Null");
        return;
    }

    LineInfo* lineInfo = getLineInfo(node->line);
    if(!lineInfo){
        return;
    }
    --lineInfo->count;

    if( node->task ) {
        // タスクに設定されている場合は、切り離す
        node->task->RemoveTaskLine( node->line );
    }

    if( node->flagBit & TbTaskNode::FLAG_RESERVE  ) // 予約中ノード
    {
        //子供も全部タスクラインから除去
        TbTaskNode* reserveNode = m_AddTaskNodeTop;
        TbTaskNode* nextNode = NULL;

        // まだ実際には登録されていない予約子タスクも削除
        if(reserveNode){
            do {
                nextNode = reserveNode->next;
                if(node == reserveNode->parent && reserveNode->task ){
                    deleteTaskNode( reserveNode );
                }
                reserveNode = nextNode;
            }while( reserveNode != m_AddTaskNodeTop );
        }

        // ノードをつなぎ変える
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if(m_AddTaskNodeTop == node){
            node->prev->next = node->next;
            node->next->prev = node->prev;
            m_AddTaskNodeTop = node->next;
        }
    }
    else
    {

        //子供も全部タスクラインから除去
        if(node->child){
            TbTaskNode* child = node->child;
            do{
                TbTaskNode* childNext = child->next;
                deleteTaskNode( child );
                child = childNext;
            }while( node->child );
        }

        // ノードをつなぎ変える
        if(node->parent->child == node){
            node->parent->child = ( node->next != node ) ? node->next : NULL;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    TB_DELETE node;
}

/*!
 * 実際にタスクノード追加
 * @param taskNode タスクノード
 * @param taskLine タスクライン
 * @author Miyake Shunsuke
 */
void TbTaskManager::addTaskNode( TbTaskNode* node)
{
    if(!node){
        TB_ASSERT_MSG(TB_FALSE,"node is Null");
        return;
    }

    TbTaskNode* top = node->parent->child;
    if(top){
        node->next = top;
        node->prev = top->prev;
        top->prev->next = node;
        top->prev = node;
    }else{
        node->parent->child = node;
        node->prev = node;
        node->next = node;
    }

    node->flagBit &= ~TbTaskNode::FLAG_RESERVE;

}

/*!
 * 自殺タスク処理
 * @author Miyake Shunsuke
 * @since 2011.05.16
 */
void TbTaskManager::killTask()
{
    TbTaskManager::LineInfo* lineInfo = getLineInfo( TB_TASK_LINE_SYSTEM_SUICIDE );
    TbTaskNode* node = NULL;
    TbTaskNode* next = lineInfo->top.child;
    if( next ){
        do { 
            node = next;
            next = node->next;
            TB_ASSERT( node && !node->child ); // 子タスクは存在しない

            // タスクを削除
            TB_SAFE_RELEASE( node->task );

            // ノード削除
            deleteTaskNode( node );

        }while( next != node );
    }
}

/*!
 * タスクライン情報取得
 * @author Miyake Shunsuke
 * @since 2011.11.20
 */
TbTaskManager::LineInfo* TbTaskManager::getLineInfo( TbTaskLine taskLine )
{
    if( taskLine < static_cast<TbUint32>(m_TaskLineInfo.GetCount()) ){
        return &m_TaskLineInfo[taskLine];   // 通常ライン
    }else if( TbTaskLineIsSystem(taskLine) ){
        return &m_TaskLineInfoSystem[ taskLine - TB_TASK_LINE_SYSTEM_BEGIN ];   // システム予約ライン
    }
    return NULL;
}

}
