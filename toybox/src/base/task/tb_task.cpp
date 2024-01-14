/*!
 * @file
 * @brief タスククラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/task/tb_task.h"
#include "base/task/tb_task_manager.h"
#include "base/algorithm/tb_sort.h"
#include "base/algorithm/tb_search.h"
#include "base/util/tb_adapter_iterator.h"

#define Sort TbInsertionSort

namespace toybox
{

namespace
{

struct Getter
{
    TbTaskLine operator()( const TbTaskNode* value ){
        return value->line;
    }
};

struct CompareNodeSort
{
    TbBool operator()( const TbTaskNode* left , const TbTaskNode* right ){
        return left->line < right->line;
    }
};

}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbTask::TbTask( TbUint32 lineCount , const char* name )
    : TbRefCounter()
    , m_Name( name )
    , m_BitArray()
    , m_TaskNode(lineCount)
    , m_TaskNodeSuicide( NULL )
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbTask::~TbTask(void)
{
    //タスク管理クラスからの削除漏れがないかチェック( TB_TASKLINE_SUICIDEだけは管理クラスで自動除去 )
    TbArray<TbTaskNode*>::Iterator ite = m_TaskNode.Begin();
    TbArray<TbTaskNode*>::Iterator endIte = m_TaskNode.End();
    for( ; ite != endIte; ++ite ){
        (*ite)->flagBit |= TbTaskNode::FLAG_REMOVE;
        (*ite)->task = NULL;
    }
    if( m_TaskNodeSuicide ){
        m_TaskNodeSuicide->flagBit |= TbTaskNode::FLAG_REMOVE;
        m_TaskNodeSuicide->task = NULL;
        m_TaskNodeSuicide = NULL;
    }
    m_TaskNode.Clear();
}

/*!
 * タスクライン追加
 * @param taskLine 追加タスクライン
 * @param parent 親タスク
 * @author Miyake Shunsuke
 */
TbResult TbTask::addTaskLine( TbTaskLine taskLine , TbTaskFuncType func , TbTask* parent )
{
    TB_ASSERT( getTaskNodeIndex(taskLine) <= 0 );
    if( TB_VERIFY(!m_TaskNode.IsFull()) ){
        TbTaskNode* node = TbTaskManager::GetInstance().addTask( this , taskLine , func , parent);
        if(node){
            node->flagBit |= TbTaskNode::FLAG_ENABLE_EXEC_CHILD;
            m_TaskNode.PushBack( node );
            Sort(m_TaskNode.Begin(),m_TaskNode.End(),CompareNodeSort());
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * タスクライン除去
 * @param taskLine 除去タスクライン
 * @author Miyake Shunsuke
 */
TbResult TbTask::RemoveTaskLine(TbTaskLine taskLine)
{
    TbSint32 index = getTaskNodeIndex(taskLine);
    if( index >= 0 ){
        m_TaskNode[ index ]->flagBit |= TbTaskNode::FLAG_REMOVE;
        m_TaskNode[ index ]->task = NULL;
        m_TaskNode.Erase( index );
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * タスクライン別タスク情報取得
 * @author Miyake Shunsuke
 */
TbTaskNode* TbTask::getTaskNode(TbTaskLine taskLine)
{
    TbSint32 index = getTaskNodeIndex(taskLine);
    if( index >= 0 ){
        return m_TaskNode[index];
    }
    return NULL;
}

/*!
 * タスクライン別タスク情報取得
 * @author Miyake Shunsuke
 */
const TbTaskNode* TbTask::getTaskNode(TbTaskLine taskLine) const
{
    TbSint32 index = getTaskNodeIndex(taskLine);
    if( index >= 0 ){
        return m_TaskNode[index];
    }
    return NULL;
}

/*!
 * タスクライン別タスク情報取得
 * @author Miyake Shunsuke
 * @since 2011.11.18
 */
TbSint32 TbTask::getTaskNodeIndex(TbTaskLine taskLine) const
{
    Getter getter;
    TbSint32 result = TbBinarySearchIndex( TbGetAdapterIterator<TbTaskLine>(m_TaskNode.Begin(),getter),
                                           TbGetAdapterIterator<TbTaskLine>(m_TaskNode.End(),getter) ,
                                           taskLine );
    return result;
}

/*!
 * タスクライン別存在取得
 * @author Miyake Shunsuke
 */
TbBool TbTask::ExsitsTaskLine(TbTaskLine taskLine) const
{
    TbBool result = getTaskNodeIndex(taskLine) >= 0;
    return result;
}

/*!
 * タスクラインの有効設定
 * @author Miyake Shunsuke
 */
TbResult   TbTask::SetActiveTaskLine(TbTaskLine taskLine,TbBool isActive)
{
    TbTaskNode* node = getTaskNode( taskLine );
    if(!node){
        return TB_E_FAIL;
    }
    if(isActive){
        node->flagBit |= TbTaskNode::FLAG_ACTIVE;
    }else{
        node->flagBit &= ~TbTaskNode::FLAG_ACTIVE;
    }
    return TB_S_OK;
}

/*!
 * タスクラインの有効設定
 * @author Miyake Shunsuke
 */
TbResult   TbTask::SetActiveTaskLine(TbBool isActive)
{
    TbArray<TbTaskNode*>::Iterator ite = m_TaskNode.Begin();
    TbArray<TbTaskNode*>::Iterator endIte = m_TaskNode.End();
    for( ; ite != endIte; ++ite ) {
        if(isActive){
            (*ite)->flagBit |= TbTaskNode::FLAG_ACTIVE;
        }else{
            (*ite)->flagBit &= ~TbTaskNode::FLAG_ACTIVE;
        }
    }
    return TB_S_OK;
}

/*!
 * タスクラインの有効取得
 * @author Miyake Shunsuke
 */
TbBool TbTask::IsActiveTaskLine(TbTaskLine taskLine) const
{
    const TbTaskNode* node = getTaskNode( taskLine );
    if(!node){
        return TB_FALSE;
    }
    return (node->flagBit & TbTaskNode::FLAG_ACTIVE);
}

/*!
 * タスクラインの子タスクを呼び出し可能を設定
 * @author Miyake Shunsuke
 * @since 2012.10.10
 */
TbResult TbTask::SetEnableExecChild( TbTaskLine taskLine , TbBool isActive )
{
    TbTaskNode* node = getTaskNode( taskLine );
    if(!node){
        return TB_E_FAIL;
    }
    if(isActive){
        node->flagBit |= TbTaskNode::FLAG_ENABLE_EXEC_CHILD;
    }else{
        node->flagBit &= ~TbTaskNode::FLAG_ENABLE_EXEC_CHILD;
    }
    return TB_S_OK;
}

/*!
 * 登録している全てのタスクラインの子タスク呼び出し可能を設定
 * @author Miyake Shunsuke
 * @since 2012.10.10
 */
TbResult TbTask::SetEnableExecChild( TbBool isActive )
{
    TbArray<TbTaskNode*>::Iterator ite = m_TaskNode.Begin();
    TbArray<TbTaskNode*>::Iterator endIte = m_TaskNode.End();
    for( ; ite != endIte; ++ite ) {
        if(isActive){
            (*ite)->flagBit |= TbTaskNode::FLAG_ENABLE_EXEC_CHILD;
        }else{
            (*ite)->flagBit &= ~TbTaskNode::FLAG_ENABLE_EXEC_CHILD;
        }
    }
    return TB_S_OK;
}

/*!
 * タスクラインの子タスクを呼び出し可能
 * @author Miyake Shunsuke
 * @since 2012.10.10
 */
TbBool TbTask::IsEnableExecChild(TbTaskLine taskLine) const
{
    const TbTaskNode* node = getTaskNode( taskLine );
    if(!node){
        return TB_FALSE;
    }
    return (node->flagBit & TbTaskNode::FLAG_ENABLE_EXEC_CHILD);
}


/*!
 *
 * TbTask::
 * @author Miyake Shunsuke
 * @since 2011.05.16
 */ 
TbResult TbTask::Kill()
{
    if( !m_TaskNodeSuicide ){
        TbTaskNode* node = TbTaskManager::GetInstance().addTask( this , TB_TASK_LINE_SYSTEM_SUICIDE , NULL , NULL );
        if(node){
            m_TaskNodeSuicide = node;
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

}
