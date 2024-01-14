/*!
 * @file
 * @brief GFX用ビューポートシザーステート
 * @author teco
 */

#ifndef _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_GFX_H_
#define _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_GFX_H_

namespace toybox
{

class TbViewportScissorStateDepend
{
public:
    // コンストラクタ
    TbViewportScissorStateDepend()
        : m_IsInitialized(TB_FALSE)
    {
    }
    // デストラクタ
    ~TbViewportScissorStateDepend() {
        Finalize();
    }
public:
    // 初期化
    void Initialize(const TbViewportScissorStateParam& parma);
    // 後始末
    void Finalize();
public:
    // ステート取得
    const nn::gfx::ViewportScissorState& GetState() const {
        return m_State;
    }
private:
    TbBool                          m_IsInitialized;
    nn::gfx::ViewportScissorState   m_State;
};

}
#endif