/*!
 * インデックスバッファ依存コード(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_index_buffer.h"

#if TB_3DAPI_IS_S3D

#include "graphics/3dapi/tb_render_device.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @param format 頂点フォーマット
 * @param size バッファサイズ
 * @parma device　所属デバイスインデックス
 * @author Miyake Shunsuke
 * @since 2011.05.04
 */
TbIndexBuffer::TbIndexBuffer( const TbIndexBufferFormat format , const TbSint32 size )
    : m_Size(size)
    , m_Format( format )
{
    AS3::ui::flash::display3D::Context3D& context = TbRenderDevice::GetInstance().GetDepend().GetContext();
    if( TB_VERIFY_MSG_FORMAT(TB_INDEX_BUFFER_FORMAT_U16==format,"Unsupported Format %d",format) ){
        TbUint8* buf = TB_NEW TbUint8[size];
        m_Depend.indexBuffer = context->createIndexBuffer(size/2);
        m_Depend.indexBuffer->uploadFromByteArray(AS3::ui::internal::get_ram(),
                                                  reinterpret_cast<int>(buf), 
                                                  0 , 
                                                  size/2, 
                                                  reinterpret_cast<void*>(buf));
        TB_DELETE buf;
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbIndexBuffer::~TbIndexBuffer()
{
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
TbBool TbIndexBuffer::Write( const TbUint32 offset , const TbUint32 size , const void* src )
{
    if( src && TB_INDEX_BUFFER_FORMAT_U16 == m_Format && (offset%2) == 0 ) {
        m_Depend.indexBuffer->uploadFromByteArray(AS3::ui::internal::get_ram(),
                                                  reinterpret_cast<int>(src), 
                                                  offset/2 , 
                                                  size/2 , 
                                                  const_cast<void*>(src));
        return TB_TRUE;
    }
    return TB_FALSE;
}
    
}

#endif