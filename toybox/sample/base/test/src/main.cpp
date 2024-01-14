#include "fwd.h"
#include "thread_test.h"
#include "container_test.h"
#include <framework/tb_framework_cui.h>

#include <string>

namespace toybox
{

/*!
 * 開始ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // 初期化
    TbFrameworkCui::Initialize();

#if 0
    TestThread();
#endif

#if 0
    TestWorkerThread();
#endif

#if 0
    TestContainer();
#endif
}

/*!
 * 終了ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // 終了
    TbFrameworkCui::Finalize();
}

}