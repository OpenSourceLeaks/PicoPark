/*!
 * @file
 * @brief ビューポートシザーステート
 * @author teco
 */

#ifndef _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_H_
#define _INCLUDED_TB_VIEWPORT_SCISSOR_STATE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <base/math/tb_geometry.h>

namespace toybox
{
    struct TbViewportScissorStateParam
    {
        TbViewportScissorStateParam()
        {}
        TbRectS32   viewPortRect;
    };
}

// 依存コード
#include "./depend/tb_viewport_scissor_state_depend.h"

namespace toybox
{

/*!
 * 深度ステンシルステート
 */
class TbViewportScissorState
{
public:

    typedef TbViewportScissorStateParam Param;

public:

    // コンストラクタ
    TbViewportScissorState() {}

    //デストラクタ
    ~TbViewportScissorState() {}

public:

    // 初期化
    void Initialize(const Param& param) {
        m_Param = param;
        m_Depend.Initialize(param);
    }

    void Finalize() {
        m_Depend.Finalize();
    }

public:

    // パラメータ取得
    const Param& GetParam() const {
        return m_Param;
    }
    
public:

    // 依存コード取得
    TbViewportScissorStateDepend& GetDepend() { return m_Depend; }
    const TbViewportScissorStateDepend& GetDepend() const { return m_Depend; }

private:

    Param                           m_Param;
    TbViewportScissorStateDepend    m_Depend;             //!< 依存コード

};

}

#endif