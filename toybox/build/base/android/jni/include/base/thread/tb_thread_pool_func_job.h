/*!
 * 関数を呼び出すスレッドプール用ジョブ
 * @author Miyake Shunsuke
 * @since 2011.06.07
 */
 
#ifndef _INCLUDED_TB_THREAD_POOL_FUNC_JOB_H_
#define _INCLUDED_TB_THREAD_POOL_FUNC_JOB_H_

#include <base/thread/tb_thread_pool_job.h>
#include <base/util/tb_function.h>

namespace toybox
{

/*!
 * スレッドプールクラス
 * @author Miyake Shunsuke
 * @since 2011.06.25
 */
class TbThreadPoolFuncJob : public TbThreadPoolJob
{
public:
    typedef TbStaticFunction32<void ()> ExecFunc;
    typedef TbThreadPoolJob Super;
public:
    
    // コンストラクタ
    TbThreadPoolFuncJob( const char* name , const ExecFunc& func )
        : Super(name)
        , m_ExecFunc(func)
    {}

protected:

    // デストラクタ
    virtual ~TbThreadPoolFuncJob()
    {
    }

protected:

    // 実行(オーバーライド用)
    virtual void Execute() {
        if( m_ExecFunc.IsValid() ){
            m_ExecFunc();
        }
    }

private:

    ExecFunc    m_ExecFunc;

};

}

#endif
