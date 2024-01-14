/*!
 * リソース
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

namespace toybox
{


template<typename T,typename UserType>
typename TbResource<T,UserType>::ResourceCreateFuncType TbResource<T>::s_CreateFunc = NULL;

template<typename T,typename UserType>
typename TbResource<T,UserType>::ResourceDeleteFuncType TbResource<T>::s_DeleteFunc = NULL;

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
TbResource<T>::TbResource( const char* fileName , TbFileSyncMode syncMode , TbFilePathId pathId , const UserType& param )
    : m_Phase( TB_RESOURCE_PHASE_BEGIN )
    , m_File( NULL )
    , m_LoadedCallback( NULL )
    , m_UserParam( param )
{
    if( !TB_VERIFY( Self::s_CreateFunc && Self::s_DeleteFunc ) ){
        // リソース生成関数、削除関数が存在しない
        m_BitArray.SetBit( FLAG_ERROR );
        return;
    }
    
    // ファイル生成
    m_File = TbFileManagrer::GetInstance().CreateFileHandle( fileName , TB_FILE_OPERATE_READ , syncMode , pathId );
    
    if( TB_SUCCEEDED(m_File->Open()) ){
        // ファイルが存在しない
        TB_SAFE_RELEASE( m_File );
        m_BitArray.SetBit( FLAG_ERROR );
        return;
    }
    m_File->SetCallbackCompletedRead( this , &Self::onLoaded() );
    Update();
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
TbResource<T>::~TbResource()
{
    // リソース解放
    if( m_Resource && !TB_VERIFY( Self::s_CreateFunc && Self::s_DeleteFunc ) ){
        Self::s_DeleteFunc( m_Resource );
        m_Resource = NULL;
    }
    // ファイル解放
    if( m_File ) {
        TbFileManager::GetInstance().DeleteFileHandle( m_File );
        m_File = NULL;
    }
}

/*!
 * リソース作成後にファイルリソースを開放して良いか
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
void TbResource<T>::SetEnableReleaseFileResource( TbBool isEnable )
{
}

/*!
 * リソース作成後にファイルリソースを開放しているか
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
TbBool TbResource<T>::IsEnableReleaseFileResource() const
{
    return m_BitArray.IsOnBit( FLAG_ENABLE_RELEASE_FILE_RESOURCE );
}

/*!
 * ファイルリソース開放済みか
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
TbBool TbResource<T>::IsReleasedFileResource() const
{
    return (!m_File);
}
        
/*
 * 更新!
 * @author Miyake Shunsuke
 * @since 2011.12.16
 */
template<typename T,typename UserType>
void TbResource<T>::Update()
{
}

/*!
 * 読み込み完了
 * @author Miyake Shunsuke
 * @since 2011.12.17
 */
template<typename T,typename UserType>
void TbResource<T>::onLoaded()
{
    m_Phase = TB_RESOURCE_PHASE_SETUP_RESOURCE:
}

}
