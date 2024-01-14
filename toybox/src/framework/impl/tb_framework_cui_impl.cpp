/*!
 * フレームワーク実装部分の共通
 * @author teco
 */

#include "tb_fwd.h"
#include "framework/impl/tb_framework_cui_impl.h"

namespace toybox
{

/*!
 * 更新
 * @author teco
 */
void TbFrameworkCuiImpl::Update()
{
    // 時間処理
    TbTick::Update();

    TbTaskManager& task = GetTask();

    // フレーム開始処理
    CallMainLoopBegin();

    TbFileManager::GetInstance().Update();

    task.ExecuteTask( TB_TASK_LINE_SYSTEM_PRE ); // タスク(システム前用)
    m_ResourceManager.Update();  // リソース

    // メイン処理
    CallMainLoop();

    // システム後処理
    task.ExecuteTask( TB_TASK_LINE_SYSTEM_POST );
    task.Update();

    // フレーム終了処理
    CallMainLoopEnd();
}

}
