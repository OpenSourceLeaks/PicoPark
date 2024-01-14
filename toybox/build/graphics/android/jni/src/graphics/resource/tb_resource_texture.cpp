/*!
 * テクスチャ用リソース
 * @author Miyake Shunsuke
 * @since 2011.12.28
 */

#include "tb_fwd.h"
#include "graphics/resource/tb_resource_texture.h"
#include "graphics/render/3dapi/tb_texture.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.28
 */
TbResourceDataTexture::TbResourceDataTexture()
    : TbResourceData()
    , m_Texture( nullptr )
{
}


/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.28
 */
TbResourceDataTexture::~TbResourceDataTexture()
{
    TB_SAFE_DELETE(m_Texture);
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 * @since 2011.12.28
 */
TbResourceData::Result TbResourceDataTexture::Setup(  const char* filePath , 
                                                      TbUint32 listType , 
                                                      TbFileSyncMode syncMode , 
                                                      const void* buf , 
                                                      TbUint32 bufSize )
{
    m_Texture = TB_NEW TbTexture(buf,bufSize,TbTexture::GetFileTypeFromFileName(filePath));
    return RESULT_COMPLETE;
}

}
