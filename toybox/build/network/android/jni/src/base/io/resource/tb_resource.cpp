/*!
 * リソース
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#include "tb_fwd.h"
#include "base/io/resource/tb_resource.h"
#include "base/io/resource/tb_resource_manager.h"
#include "base/io/tb_file_all.h"

namespace toybox
{

namespace
{
    enum Phase
    {
        PHASE_BEGIN ,

        PHASE_INIT = PHASE_BEGIN , 

        PHASE_OPEN_FROM_FILE , 
        PHASE_OPEN_FROM_ARCHIVE , 

        PHASE_READ ,
        PHASE_WAIT_READ , 
        PHASE_SETUP_RESOURCE ,

        PHASE_FINISH ,
    };
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.26
 */
TbResourceBase::TbResourceBase()
    : m_BitArray()
    , m_ListType(0)
    , m_FileName()
    , m_SyncMode( TB_FILE_SYNC )
    , m_TypeId(0)
    , m_Phase( PHASE_BEGIN )
    , m_File()
    , m_Data(NULL)
{
}
    
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.26
 */
TbResourceBase::~TbResourceBase()
{
    Unload();
    TbResourceManager::GetInstance().removeResource(m_FileName.GetBuffer(),m_ListType);
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2011.12.26
 */
TbResult TbResourceBase::initialize( const char* fileName , 
                                     TbUint32 listType , 
                                     TbFileSyncMode syncMode , 
                                     TbFilePathId pathId , 
                                     TbTypeId typeId ,
                                     TbResourceType type )
{
    // ファイル生成
    m_FileName = fileName;
    m_SyncMode = syncMode;
    m_ListType = listType;
    m_TypeId = typeId;
    m_Type = type;
    return TB_S_OK;
}

/*!
 * 有効化
 * @author Miyake_Shunsuke
 * @since 2013.11.21
 */
void TbResourceBase::Load()
{
    if( m_Data ){
        m_Data->Retain();
        return;
    }
    static Phase PHASE_CONVERT_TABLE[] = 
    {
        PHASE_OPEN_FROM_FILE , 
        PHASE_OPEN_FROM_ARCHIVE , 
    };
    TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(PHASE_CONVERT_TABLE)==TB_RESOURCE_TYPE_MAX);

    if( TB_VERIFY(TB_RESOURCE_TYPE_BEGIN <= m_Type && m_Type < TB_RESOURCE_TYPE_END) ){
        m_Phase = PHASE_CONVERT_TABLE[m_Type];
        update();
    }
}

/*! 
 * @author Miyake_Shunsuke
 * @since 2013.11.22
 */
void TbResourceBase::Unload()
{
    TB_SAFE_RELEASE(m_Data);
}

/*!
 * ビジー状態か
 * @author Miyake_Shunsuke
 * @since 2013.11.27
 */
TbBool TbResourceBase::IsBusy() const {
    return m_Data && m_Phase != PHASE_FINISH;
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.12.26
 */
void TbResourceBase::update()
{
    if( m_Data ) {
        return;
    }
    switch( m_Phase )
    {
    case PHASE_OPEN_FROM_FILE: // ファイルオープン
        {
            TbFilePathId pathId = TbResourceManager::GetInstance().GetFilePath(m_ListType);
            if( TB_FAILED(m_File.Open( m_FileName.GetBuffer(),TB_FILE_OPERATE_READ,m_SyncMode,pathId))){
                // ファイルが存在しない
                m_BitArray.SetBit( FLAG_ERROR );
                m_File.Release();
                m_Phase = PHASE_FINISH;
                break;
            }else{
                m_Phase = PHASE_READ;
                if( m_File.GetSyncMode() != TB_FILE_SYNC ){
                    break;
                }
                // 完了復帰の場合このまま読み込みを開始する
            }
        }
        // break;

    case PHASE_READ: // ファイル読み込み
        {
            TbResult result = m_File.Read();
            if( !(TB_SUCCEEDED(result) || TB_E_PENDING==result) ){
                break;
            }
            m_Phase = PHASE_WAIT_READ;
        }
        // break
    case PHASE_WAIT_READ: // 読み込み完了待ち

        if( m_File.IsBusy() ){
            break;
        }
        m_Phase = PHASE_SETUP_RESOURCE;

        // break;
    case PHASE_SETUP_RESOURCE: // リソースセットアップ
        {
            m_Data = CreateData();
            TB_ASSERT(m_Data);
            const TbResourceData::Result result = m_Data->Setup( m_FileName.GetBuffer() , 
                                                                 m_ListType , 
                                                                 m_SyncMode ,
                                                                 m_File.GetReadBuffer() , 
                                                                 m_File.GetReadBufferSize() );
            if( !TB_VERIFY( TbResourceData::RESULT_COMPLETE == result ) ){
                break;
            }
            m_Phase = PHASE_FINISH;
            m_BitArray.SetBit( FLAG_READY , TB_TRUE );

            // ファイル解放
            m_File.Release();
#if 0
            // コールバックを呼び出す
            if( m_ReadCallback.IsValid() ){
                m_ReadCallback();
            }
#endif
        }
        break;

    case PHASE_FINISH: // 終了
        break;
    };
}

}