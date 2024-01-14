#include "fwd.h"
#include <framework/tb_framework_cui.h>
#include <base/io/archive/tb_archive_exporter.h>
#include <base/io/archive/tb_archive.h>
#include <base/io/tb_file_util.h>
#include <base/io/tb_file_all.h>
#include <base/algorithm/tb_algorithm_misc.h>

#include <string>

namespace
{

using namespace toybox;
    
TbArchive s_Archive;
TbUint32  s_Phase = 0;

TbUint8   s_Data1[256];
TbUint8   s_Data2[256];

void mainLoop()
{
    switch( s_Phase )
    {
    case 0:
        if( s_Archive.IsReady() ){
            s_Archive.ReadFileSync("stage_common.lua",s_Data1,sizeof(s_Data1));
            s_Archive.ReadFileSync("stage_002.lua",s_Data2,sizeof(s_Data2));
            ++s_Phase;
        }
        break;
    case 1:
        if( !s_Archive.IsBusy() ){
            ++s_Phase;
        }
        break;
    case 2:
        break;
    };
}

}

namespace toybox
{

/*!
 * 開始ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount )
{
    // 初期化
    TbFrameworkCui::Initialize();
    if( argCount < 1 ) {
        return;
    }

#if 1
    TbStaticString512 path = argList[0];
    TbFileUtil::PathList list;
    TbFileUtil::GetFiles( list , path.GetBuffer() , TB_FILE_PATH_ID_NONE , TB_FILE_SEARCH_FLAG_SUBDIR );
    TbArchiveExporter exporter;
    if( list.IsEmpty() ){
        TbPrintf("file list is empty!\n");
    }else{
        TbForIterator( ite , list ) 
        {
            exporter.AddFileInfo( &ite->GetBuffer()[path.GetCharCount()+1] , ite->GetBuffer() , TB_FILE_PATH_ID_NONE );
        }
        const char* outputFileName = "test.tbarc";
        if( 2 <= argCount ){
            outputFileName = argList[1];
        }
        exporter.Export( outputFileName , TB_FILE_PATH_ID_NONE );
    //    TbArchive::LoadAndExpand( "test.tbarc" , TB_FILE_PATH_ID_NONE );
    }
#elif 0
    TbArchive::Expand( TbPath("dst",TB_FILE_PATH_ID_NONE) , 
                       TbPath("test.tbarc",TB_FILE_PATH_ID_NONE) );
#else
    s_Archive.SetConcurrentReadMax(2);
    s_Archive.InitializeAsync( "test.tbarc" , TB_FILE_PATH_ID_NONE , TB_FALSE );
    TbFrameworkCui::StartMainLoop(NULL,mainLoop,NULL);
#endif
}

/*!
 * 終了ポイント
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void TbFrameworkCui::OnExitPoint()
{
    // 終了
    TbFrameworkCui::Finalize();
}

}