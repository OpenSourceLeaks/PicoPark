/*!
 * 画像クラス
 * @author Miyake_Shunsuke
 * @since 2014.10.16
 */

#include "tb_fwd.h"
#include "graphics/image/tb_image.h"
#include "graphics/image/tb_image_bmp.h"
#include "graphics/image/tb_image_tga.h"

namespace toybox
{

/*!
 * 設定
 * @author Miyake_Shunsuke
 * @since 2014.10.16
 */
void TbImage::Set( void* buffer , TbSizeT bufferSize , Type type )
{
    TB_RESQUE(buffer&&bufferSize>0&&TYPE_INVALID<type&&type<TYPE_MAX);

    TbImageImportInfo info;
    info.fileBuf = buffer;
    info.fileBufSize = bufferSize;
    info.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
    
    switch( type )
    {
    case TYPE_BMP:
        if( !TbImageBmp::Import(m_ImportResult,info) ){
            return;
        }
        break;
    case TYPE_TGA:
        if( !TbImageTga::Import(m_ImportResult,info) ){
            return;
        }
        break;
    default:
        TB_ASSERT(!"invalid format");
        break;
    }
    m_Type = type;
}

/*!
 * リセット
 * @author Miyake_Shunsuke
 * @since 2014.10.16
 */
void TbImage::Reset()
{
    if( TYPE_INVALID == m_Type ){
        return;
    }
    TbImageAllocatorDefault::Free(m_ImportResult.buffer);
    m_Type = TYPE_INVALID;
}

/*!
 * 色取得
 * @author Miyake_Shunsuke
 * @since 2014.10.16
 */
TbColor TbImage::GetColor( TbUint32 x , TbUint32 y ) const
{
    TbColor result;
    if( TYPE_INVALID == m_Type ){
        return result;
    }
    if( x >= GetWidth() ){
        return result;
    }
    if( y >= GetHeight() ){
        return result;
    }
    TbUint32* image = reinterpret_cast<TbUint32*>(m_ImportResult.buffer);
    TbUint32 argb = image[y*GetWidth()+x];
    result.SetColorU8(TB_COLOR_A8R8G8B8_GET_R(argb),
                      TB_COLOR_A8R8G8B8_GET_G(argb),
                      TB_COLOR_A8R8G8B8_GET_B(argb),
                      TB_COLOR_A8R8G8B8_GET_A(argb));
    return result;
}

}