/*!
 * @file
 * @brief タスク管理システム
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TASK_MANAGER_H_
#define _INCLUDED_TB_TASK_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <base/task/tb_task_types.h>
#include <base/container/tb_array.h>

namespace toybox
{

/*!
 * タスク管理クラス
 */
class TbTaskManager : public TbSingletonRef<TbTaskManager>
{
public:

    //コンストラクタ
    explicit TbTaskManager( const TbUint32 taskLineCount );

    //デストラクタ
    ~TbTaskManager(void);

public:

    // 毎フレーム処理
    void Update();

    // 自殺タスクを明示的に殺す
    void Kill();

    // タスク実行
    void ExecuteTask(TbTaskLine taskLine);

    // 登録タスク数取得
    TbUint32 GetTaskCount(TbTaskLine taskLine);

    // TaskLine別有効設定
    void SetActiveTaskLine(TbTaskLine taskLine , TbBool isActive );

    // TaskLine別有効設定取得
    TbBool IsActiveTaskLine( TbTaskLine taskLine );

private: // 定数

    struct LineInfo
    {
        // コンストラクタ
        LineInfo()
            : top()
            , count(0) 
            , isActive(TB_TRUE)
        {}
        // デストラクタ
        ~LineInfo() {}

        TbTaskNode  top;
        TbUint32    count;
        TbBool      isActive;
    };

private:

    // 自殺タスク処理
    void killTask();

    // タスク追加
    TbTaskNode* addTask( TbTask* task , TbTaskLine taskLine , TbTaskFuncType func , TbTask* parent = NULL);

    //タスクノード生成
    TbTaskNode* createTaskNode();

    // タスクライン上の前ノード削除
    void deleteTaskNode( TbTaskLine taskLine );

    //タスクノード削除
    void deleteTaskNode( TbTaskNode* node );

    // 実際にタスクノード追加
    void addTaskNode( TbTaskNode* node );

    // タスクライン情報取得
    LineInfo* getLineInfo( TbTaskLine taskLine );

private: // メンバ変数

    TbBitArray32                m_BitArray;                                     //!< ビット配列
    LineInfo                    m_TaskLineInfoSystem[TB_TASK_LINE_SYSTEM_MAX];  //!< 登録タスク(システム予約)
    TbArray<LineInfo>           m_TaskLineInfo;                                 //!< 登録タスク数
    TbTaskNode*                 m_AddTaskNodeTop;                               //!< 追加用先頭タスクノード

private:    //フレンド設定

    friend class TbTask;

};

}

#endif
