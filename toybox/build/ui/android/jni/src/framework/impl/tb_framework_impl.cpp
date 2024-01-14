/*!
 * フレームワーク実装部分
 * @author teco
 */

#include <tb_fwd.h>
#include <framework/impl/tb_framework_impl.h>
#include <graphics/render/tb_draw_2d.h>

namespace toybox
{

/*!
 * 更新
 * @author teco
 */
void TbFrameworkImpl::Update()
{
    // 時間処理
    TbTick::Update();
    TbTaskManager& task = GetTask();

    // フレーム開始処理
    CallMainLoopBegin();

    // システム前処理
    if(!m_WindowManager.Update()){ // ウィンドウ
        FinishMainLoop();
        return;
    }

    TbFileManager::GetInstance().Update();

    task.ExecuteTask(TB_TASK_LINE_SYSTEM_PRE); // タスク(システム前用)
    m_ResourceManager.Update();  // リソース

    // メイン処理
    CallMainLoop();
}

/*!
 * 描画処理
 * @author teco
 */
void TbFrameworkImpl::Draw()
{
    if( !IsBusyMainLoop() ){
        return;
    }
    TbTaskManager& task = GetTask();

    // 描画コマンド発行処理
    TbDraw2dTextureResetVertexBuffer();
    m_RenderPassManager.Begin();
    if( m_MainLoopDrawFunc ){
        m_MainLoopDrawFunc();
    }
    m_RenderPassManager.End();

    // 実際の描画
    m_RenderDevice.BeginScene();
    m_RenderPassManager.Flush();
    if (m_MainLoopDrawDirectFunc) {
        m_MainLoopDrawDirectFunc();
    }
    m_RenderDevice.EndScene();
    m_RenderDevice.PresentAll();  
}

/*!
 * 後処理
 * @author teco
 */
void TbFrameworkImpl::Post()
{
    if( !IsBusyMainLoop() ){
        return;
    }
    TbTaskManager& task = GetTask();

    // システム後処理
    task.ExecuteTask( TB_TASK_LINE_SYSTEM_POST );
    task.Update();

    // フレーム終了処理
    CallMainLoopEnd();
}

}
