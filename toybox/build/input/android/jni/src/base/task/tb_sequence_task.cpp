/*!
 * 準備、更新、終了処理を行うシーケンスタスク
 * @author Miyake Shunsuke
 * @since 2011.09.06
 */

#include "tb_fwd.h"
#include "base/task/tb_sequence_task.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.08.11
 */
TbSequenceTask::TbSequenceTask( TbUint32 lineCount , const char* name , TbTaskLine mainTaskLine )
    : Super(lineCount,name)
    , m_BitArray()
    , m_State( STATE_PREPARE )
    , m_Phase( 0 )
{
    AddTaskLine( mainTaskLine , &Self::Main );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.08.11
 */
TbSequenceTask::~TbSequenceTask()
{
}

/*!
 * メイン関数
 * @author Miyake Shunsuke
 * @since 2012.08.11
 */
void TbSequenceTask::Main()
{
    switch( m_State ){
    case STATE_PREPARE:
        if( OnPrepare() ){
            setState( STATE_READY_FOR_UPDATE );
        }
        break;
    case STATE_UPDATE:
        if( m_BitArray.Test( FLAG_REQUEST_FINISH ) ){
            setState( STATE_FINISHING );
        }else{
            OnUpdate();
        }
        break;
    case STATE_FINISHING:
        if( OnFinish() ){
            setState( STATE_FINISHED );
        }
        break;
    case STATE_READY_FOR_UPDATE:
    case STATE_FINISHED:
    default:
        break;
    }
}

}
