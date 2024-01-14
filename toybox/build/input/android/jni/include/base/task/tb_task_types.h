/*!
 * @file
 * @brief タスク定義
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TASK_TYPES_H_
#define _INCLUDED_TB_TASK_TYPES_H_

namespace toybox
{

class TbTask;

typedef void (TbTask::*TbTaskFuncType)();

// タスク名マクロ
#define TB_TASK_NAME(name) name

// タスクライン
typedef TbUint32 TbTaskLine;
#define TB_TASK_LINE_INVALID (0xFFFFFFFF) 

enum TbTaskLineSystem // システム予約のタスクライン
{
    TB_TASK_LINE_SYSTEM_BEGIN = 0x8FFFFF00 ,
    TB_TASK_LINE_SYSTEM_PRE = TB_TASK_LINE_SYSTEM_BEGIN , // 前処理
    TB_TASK_LINE_SYSTEM_POST                            , // 後処理
    TB_TASK_LINE_SYSTEM_SUICIDE , // 基本的にアプリケーションがこのラインを直接触る必要はない
    TB_TASK_LINE_SYSTEM_END ,
    TB_TASK_LINE_SYSTEM_MAX = TB_TASK_LINE_SYSTEM_END - TB_TASK_LINE_SYSTEM_BEGIN
};

// システム予約されたタスクラインかどうか
inline TbBool TbTaskLineIsSystem( TbTaskLine line )
{
    return ( TB_TASK_LINE_SYSTEM_BEGIN <= line && line < TB_TASK_LINE_SYSTEM_END );
};

/*!
 * タスクノード
 * @author Miyake Shunsuke
 * @since 2011.11.15
 */
struct TbTaskNode
{
    enum{
        FLAG_ACTIVE  = TB_BIT(0) , // 有効
        FLAG_RESERVE = TB_BIT(1) , // 予約状態
        FLAG_REMOVE = TB_BIT(2) , // 除去状態
        FLAG_ENABLE_EXEC_CHILD = TB_BIT(3) , // 子タスクの実行が可能
    };

    TbTaskNode()
        : task( NULL )
        , func( NULL )
        , line( TB_TASK_LINE_INVALID )
        , parent( NULL )
        , child( NULL )
        , prev( NULL )
        , next( NULL )
        , flagBit( 0 )
    {}
    ~TbTaskNode(){
            
    }

    TbTask*         task;               //タスク
    TbTaskFuncType  func;               // 呼び出される関数
    TbTaskLine      line;               // 所属ライン
    TbTaskNode*     parent;             // 親
    TbTaskNode*     child;              // 子
    TbTaskNode*     prev;               // 前
    TbTaskNode*     next;               // 次
    TbUint32        flagBit;            //フラグビット

};

}

#endif

