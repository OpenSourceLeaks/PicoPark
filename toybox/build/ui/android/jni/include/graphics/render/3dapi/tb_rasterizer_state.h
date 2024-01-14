/*!
* @file
* @brief ラスタライザステート
* @author teco
*/

#ifndef _INCLUDED_TB_RASTERIZER_STATE_H_
#define _INCLUDED_TB_RASTERIZER_STATE_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{

struct TbRasterizerStateParam
{
    TbRasterizerStateParam() 
        : fillMode(TB_FILL_MODE_SOLID) 
        , isFrontFaceClockWise(TB_TRUE) 
        , cullMode(TB_CULL_MODE_NONE)
    {}
    TbFillMode  fillMode;
    TbBool      isFrontFaceClockWise;
    TbCullMode  cullMode;
};

}

// 依存コード
#include "./depend/tb_rasterizer_state_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
* 深度ステンシルステート
*/
class TbRasterizerState
{
public:

    typedef TbRasterizerStateParam Param;

public:

    // コンストラクタ
    TbRasterizerState() {}

    //デストラクタ
    ~TbRasterizerState() {}

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
    TbRasterizerStateDepend&        GetDepend() { return m_Depend; }
    const TbRasterizerStateDepend&  GetDepend() const { return m_Depend; }

private:

    Param                     m_Param;
    TbRasterizerStateDepend   m_Depend;             //!< 依存コード

};

}

#endif