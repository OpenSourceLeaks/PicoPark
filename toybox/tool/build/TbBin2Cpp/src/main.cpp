#include "fwd.h"
#include <framework/tb_framework_cui.h>
#include <base/io/archive/tb_archive_exporter.h>
#include <base/io/archive/tb_archive.h>
#include <base/io/tb_file_util.h>
#include <base/io/tb_file_all.h>
#include <base/algorithm/tb_algorithm_misc.h>
#include <base/io/stream/tb_stream_text_writer.h>
#include <base/io/stream/tb_file_stream.h>

namespace toybox
{

/*!
 * 開始ポイント
 * @author teco
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // 初期化
    TbFrameworkCui::Initialize();

    if( argCount < 1 ){
        return;
    }

    TbStaticString256 baseName;
    TbFileUtil::GetBaseName(baseName,argList[0]);
    TbStaticString256 extName;
    TbFileUtil::GetExtension(extName,argList[0]);

    TbFile file( argList[0] , TB_FILE_OPERATE_READ , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
    file.Open();
    file.Read();
    
    const char* outputFileName = "out.bin";
    if( 2 <= argCount ){
        outputFileName = argList[1];
    }
    TbFile outputFile( outputFileName , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
    outputFile.Open();
    TbFileStream stream(&outputFile);
    TbStreamTextWriter writer(&stream);
    const TbUint8* fileBuf = reinterpret_cast<const TbUint8*>(file.GetReadBuffer());
    TbUint32 fileSize = file.GetReadBufferSize();
    TbStaticString256 str;
    str.SetFormatedString("TbUint8 RESOURCE_BIN[] = \n{\n    ",baseName.GetBuffer(),extName.GetBuffer());
    writer.WriteString(str.GetBuffer());
    for( TbUint32 i = 0; i < fileSize; ++i ) {
        str.SetFormatedString("0x%X,",fileBuf[i]);
        writer.WriteString(str.GetBuffer());
        if( 0 != i && i % 16 == 0 ){
            writer.WriteString("\n    ");
        }
    }
    writer.WriteString("\n};");

}

/*!
 * 終了ポイント
 * @author teco
 */
void TbFrameworkCui::OnExitPoint()
{
    // 終了
    TbFrameworkCui::Finalize();
}

}