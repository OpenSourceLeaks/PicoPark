/*!
* @file
* @brief ブレンドステート
* @author teco
*/

#ifndef _INCLUDED_TB_BLEND_STATE_H_
#define _INCLUDED_TB_BLEND_STATE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{
    struct TbBlendStateParam
    {
        TbBlendStateParam()
            : colorMaskBit(TB_COLOR_MASK_BIT_ALL)
            , isEnableBlend(TB_FALSE)
            , blendOp(TB_BLEND_OP_ADD)
            , blendSrcFactor(TB_BLEND_FACTOR_ONE)
            , blendDstFactor(TB_BLEND_FACTOR_ZERO)
        {}
        TbUint32      colorMaskBit;
        TbBool        isEnableBlend;
        TbBlendOp     blendOp;
        TbBlendFactor blendSrcFactor;
        TbBlendFactor blendDstFactor;
    };
}

// 依存コード
#include "./depend/tb_blend_state_depend.h"

namespace toybox
{

/*!
* 深度ステンシルステート
*/
class TbBlendState
{
public:

    typedef TbBlendStateParam Param;

public:

    // コンストラクタ
    TbBlendState() {}

    //デストラクタ
    ~TbBlendState() {}

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
    TbBlendStateDepend& GetDepend() { return m_Depend; }
    const TbBlendStateDepend& GetDepend() const { return m_Depend; }

private:

    Param              m_Param;
    TbBlendStateDepend m_Depend;             //!< 依存コード

};

}

#endif