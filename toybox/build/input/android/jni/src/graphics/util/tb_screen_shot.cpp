/*!
 * 画面キャプチャ
 * @author Miyake Shunsuke
 * @since 2011.10.24
 */

#include "tb_fwd.h"
#include "graphics/util/tb_screen_shot.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/image/tb_image_bmp.h>
#include <graphics/image/tb_image_tga.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>

namespace toybox
{

struct ExportInfo
{
    TbBool (*exportFunc)( TB_OUT TbImageExportResult& result , const TbImageExportInfo& info , const TbImageAllocFunc allocFunc );
};
static const ExportInfo EXPORT_INFO[] =
{
    { TbImageBmp::Export } , 
    { TbImageTga::Export }
};

/*!
 * フロントバッファファイル出力
 * @author Miyake Shunsuke
 * @since 2011.10.31
 */
TbResult TbScreenShot::CaptureAndExportSync( const char* filepath , 
                                             FileType format )
{
    if( !filepath ){
        return TB_E_INVALIDARG;
    }
    TbRenderView* view = TbRenderDevice::GetInstance().GetView(0);
    if( !view ){
        return TB_E_INVALIDARG;
    }
    TbUint32 pitch = view->GetBackBufferPitch();
    TbUint32 height = view->GetBackBufferHeight();
    TbUint32 bufSize = pitch * height;
    TbUint8* texBuffer = new TbUint8[ bufSize ];
    TbBool isBufferDownToUp = TB_FALSE;
    if( TB_SUCCEEDED( view->ReadBackBuffer( texBuffer , bufSize , NULL , &isBufferDownToUp ) ) ){
        TbFile texFile( filepath , 
                        TB_FILE_OPERATE_WRITE , 
                        TB_FILE_SYNC , 
                        TB_FILE_PATH_ID_NONE );
        texFile.Open();
        TbImageExportInfo exportInfo;
        exportInfo.buffer = texBuffer;
        exportInfo.bufSize = pitch * height;
        exportInfo.width = view->GetBackBufferWidth();
        exportInfo.height = height;
        exportInfo.colorFormat = view->GetBackBufferFormat();
        exportInfo.flag = isBufferDownToUp ? TbImageExportInfo::FLAG_BIT_DOWN_TO_UP : 0;
        TbImageExportResult exportResult;
        if( EXPORT_INFO[format].exportFunc( exportResult , exportInfo , NULL ) ){
            texFile.Write( exportResult.fileBuf , exportResult.fileBufSize ); 
        }
        TbImageAllocatorDefault::Free( exportResult.fileBuf );
        texFile.Close();
    }
    TB_SAFE_DELETE_ARRAY( texBuffer );
    return TB_S_OK;
}

}