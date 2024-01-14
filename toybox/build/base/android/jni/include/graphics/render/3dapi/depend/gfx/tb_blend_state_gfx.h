/*!
* @file
* @brief GFX用ブレンドステート
* @author teco
*/

#ifndef _INCLUDED_TB_BLEND_STATE_GFX_H_
#define _INCLUDED_TB_BLEND_STATE_GFX_H_

namespace toybox
{

class TbBlendStateDepend
{
public:
    // コンストラクタ
    TbBlendStateDepend() : m_IsInitialized(TB_FALSE) , m_Memory(nullptr) {}
    // デストラクタ
    ~TbBlendStateDepend() {
        Finalize();
    }
public:
    // 初期化
    void Initialize(const TbBlendStateParam& parma);
    // 後始末
    void Finalize();
public:
    // ステート取得
    const nn::gfx::BlendState& GetState() const {
        return m_State;
    }
private:
    TbBool               m_IsInitialized;
    TbUint8*             m_Memory;
    nn::gfx::BlendState  m_State;
};

}
#endif