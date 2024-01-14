/*!
 * @file
 * @brief AndroidのAssetを利用したファイルクラスの実装ファイル
 */

#include "tb_fwd.h"
#include "base/io/depend/android/tb_file_android.h"
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
    
static AAssetManager* s_AssertManager = nullptr;

/*!
 * アセット管理クラス設定
 * @author teco
 */
void TbFileAndroid::SetAssetManager(AAssetManager* mgr)
{
    s_AssertManager = mgr;
}

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileAndroid::GetFileSize( const char* path , TbFilePathId pathId )
{
    TbStaticString512 fname;
    TbFileUtil::GetFileName( fname , path );
    
    TbUint32 result = 0;
    AAsset* asset = AAssetManager_open(s_AssertManager, fname.GetBuffer() , AASSET_MODE_BUFFER);
    if( !asset ){
        return 0;
    }
    
    result = AAsset_getLength(asset);
    AAsset_close(asset);
    return result;
}

/*!
 * ファイル存在しているか
 * @author teco
 */
TbBool TbFileAndroid::ExistsFile( const char* path , TbFilePathId pathId )
{
    TbStaticString512 fname;
    TbFileUtil::GetFileName( fname , path );
    
    TbUint32 result = 0;
    AAsset* asset = AAssetManager_open(s_AssertManager, fname.GetBuffer() , AASSET_MODE_BUFFER);
    if( !asset ){
        return TB_FALSE;
    }
    AAsset_close(asset);
    return TB_TRUE;
}

/*!
 * コンストラクタ
 * @path ファイルパス
 * @param operateMode 処理モード
 * @param syncMode 同期モード
 * @parma pathId ルートパスID
 * @author teco
 */
TbFileAndroid::TbFileAndroid( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
     : Super(path,operateMode,syncMode,pathId)
     , m_Handle(nullptr)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbFileAndroid::~TbFileAndroid()
{
    Close();
}

/*!
 * 開く
 * @author teco
 */
TbResult TbFileAndroid::Open()
{
    if( m_Handle ){
        // 開いている場合は、一度Closeを呼んでください
        return TB_E_DENY;
    }

    switch( GetOperateMode() )
    {
    case TB_FILE_OPERATE_READ:
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

    TbStaticString512 fname;
    TbFileUtil::GetFileName( fname , GetPath() );
    m_Handle = AAssetManager_open(s_AssertManager, fname.GetBuffer() , AASSET_MODE_BUFFER);

    // 失敗処理
    if( !m_Handle ){
        TbPrintf("[FILE] Load Failed : %s\n" , GetPath());
        SetState( TB_IO_STATE_ERROR );
        return TB_E_FAIL;
    }

    TbPrintf("[FILE] Load Success : %s\n" , GetPath());
    SetState( TB_IO_STATE_READY );
    return TB_S_OK;
}

/*!
 * シーク
 * @author teco
 */
TbResult TbFileAndroid::Seek( TbSint32 offset , TbIOSeekType type )
{
    if( m_Handle ){
        int result = AAsset_seek( m_Handle , offset , getSeekOrigin(type) );
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * シーク
 * @author teco
 */
TbUint32 TbFileAndroid::GetSeek() const
{
    if( m_Handle ){
        int result = AAsset_seek( m_Handle , 0 , SEEK_CUR );
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

/*!
 * ファイルサイズ取得
 * @author teco
 */
TbUint32 TbFileAndroid::GetFileSize() const
{
    TbUint32 result = 0U;
    if( m_Handle ){
        result = AAsset_getLength(m_Handle);
    }
    return result;
}

/*!
 * 同期読みこみ
 * @author teco
 */
TbResult TbFileAndroid::ReadSyncImpl( void* readBuffer ,
                                TbUint32 readSize ,
                                TbSint32 seekPos , 
                                TbUint32* resultReadSize )
{
    if( seekPos >= 0 ) {
        Seek( seekPos , TB_IO_SEEK_TYPE_BEGIN );
    }
    size_t result = AAsset_read( m_Handle , readBuffer , readSize );
    if( resultReadSize ){
        *resultReadSize = result;
    }
    return TB_S_OK;
}

/*!
 * 同期書き込み
 * @author teco
 */
TbResult TbFileAndroid::WriteSyncImpl( const void* buffer , TbUint32 size )
{
    return TB_E_FAIL;
}

/*!
 * クローズ処理
 * @author teco
 */
TbResult TbFileAndroid::Close()
{
    SetState(TB_IO_STATE_CLOSED);
    if( m_Handle ){
        AAsset_close( m_Handle );
        m_Handle = nullptr; 
    }
    return TB_S_OK;
}

}