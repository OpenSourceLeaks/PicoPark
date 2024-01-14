/*!
 * レンダーデバイスの共通処理
 * @author Miyake Shunsuke
 * @since 2010.12.05
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_texture.h"
#include "base/string/tb_string_util.h"

namespace toybox
{

// ファイル名からファイルタイプ取得
TbTextureFileType TbTexture::GetFileTypeFromFileName( const char* filename )
{
    if( !filename ){
        return TB_TEXTURE_FILE_TYPE_UNKNOWN;
    }
    TbSint32 index = -1;
    if( ( index = TbStrReverseFind( filename , "." ) ) >= 0 ){
        const char* extension = &filename[index+1];
        if( 0 == TbStrCmp( extension , "bmp" ) ){
            return TB_TEXTURE_FILE_TYPE_BMP;
        }else if( 0 == TbStrCmp( extension , "tga" ) ){
            return TB_TEXTURE_FILE_TYPE_TGA;
        }
    }
    return TB_TEXTURE_FILE_TYPE_UNKNOWN;
}

}
