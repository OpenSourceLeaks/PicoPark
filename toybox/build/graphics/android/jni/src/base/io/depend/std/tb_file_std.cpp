/*!
 * @file
 * @brief 標準関数を利用したファイルクラスの実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/io/depend/std/tb_file_std.h"
#include "base/io/tb_file_util.h"
#include "base/math/tb_math_util.h"

// 警告抑制

namespace toybox
{

namespace
{
    const int ORIGIN_CONVERT_TABLE[] = {
        SEEK_SET , 
        SEEK_CUR , 
        SEEK_END
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(ORIGIN_CONVERT_TABLE)==TB_IO_SEEK_TYPE_MAX);
    int getSeekOrigin( TbIOSeekType type )
    {
        if( 0 <= type && type < TB_IO_SEEK_TYPE_MAX ){
            return ORIGIN_CONVERT_TABLE[type];
        }
        TB_ASSERT(0);
        return SEEK_CUR;
    };
}

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 * @since 2011.12.06
 */
TbUint32 TbFileStd::GetFileSize( const char* path , TbFilePathId pathId )
{
    TbUint32 result = 0;
    const TbStaticString512& fullpath( TbFileUtil::GetFullPath( path , pathId ) );
    FILE* fp = fopen( fullpath.GetBuffer() , "rb" );
    fseek( fp , 0 , SEEK_END );
    if( !fp ){
        return 0;
    }
    result = ftell(fp);
    fclose(fp);
    return result;
}

/*!
 * ファイル存在しているか
 * @author Miyake Shunsuke
 * @since 2011.12.06
 */
TbBool TbFileStd::ExistsFile( const char* path , TbFilePathId pathId )
{
    const TbStaticString512& fullpath( TbFileUtil::GetFullPath( path , pathId ) );
    FILE* fp = fopen( fullpath.GetBuffer() , "rb" );
    if( fp ){
        fclose(fp);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author Miyake Shunsuke
 */
TbFileStd::TbFileStd( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
     : Super(path,operateMode,syncMode,pathId)
     , m_Handle(NULL)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbFileStd::~TbFileStd()
{
    Close();
}

/*!
 * 開く
 * @author Miyake Shunsuke
 * @since 2011.11.29
 */
TbResult TbFileStd::Open()
{
    if( m_Handle ){
        // 開いている場合は、一度Closeを呼んでください
        return TB_E_DENY;
    }

    const char* mode = "";
    switch( GetOperateMode() )
    {
    case TB_FILE_OPERATE_READ:
        mode = "rb";
        SetState( TB_IO_STATE_READY );
        break;
    default:
        TB_ASSERT(0);
        return TB_E_FAIL; 
        break;
    }
    
    switch(GetSyncMode())
    {
    case TB_FILE_SYNC:
        break;
    case TB_FILE_ASYNC: // 非同期未対応
    default:
        TB_ASSERT(0);
        return TB_E_FAIL; 
        break;
    }

    m_Handle = fopen( GetPath() , mode );

    // 失敗処理
    if( !m_Handle ){
        TbDebugPrintf("[FILE] Load Failed : %s\n" , GetPath());
        SetState( TB_IO_STATE_ERROR );
        return TB_E_FAIL;
    }

    TbDebugPrintf("[FILE] Load Success : %s\n" , GetPath());
    SetState( TB_IO_STATE_READY );
    return TB_S_OK;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.27
 */
TbResult TbFileStd::Seek( TbSint32 offset , TbIOSeekType type )
{
    if( m_Handle ){
        int result = fseek( m_Handle , offset , getSeekOrigin(type) );
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.27
 */
TbUint32 TbFileStd::GetSeek() const
{
    if( m_Handle ){
        int result = fseek( m_Handle , 0 , SEEK_CUR );
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * ファイルサイズ取得
 * @author Miyake Shunsuke
 * @since 2011.11.30
 */
TbUint32 TbFileStd::GetFileSize() const
{
    TbUint32 result = 0U;
    if( m_Handle ){
        TbUint32 currentSeek = ftell(m_Handle);
        fseek( m_Handle , 0 , SEEK_END );
        result = ftell( m_Handle );
        // シーク位置を元に戻す
        fseek( m_Handle , currentSeek , SEEK_SET );
    }
    return result;
}

/*!
 * 同期読みこみ
 * @author Miyake_Shunsuke
 * @since 2015.01.09
 */
TbResult TbFileStd::ReadSyncImpl( void* readBuffer ,
                                TbUint32 readSize ,
                                TbSint32 seekPos , 
                                TbUint32* resultReadSize )
{
    if( seekPos >= 0 ) {
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    size_t result = fread( readBuffer , readSize , 1 , m_Handle );
    if( resultReadSize ){
        *resultReadSize = result;
    }
    return TB_S_OK;
}

/*!
 * 同期書き込み
 * @author Miyake_Shunsuke
 * @since 2015.01.09
 */
TbResult TbFileStd::WriteSyncImpl( const void* buffer , TbUint32 size )
{
    fwrite( buffer , size , 1 , m_Handle );
    return TB_S_OK;
}

/*!
 * クローズ処理
 * @author Miyake Shunsuke
 * @since 2011.11.29
 */
TbResult TbFileStd::Close()
{
    SetState(TB_IO_STATE_CLOSED);
    if( m_Handle ){
        fclose( m_Handle );
        m_Handle = NULL; 
    }
    return TB_S_OK;
}

}