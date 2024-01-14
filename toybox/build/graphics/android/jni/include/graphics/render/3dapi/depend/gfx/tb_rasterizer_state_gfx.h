/*!
 * @file
 * @brief GFX用ラスタラザーステート
 * @author teco
 */

#ifndef _INCLUDED_TB_RASTERIZER_STATE_GFX_H_
#define _INCLUDED_TB_RASTERIZER_STATE_GFX_H_

namespace toybox
{

class TbRasterizerStateDepend
{
public:
    // コンストラクタ
    TbRasterizerStateDepend()
        : m_IsInitialized(TB_FALSE)
    {}
    // デストラクタ
    ~TbRasterizerStateDepend() {
        Finalize();
    }
public:
    // 初期化
    void Initialize(const TbRasterizerStateParam& parma);
    // 後始末
    void Finalize();
public:
    // ステート取得
    const nn::gfx::RasterizerState& GetState() const {
        return m_State;
    }
private:
    TbBool                      m_IsInitialized;
    nn::gfx::RasterizerState    m_State;
};

}

#endif