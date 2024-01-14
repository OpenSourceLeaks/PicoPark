/*!
 * PMX用モデル
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_MATERIAL_PMX_H_
#define _INCLUDED_TB_MATERIAL_PMX_H_

#include <graphics/material/tb_material.h>
#include <graphics/model/tb_model_data_pmx.h>
#include <graphics/render/3dapi/tb_shader_manager.h>

namespace toybox
{

class TbModelPmx;

class TbMaterialPmx : public TbMaterial
{
public:

    // コンストラクタ
    TbMaterialPmx();

    // デストラクタ
    virtual ~TbMaterialPmx();

public:

    // シェーダーパラメータをGPUに送る
    virtual void SendShaderParam( TbUint32 passIndex , 
                                  const TbMatrix* world , 
                                  const TbMatrix43* boneMatrix , 
                                  TbUint32 boneMatrixCount );

public:

    // パラメータ設定
    void SetParam( TbModelDataPmx::MaterialInfo& param );

    // パラメータ取得
    const TbModelDataPmx::MaterialInfo& GetParam() const {
        return m_Param;
    }

    // テクスチャ設定
    void SetTexture( TbUint32 index , TbTexture* texture );

private:

    static const TbUint32 TEX_MAX = 10;
    TbModelDataPmx::MaterialInfo m_Param;
    TbUint32                     m_TextureCount;
    TbTexture*                   m_Texture[TEX_MAX];
    TbShaderConstHandle          m_BoneConstHandle;  // 定数ハンドル

};

}

#endif
