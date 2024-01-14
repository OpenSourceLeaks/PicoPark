/*!
 * レンダリングパス管理クラス
 * @author Miyake Shunsuke
 * @since 2011.09.01
 */

#ifndef _INCLUDED_TB_RENDER_PASS_MANAGER_H_
#define _INCLUDED_TB_RENDER_PASS_MANAGER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/tb_render_command_buffer.h>
#include <base/util/tb_singleton.h>
#include <graphics/render/tb_render_pass.h>

namespace toybox
{

class TbRenderPassManager : public TbSingletonRef<TbRenderPassManager>
{
public:

    // コンストラクタ
    TbRenderPassManager();

    // デストラクタ
    virtual ~TbRenderPassManager();

public:

    // 初期化
    TbResult Initialize( TbSizeT renderPassMax );

    // パス数取得
    TbUint32 GetPassCount() const {
        return m_RenderPassCount;
    }

    // 描画コマンド用アロケータ取得
    TbRenderCommandAllocator* GetCommandAllocator() {
        return &m_CommandAllocator;
    }

public:

    // レンダリングパス取得
    TbRenderPass* GetPass( TbSizeT index );


public: // 描画

    // 各パスに描画コマンドを発行許可を出す
    void Begin();

    // 全パスの描画コマンド発行を禁止する
    void End();

    // 描画コマンドをGPUに転送
    void Flush();

private:

    TbRenderPass*               m_RenderPass;
    TbSizeT                     m_RenderPassCount;
    TbRenderCommandAllocator    m_CommandAllocator;

};

// レンダリングパス取得
inline TbRenderPass* TbGetRenderPass( TbSizeT index )
{
    return TbRenderPassManager::GetInstance().GetPass(index);
}

};

#endif
