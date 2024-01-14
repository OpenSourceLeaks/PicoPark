#include "fwd.h"
#include "thread_test.h"
#include "container_test.h"
#include <framework/tb_framework_cui.h>

#include <string>

namespace toybox
{

/*!
 * �J�n�|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // ������
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
 * �I���|�C���g
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // �I��
    TbFrameworkCui::Finalize();
}

}