/*!
 * テクスチャ用リソース
 * @author Miyake Shunsuke
 * @since 2011.12.27
 */

#ifndef _INCLUDED_TB_RESOURCE_TEXTURE_H_
#define _INCLUDED_TB_RESOURCE_TEXTURE_H_

#include <base/io/resource/tb_resource_data.h>
#include <base/io/resource/tb_resource.h>
#include <base/io/resource/tb_resource_handle.h>
#include <graphics/render/3dapi/tb_3dapi_types.h>

namespace toybox
{

class TbTexture;
class TbRenderDevice;

class TbResourceDataTexture : public TbResourceData
{
public:

    // コンストラクタ
    TbResourceDataTexture();

    // デストラクタ
    virtual ~TbResourceDataTexture();

public:

    // セットアップ
    virtual Result Setup( const char* filePath , 
                          TbUint32 listType , 
                          TbFileSyncMode syncMode , 
                          const void* buf , 
                          TbUint32 bufSize );
    // テクスチャ取得
    TbTexture* GetTexture() { 
        return m_Texture;
    }

    // テクスチャ取得
    const TbTexture* GetTexture() const { 
        return m_Texture;
    }

private:
    TbTexture*      m_Texture;  // テクスチャ
};

typedef TbResource<TbResourceDataTexture> TbResourceTexture;
class TbResourceHandleTexture : public TbResourceHandle<TbResourceDataTexture>
{
public:
    // テクスチャ取得
    TbTexture* GetTexture() { 
        TbResourceDataTexture* data = GetData();
        if( data ) {
            return data->GetTexture();
        }
        return nullptr;
    }
    // テクスチャ取得
    const TbTexture* GetTexture() const { 
        const TbResourceDataTexture* data = GetData();
        if( data ) {
            return data->GetTexture();
        }
        return nullptr;
    }
};

}

#endif
