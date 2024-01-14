/*!
 * @file
 * @brief GFX用深度ステンシルステート
 * @author teco
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_STATE_GFX_H_
#define _INCLUDED_TB_DEPTH_STENCIL_STATE_GFX_H_

namespace toybox
{

class TbDepthStencilStateDepend
{
public:
    // コンストラクタ
    TbDepthStencilStateDepend() 
        : m_IsInitialized(TB_FALSE)
    {
    }
    // デストラクタ
    ~TbDepthStencilStateDepend() {
        Finalize();
    }
public:
    // 初期化
    void Initialize(const TbDepthStencilStateParam& parma);
    // 後始末
    void Finalize();
public:
    // ステート取得
    const nn::gfx::DepthStencilState& GetState() const {
        return m_State;
    }
private:
    TbBool                      m_IsInitialized;
    nn::gfx::DepthStencilState  m_State;
};

}
#endif