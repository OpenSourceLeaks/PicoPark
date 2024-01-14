/*!
 * PMXモデル用リソース
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_RESOURCE_MODEL_PMX_H_
#define _INCLUDED_TB_RESOURCE_MODEL_PMX_H_

#include <base/io/resource/tb_resource_data.h>
#include <base/io/resource/tb_resource_handle.h>
#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/model/tb_model_data_pmx.h>
#include <graphics/model/tb_mesh.h>
#include <graphics/material/tb_material_pmx.h>
#include <graphics/skeleton/tb_skeleton_pmx.h>
#include <graphics/resource/tb_resource_texture.h>

namespace toybox
{

class TbTexture;
class TbRenderDevice;

class TbResourceDataModelPmx : public TbResourceData
{
public:

    // コンストラクタ
    TbResourceDataModelPmx();

    // デストラクタ
    virtual ~TbResourceDataModelPmx();

public:

    // セットアップ
    virtual Result Setup( const char* filePath , 
                          TbUint32 listType , 
                          TbFileSyncMode syncMode , 
                          const void* buf , 
                          TbUint32 bufSize );

public:

    // 頂点フォーマット取得
    TbVertexFormat* GetVertexFormat() {
        return m_VertexFormat;
    }

    // メッシュ取得
    TbMesh& GetMesh() {
        return m_Mesh;
    }

    // スケルトン取得
    const TbSkeletonPmx& GetSkeleton() const {
        return m_Skeleton;
    }

    // マテリアル数取得
    TbUint32 GetMaterialCount() const {
        return m_ModelData.material.count;
    }

    // マテリアル取得(コピー取得)
    TbBool GetMaterial( TbMaterialPmx& dst , TbUint32 index );

    // マテリアルに設定してあるテクスチャ取得
    TbTexture* GetMaterialTexture( TbUint32 materialIndex );

private:
    TbVertexFormat*          m_VertexFormat;
    TbMesh                   m_Mesh;
    TbSkeletonPmx            m_Skeleton;
    TbResourceHandleTexture* m_Texture;
    TbModelDataPmx           m_ModelData;
    TbUint32                 m_Phase;
};

typedef TbResource<TbResourceDataModelPmx> TbResourceModelPmx;
class TbResourceHandleModelPmx : public TbResourceHandle<TbResourceDataModelPmx>
{
public:

    // 頂点フォーマット取得
    TbVertexFormat* GetVertexFormat() {
        TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return data->GetVertexFormat();
        }
        return nullptr;
    }

    // メッシュ取得
    TbMesh* GetMesh() {
        TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return &data->GetMesh();
        }
        return nullptr;
    }

    // スケルトン取得
    const TbSkeletonPmx* GetSkeleton() const {
        const TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return &data->GetSkeleton();
        }
        return nullptr;
    }

    // マテリアル数取得
    TbUint32 GetMaterialCount() const {
        const TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return data->GetMaterialCount();
        }
        return 0;
    }

    // マテリアル取得(コピー取得)
    TbBool GetMaterial( TbMaterialPmx& dst , TbUint32 index ) {
        TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return data->GetMaterial(dst,index);
        }
        return TB_FALSE;
    }

    // マテリアルに設定してあるテクスチャ取得
    TbTexture* GetMaterialTexture( TbUint32 materialIndex ) {
        TbResourceDataModelPmx* data = GetData();
        if( data ) {
            return data->GetMaterialTexture(materialIndex);
        }
        return nullptr;
    }


};

}

#endif
