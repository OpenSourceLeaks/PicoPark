/*!
 * スレッドプール用ジョブ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#include "tb_fwd.h"
#include "base/thread/tb_thread_pool_job.h"
#include "base/thread/tb_thread_pool.h"
#include "base/util/tb_ref_counter.h"

namespace toybox
{
    
/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
TbThreadPoolJob::TbThreadPoolJob( const char* name )
    : m_Name(name)
    , m_BitArray()
    , m_State(STATE_READY)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
TbThreadPoolJob::~TbThreadPoolJob()
{
}

/*!
 * 開始
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
TbResult TbThreadPoolJob::Start()
{ 
    if( m_State.CompareAndSwap(STATE_BUSY,STATE_READY) ){
        if( TbThreadPool::GetInstance().PushJob(this) ){
            return TB_S_OK;
        }
        m_State.Store(STATE_READY);
    }
    return TB_E_FAIL;
}

/*!
 * 終了待ち
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
void TbThreadPoolJob::Join()
{
    // スピンロック
    while( m_State.Load() == STATE_BUSY ) {}
}

}
