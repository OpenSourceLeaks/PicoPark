/*!
 * @file
 * @brief 深度ステンシルステート
 * @author teco
 */

#ifndef _INCLUDED_TB_DEPTH_STENCIL_STATE_H_
#define _INCLUDED_TB_DEPTH_STENCIL_STATE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{
    struct TbDepthStencilStateParam
    {
        struct StencilOpSet
        {
            StencilOpSet()
                : failOp(TB_STENCIL_OP_KEEP)
                , zfailOp(TB_STENCIL_OP_KEEP)
                , passOp(TB_STENCIL_OP_KEEP)
            {}
            TbStencilOp failOp;
            TbStencilOp zfailOp;
            TbStencilOp passOp;
        };
        TbDepthStencilStateParam()
            : isEnableDepthTest(TB_FALSE)
            , depthWriteMask(TB_DEPTH_MASK_ALL)
            , depthTestFunc(TB_CMP_FUNC_LESS)
            , isEnableStencilTest(TB_FALSE)
            , stencilReadMask(0xFF)
            , stencilWriteMask(0xFF)
            , stencilTestFunc(TB_CMP_FUNC_ALWAYS)
            , stencilTestRef(0U)
        {}
        TbBool      isEnableDepthTest;
        TbDepthMask depthWriteMask;
        TbCmpFunc   depthTestFunc;
        TbBool      isEnableStencilTest;
        TbUint8     stencilReadMask;
        TbUint8     stencilWriteMask;
        TbCmpFunc   stencilTestFunc;
        TbUint8     stencilTestRef;
#if 0
        StencilOpSet stencilOp[TB_RENDER_FACE_MAX];
#else
        StencilOpSet stencilOp; // 現在両面同一設定のみ
#endif
    };
}

// 依存コード
#include "./depend/tb_depth_stencil_state_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * 深度ステンシルステート
 */
class TbDepthStencilState
{
public:

    typedef TbDepthStencilStateParam Param;

public:

    // コンストラクタ
    TbDepthStencilState() {}

    //デストラクタ
    ~TbDepthStencilState() {}

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
    TbDepthStencilStateDepend& GetDepend() { return m_Depend; }
    const TbDepthStencilStateDepend& GetDepend() const { return m_Depend; }

private:

    Param                     m_Param;
    TbDepthStencilStateDepend m_Depend;             //!< 依存コード

};

}

#endif