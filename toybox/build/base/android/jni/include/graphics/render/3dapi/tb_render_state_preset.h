/*!
 * @file
 * @brief 描画デバイス
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_STATE_PRESET_H_
#define _INCLUDED_TB_RENDER_STATE_PRESET_H_

#include <base/util/tb_singleton.h>
#include <graphics/render/3dapi/tb_blend_state.h>
#include <graphics/render/3dapi/tb_depth_stencil_state.h>
#include <graphics/render/3dapi/tb_rasterizer_state.h>

namespace toybox
{

class TbRenderStatePreset : public TbSingletonRef<TbRenderStatePreset>
{
public:

    enum BlendType
    {
        BLEND_NORMAL ,
        BLEND_OPACITY , // 不透明
        BLEND_MAX
    };

    enum DepthStencilType
    {
        DEPTH_STENCIL_3D_NORMAL ,  
        DEPTH_STENCIL_2D     ,  // 2D描画用
        DEPTH_STENCIL_MAX
    };

    enum RasterizerType
    {
        RASTERIZER_CULL_NONE , // カリングなし
        RASTERIZER_MAX 
    };

public:

    // コンストラクタ
    TbRenderStatePreset() {}

    // デストラクタ
    ~TbRenderStatePreset() 
    {
        Finalize();
    }

public:

    // 初期化
    void Initialize();

    // 後始末
    void Finalize() {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_BlendState); ++i) {
            m_BlendState[i].Finalize();
        }
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_DepthStencilState); ++i) {
            m_DepthStencilState[i].Finalize();
        }
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_RasterizerState); ++i) {
            m_RasterizerState[i].Finalize();
        }
    }

public:

    // ブレンドステート取得
    const TbBlendState* GetBlendState(BlendType type) 
    {
        if (type < TB_ARRAY_COUNT_OF(m_BlendState)) {
            return &m_BlendState[type];
        }
        return nullptr;
    }

    // 深度ステンシルステート取得
    const TbDepthStencilState* GetDepthStencilState(DepthStencilType type) {
        if (type < TB_ARRAY_COUNT_OF(m_DepthStencilState)) {
            return &m_DepthStencilState[type];
        }
        return nullptr;
    }

    // ラスタライザーステート取得
    const TbRasterizerState* GetRasterizerState(RasterizerType type) {
        if (type < TB_ARRAY_COUNT_OF(m_RasterizerState)) {
            return &m_RasterizerState[type];
        }
        return nullptr;
    }

private:

    TbBlendState           m_BlendState[BLEND_MAX];
    TbDepthStencilState    m_DepthStencilState[DEPTH_STENCIL_MAX];
    TbRasterizerState      m_RasterizerState[RASTERIZER_MAX];

};

}

#endif
