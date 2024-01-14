/*!
 * @file
 * @brief タスク
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TASK_H_
#define _INCLUDED_TB_TASK_H_

#include <base/container/tb_array.h>
#include <base/util/tb_ref_counter.h>
#include <base/string/tb_static_string.h>
#include <base/task/tb_task_types.h>

namespace toybox
{

class TbTaskManager;

/*!
 * タスククラス
 * @author Miyake Shunsuke
 */
class TbTask
    : public TbRefCounter
{
public:

    // コンストラクタ( nameにはTB_TASK_NAMEマクロを利用してください)
    TbTask( TbUint32 lineCount , const char* name );

protected:

    // デストラクタ
    virtual ~TbTask();

public:

    // 名前設定
    void SetName( const char* name ) {
        m_Name = name;
    }

    // 名前取得
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // タスクライン追加
    template<typename T>
    TbResult AddTaskLine(TbTaskLine taskLine , void (T::*func)() , TbTask* parent = NULL){
        return addTaskLine( taskLine , static_cast<TbTaskFuncType>(func) , parent );
    }

    // NULLなタスクライン追加
    TbResult AddTaskLine(TbTaskLine taskLine , TbTask* parent = NULL){
        return addTaskLine( taskLine , NULL , parent );
    }

    // タスクライン除去
    TbResult RemoveTaskLine(TbTaskLine taskLine);

    // 設定されているタスクライン数取得
    TbUint32 GetTaskLineCount() const;

    // タスクライン存在するか
    TbBool ExsitsTaskLine(TbTaskLine taskLine) const;

    // タスクラインの有効設定
    TbResult SetActiveTaskLine(TbTaskLine taskLine,TbBool isActive);

    // 登録している全てのタスクラインの有効設定
    TbResult SetActiveTaskLine( TbBool isActive );

    // タスクラインの有効中か
    TbBool IsActiveTaskLine(TbTaskLine taskLine) const;

    // タスクラインの子タスクを呼び出し可能を設定
    TbResult SetEnableExecChild(TbTaskLine taskLine,TbBool isActive);

    // 登録している全てのタスクラインの子タスク呼び出し可能を設定
    TbResult SetEnableExecChild( TbBool isActive );

    // タスクラインの子タスクを呼び出し可能
    TbBool IsEnableExecChild(TbTaskLine taskLine) const;

    // 自殺
    TbResult Kill();

private: //フレンド用関数

    //フレンドクラス
    friend class TbTaskManager;

    //タスクライン別タスク情報設定
    void setTaskNode(TbTaskNode* taskNode,TbTaskLine taskLine);

    //タスクライン別タスク情報取得
    TbTaskNode* getTaskNode(TbTaskLine taskLine);
    const TbTaskNode* getTaskNode(TbTaskLine taskLine) const;

    //タスクライン別タスク情報取得
    TbSint32 getTaskNodeIndex(TbTaskLine taskLine) const;

private:

    // タスクライン追加
    TbResult addTaskLine( TbTaskLine taskLine , TbTaskFuncType func , TbTask* parent );

private:

    TbStaticString32             m_Name;              //!< 名前
    TbBitArray32                 m_BitArray;          //!< フラグ
    TbArray<TbTaskNode*>         m_TaskNode;          //!< タスクノード
    TbTaskNode*                  m_TaskNodeSuicide;   //!< 自殺用タスクノード(特別)

};

}

#endif
