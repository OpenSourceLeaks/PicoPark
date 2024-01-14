/*!
 * レンダリングパス管理クラス
 * @author Miyake Shunsuke
 * @since 2011.09.01
 */

#include <tb_fwd.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <graphics/render/tb_render_pass.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbRenderPassManager::TbRenderPassManager()
    : m_RenderPass(nullptr)
    , m_RenderPassCount(0)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbRenderPassManager::~TbRenderPassManager()
{
    TB_SAFE_DELETE_ARRAY(m_RenderPass);
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2013.03.02
 */
TbResult TbRenderPassManager::Initialize( TbSizeT passMax )
{
    TB_SAFE_DELETE_ARRAY(m_RenderPass);
    m_RenderPassCount = passMax;
    if( m_RenderPassCount > 0 ){
        m_RenderPass = TB_NEW TbRenderPass[m_RenderPassCount];
        // コマンドバッファ用アロケータはパス間で共有
        for( TbSizeT i = 0; i <m_RenderPassCount; ++i ){
            m_RenderPass[i].SetRenderCommandAllocator(&m_CommandAllocator);
        }
    }
    return TB_S_OK;
}

/*!
 * レンダリングパス取得
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
TbRenderPass* TbRenderPassManager::GetPass( TbSizeT index )
{
    if( index < m_RenderPassCount ){
        return &m_RenderPass[index];
    }
    return nullptr;
}

/*!
 * 全レンダリングパスの描画コマンドを発行
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
void TbRenderPassManager::Begin()
{
    for( TbSizeT i = 0; i < m_RenderPassCount; ++i ) {
        m_RenderPass[i].begin();
    }
}

/*!
 * 全レンダリングパスの描画コマンドを発行
 * @author Miyake Shunsuke
 * @since 2013.02.27
 */
void TbRenderPassManager::End()
{
    for( TbSizeT i = 0; i < m_RenderPassCount; ++i ) {
        m_RenderPass[i].end();
    }
}

// 描画コマンドをGPUに転送
void TbRenderPassManager::Flush()
{
    for( TbSizeT i = 0; i < m_RenderPassCount; ++i ) {
        m_RenderPass[i].flush();
    }
    // クリアする
    m_CommandAllocator.Clear();
}

};
