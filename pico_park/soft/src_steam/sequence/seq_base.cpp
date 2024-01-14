/*!
 * シーケンス基底
 * @author teco
 */

#include "fwd.h"
#include "seq_base.h"
#include "seq_manager.h"

/*!
 * デストラクタ
 * @author teco
 */
SeqBase::~SeqBase() 
{
    if (m_Bgm) {
        if(m_IsEnableDeleteBgm ){
            SeqManager::GetInstance().DeleteBgm();
        }
        m_Bgm = nullptr;
    }
    if (m_DebugMenu) {
        m_DebugMenu->Suicide();
    }
}

/*!
 * BGM生成
 * @author teco
 */
void SeqBase::CreateBgm(const char* bgm)
{
    if (m_Bgm) {
        return;
    }
    m_Bgm = SeqManager::GetInstance().CreateBgm(bgm);
}

/*!
* BGM取得
* @author teco
*/
TbBool SeqBase::GetBgm()
{
    if (m_Bgm) {
        return TB_TRUE;
    }
    m_Bgm = SeqManager::GetInstance().GetBgm();
    return m_Bgm != nullptr;
}

// フェードイン
void SeqBase::FadeIn(TbFloat32 sec)
{
    SeqManager::GetInstance().FadeIn(sec);
}

// フェードアウト
void SeqBase::FadeOut(TbFloat32 sec)
{
    SeqManager::GetInstance().FadeOut(sec);
}

// フェード中か
TbBool SeqBase::IsFading() const
{
    return SeqManager::GetInstance().IsFading();
}

// 終了処理(終了完了したらRESULT_FINISHを返す)
SeqBase::Result SeqBase::OnFinish()
{
    if (m_Bgm) {
        if(m_IsEnableDeleteBgm ){
            SeqManager::GetInstance().DeleteBgm();
        }
        m_Bgm = nullptr;
    }
    return RESULT_FINISH;
}

