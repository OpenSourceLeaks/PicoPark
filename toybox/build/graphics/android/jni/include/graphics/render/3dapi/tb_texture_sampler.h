/*!
 * @file
 * @brief テクスチャサンプラのクラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TEXTURE_SAMPLER_H_
#define _INCLUDED_TB_TEXTURE_SAMPLER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{

struct TbTextureSamplerParam
{
    TbTextureSamplerParam()
        : minFilterMode(TB_TEXTURE_FILTER_MODE_POINT)
        , magFilterMode(TB_TEXTURE_FILTER_MODE_POINT)
        , addressModeU(TB_TEXTURE_ADDRESS_MODE_REPEAT)
        , addressModeV(TB_TEXTURE_ADDRESS_MODE_REPEAT)
    {
    }
    TbTextureFilterMode minFilterMode;
    TbTextureFilterMode magFilterMode;
    TbTextureAddressMode addressModeU;
    TbTextureAddressMode addressModeV;
};

}

// 依存コード
#include "./depend/tb_texture_sampler_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * サンプラ
 * @author Miyake Shunsuke
 */
class TbTextureSampler
{    
public:

    // コンストラクタ
    TbTextureSampler();

    //デストラクタ
    ~TbTextureSampler();

public:

    // 初期化
    void Initialize( const TbTextureSamplerParam& param );

    // 後始末
    void Finalize();

public:

    // パラメータ取得
    const TbTextureSamplerParam& GetParam() const {
        return m_Param;
    }
    
public:

    // 依存コード取得
    TbTextureSamplerDepend& GetDepend() { return m_Depend; }
    const TbTextureSamplerDepend& GetDepend() const { return m_Depend; }

private:

    TbTextureSamplerParam   m_Param;
    TbBool                  m_IsInitialized;
    TbTextureSamplerDepend  m_Depend;             //!< 依存コード

};

}

#endif