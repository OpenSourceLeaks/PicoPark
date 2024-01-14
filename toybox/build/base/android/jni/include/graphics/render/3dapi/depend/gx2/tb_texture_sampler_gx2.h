/*!
 * @file
 * @brief テクスチャサンプラ(GX2)
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_TEXTURE_SAMPLER_GX2_H_
#define _INCLUDED_TB_TEXTURE_SAMPLER_GX2_H_

namespace toybox
{

/*!
 * @class 
 */
class TbTextureSamplerDepend
{
public:

    // コンストラクタ
    TbTextureSamplerDepend()
        : minFilterMode( TB_TEXTURE_FILTER_MODE_POINT )
        , magFilterMode( TB_TEXTURE_FILTER_MODE_POINT )
        , addressModeU( TB_TEXTURE_ADDRESS_MODE_REPEAT )
        , addressModeV( TB_TEXTURE_ADDRESS_MODE_REPEAT )
    {
    }

    // デストラクタ
    ~TbTextureSamplerDepend(){}
    
public:
    
    // 更新
    void UpdateGX2Sampler();

public:

    TbTextureFilterMode  minFilterMode;
    TbTextureFilterMode  magFilterMode;
    TbTextureAddressMode addressModeU;
    TbTextureAddressMode addressModeV;
	GX2Sampler           sampler;
};

}

#endif
