/*!
 * WAVファイル
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#include "tb_fwd.h"
#include "base/io/tb_file_manager.h"
#include "base/io/resource/tb_resource_manager.h"
#include "sound/tb_sound_manager.h"
#include "sound/tb_sound_source.h"
#include "sound/tb_sound_source_handle.h"
#include "sound/tb_sound_listener.h"
#include "sound/tb_sound_stream_buffer.h"
#include "sound/resource/tb_sound_resource_wav.h"
#include "sound/resource/tb_sound_resource_ogg.h"
#include "sound/resource/tb_sound_resource_pcm.h"

namespace toybox
{

/*!
 * ファイル名からファイルタイプ取得
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
TbSoundFileType TbSoundManager::GetFileTypeFromFileName( const char* filename )
{
    if( !filename ){
        return TB_SOUND_FILE_TYPE_UNKNOWN;
    }
    TbSint32 index = -1;
    if( ( index = TbStrReverseFind( filename , "." ) ) >= 0 ){
        const char* extension = &filename[index+1];
        if( 0 == TbStrCmp( extension , "wav" ) ){
            return TB_SOUND_FILE_TYPE_WAV;
        }else if( 0 == TbStrCmp( extension , "ogg" ) ){
            return TB_SOUND_FILE_TYPE_OGG;
        }
    }
    return TB_SOUND_FILE_TYPE_UNKNOWN;
}
    
/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundManager::TbSoundManager()
    : m_BitArray()
    , m_Param()
    , m_IssueSoundId(0)
    , m_ListType(0)
    , m_MasterVolume(1.0f)
    , m_ResourceList()
    , m_CurrentListener(NULL)
    , m_LayerInfo(NULL)
    , m_SimplePlaySources(NULL)
    , m_Depend()
{
}

/*!
 * コンストラクタ
 * @param initParam 初期化パラメータ
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundManager::TbSoundManager( const TbSoundInitParam& initParam )
    : m_BitArray()
    , m_Param()
    , m_IssueSoundId(0)
    , m_ListType(0)
    , m_MasterVolume(1.0f)
    , m_ResourceList()
    , m_CurrentListener(NULL)
    , m_LayerInfo(NULL)
    , m_SimplePlaySources(NULL)
    , m_Depend()
{
    Initialize(initParam);
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundManager::~TbSoundManager()
{
#if TB_FILE_IS_ENABLE_ASYNC
    // スレッド終了待ち
    m_SourceThread.RequestFinish();
    m_SourceThread.Join();
#endif

    TB_SAFE_DELETE_ARRAY(m_SimplePlaySources);
    // ソース定期処理
    TbSoundSource* source = NULL;
    for (TbUint32 i = 0; i < m_Param.layerCount; ++i) {
        source = m_LayerInfo[i].top;
        if (source) {
            TbBool hasNext = TB_FALSE;
            do {
                TbSoundSource* next = source->getNext();
                hasNext = next && next != m_LayerInfo[i].top;
                TB_ASSERT(source->GetRefCount()==1);
                removeSource(source);
                source = next;
            } while (hasNext);
        }
    }

    // レイヤー情報解放
    ResourceList::Iterator ite = m_ResourceList.Begin();
    ResourceList::Iterator endIte = m_ResourceList.End();
    for (; ite != endIte; ++ite) {
        TB_SAFE_DELETE((*ite).resource);
    }

    TB_SAFE_DELETE_ARRAY(m_LayerInfo);
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2011.11.18
 */
TbResult TbSoundManager::Initialize( const TbSoundInitParam& initParam )
{
    if( m_BitArray.Test( FLAG_INITIALIZED ) ){
        return TB_E_FAIL;
    }

    // 依存側での初期化
    if( TB_FAILED(m_Depend.initialize( initParam )) ){
        return TB_E_FAIL;
    }

    m_BitArray.SetBit( FLAG_INITIALIZED , TB_TRUE );

    // 初期化
    m_Param = initParam;

    // レイヤー構築
    m_LayerInfo = TB_NEW LayerInfo[m_Param.layerCount];
    
    // サウンドバッファの最大値設定
    m_ResourceList.SetCapacity( initParam.soundMax );

    if (initParam.simplePlayCount > 0 ){
        m_SimplePlaySources = TB_NEW TbSoundSourceHandle[initParam.simplePlayCount];
    }

    updateVolume();

    // スレッド設定
#if TB_SOUND_IS_ENABLE_STREAM_THREAD
    {
        TbThreadParam threadParam;
        threadParam.name = "TbSoundStreamThread";
        m_SourceThread.Initialize(threadParam, TbCreateMemFunc(this, &Self::onProcStreamThread));
        m_SourceThread.Start();
    }
#endif

    return TB_S_OK;
}

/*!
* 簡易SE再生
* @author Miyake_Shunsuke
* @since 2014.06.15
*/
TbResult TbSoundManager::PlaySound(const char* name)
{
    if (!IsInitialized()) {
        return TB_E_FAIL;
    }
    for (TbUint32 i = 0; i < m_Param.simplePlayCount; ++i) {
        if (!m_SimplePlaySources[i].IsBusy()) {
            m_SimplePlaySources[i].Reset(name);
            m_SimplePlaySources[i].Play();
            return TB_S_OK;
        }
    }
    TbPrintf("[TbSoundManager::PlaySound] fail to play [name=%s]", name);
    return TB_E_FAIL;
}


/*!
 * 簡易SE再生
 * @author Miyake_Shunsuke
 * @since 2014.06.15
 */
TbResult TbSoundManager::PlaySound( const char* name , TbFloat32 volume)
{
    if( !IsInitialized() ){
        return TB_E_FAIL;
    }
    for( TbUint32 i = 0; i < m_Param.simplePlayCount; ++i ){
        if( !m_SimplePlaySources[i].IsBusy() ){
            m_SimplePlaySources[i].Reset(name);
            m_SimplePlaySources[i].Play();
            m_SimplePlaySources[i].SetVolume(volume);
            return TB_S_OK;
        }
    }
    TbPrintf("[TbSoundManager::PlaySound] fail to play [name=%s]",name);
    return TB_E_FAIL;
}

/*!
 * 定期更新
 * @author teco
 */
void TbSoundManager::Update()
{
    if( !IsInitialized() ){
        return;
    }

#if 1 || !TB_SOUND_IS_ENABLE_STREAM_THREAD
    // ソース更新
    updateSource();
#endif

    // リソース構築
    ResourceList::Iterator ite = m_ResourceList.Begin();
    ResourceList::Iterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( !(*ite).resource->IsReady() && !(*ite).resource->IsError() ){
            (*ite).resource->Initialize();
        }
    }

    // プラットフォーム依存処理
    updateDepend();
}

/*!
 * マスター音量設定(0.0f～1.0f)
 * @author Miyake_Shunsuke
 * @since 2012.04.13
 */
void TbSoundManager::SetMasterVolume( TbFloat32 volume )
{
    m_MasterVolume = volume;
    updateVolume();
}

/*!
 * ルートパスID取得
 * @author Miyake_Shunsuke
 * @since 2012.04.13
 */
TbFilePathId TbSoundManager::GetRootPathId() const
{
    return TbResourceManager::GetInstance().GetFilePath(m_ListType);
}

/*!
 * サウンドの読み込み
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbResult TbSoundManager::LoadSound( const TbChar8* filename , const TbChar8* name , TbSoundType type , const TbSoundLoadSetting* optionSetting )
{
    if( !IsInitialized() ){
        return TB_E_FAIL;
    }
    TbResult result = TB_E_FAIL;
    
    TbSoundResourceInfo resourceInfo;
    resourceInfo.id = TB_SOUND_ID_INVALID;
    resourceInfo.type = type;
    resourceInfo.name = name;
    
    TbSoundFileType fileType = GetFileTypeFromFileName( filename );
    switch(fileType)
    {
    case TB_SOUND_FILE_TYPE_WAV:
        {
            resourceInfo.id = ++m_IssueSoundId;
            resourceInfo.resource = TB_NEW TbSoundResourceWav(filename,type,optionSetting);
            // 後で対応すべき
//            while( !resourceInfo.resource->Initialize() ) {}
        }
        break;
    case TB_SOUND_FILE_TYPE_OGG:
        {
            resourceInfo.id = ++m_IssueSoundId;
            resourceInfo.resource = TB_NEW TbSoundResourceOgg(filename,type,optionSetting);
        }
        break;
    case TB_SOUND_FILE_TYPE_UNKNOWN:
    default:
        return result;
        break;
    }
    if( resourceInfo.id != TB_SOUND_ID_INVALID && resourceInfo.resource ){
        addResource( resourceInfo );
    }
    return TB_S_OK;
}

/*!
 * サウンドの読み込み
 * @author Miyake Shunsuke
 */
TbResult TbSoundManager::LoadSound( const TbSoundPcmData& data,
                                    const TbChar8* name, 
                                    TbSoundType type, 
                                    const TbSoundLoadSetting* optionSetting )
{
    TbSoundResourceInfo resourceInfo;
    resourceInfo.id = ++m_IssueSoundId;
    resourceInfo.type = type;
    resourceInfo.name = name;
    TbSoundResourcePcm* res = TB_NEW TbSoundResourcePcm(type, optionSetting);
    res->Setup(data);
    resourceInfo.resource = res;
    addResource(resourceInfo);
    return TB_S_OK;
}

/*!
 * サウンドの読み込み解除
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbResult TbSoundManager::UnloadSound( const TbChar8* name )
{
    if( !IsInitialized() ){
        return TB_E_FAIL;
    }
    return deleteResource( name );
}

// 何か読み込み中
TbBool TbSoundManager::IsReadySoundAll() const
{
    TbForIterator( it , m_ResourceList ) {
        if( !it->resource->IsReady() ) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}

/*!
 * サウンドID取得
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundId TbSoundManager::GetSoundId( const TbChar8* name ) const
{
    if( !IsInitialized() ){
        return TB_SOUND_ID_INVALID;
    }
    const TbSoundResourceInfo* resourceInfo = GetResource( name );
    return ( resourceInfo ) ? resourceInfo->id : TB_SOUND_ID_INVALID;
}
    
/*!
 * 名前からバッファ取得
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundResourceInfo* TbSoundManager::GetResource( const TbChar8* name )
{
    if( !IsInitialized() ){
        return nullptr;
    }
    // 線形検索( miyake_todo:要最適化 )
    ResourceList::Iterator ite = m_ResourceList.Begin();
    ResourceList::Iterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( (*ite).name == name ){
            return &(*ite);
        }
    }
    return NULL;
}

/*!
 * 名前からバッファ取得
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
const TbSoundResourceInfo* TbSoundManager::GetResource( const TbChar8* name ) const
{
    if( !IsInitialized() ){
        return nullptr;
    }
    // 線形検索( miyake_todo:要最適化 )
    ResourceList::ConstIterator ite = m_ResourceList.Begin();
    ResourceList::ConstIterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( (*ite).name == name ){
            return &(*ite);
        }
    }
    return NULL;
}

/*!
 * IDからバッファ取得
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbSoundResourceInfo* TbSoundManager::GetResource( const TbSoundId id )
{
    if( !IsInitialized() ){
        return nullptr;
    }
    // 線形検索( miyake_todo:要最適化 )
    ResourceList::Iterator ite = m_ResourceList.Begin();
    ResourceList::Iterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( (*ite).id == id ){
            return &(*ite);
        }
    }
    return NULL;
}

/*!
 * IDからバッファ取得
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
const TbSoundResourceInfo* TbSoundManager::GetResource( const TbSoundId id ) const
{
    if( !IsInitialized() ){
        return nullptr;
    }
    // 線形検索( miyake_todo:要最適化 )
    ResourceList::ConstIterator ite = m_ResourceList.Begin();
    ResourceList::ConstIterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( (*ite).id == id ){
            return &(*ite);
        }
    }
    return NULL;
}

/*!
 * 利用可能状態かどうか
 * @author Miyake Shunsuke
 * @since 2014.04.02
 */
TbBool   TbSoundManager::IsReadySound( TbSoundId id ) const
{
    if( !IsInitialized() ){
        return TB_FALSE;
    }
    const TbSoundResourceInfo* resource = GetResource(id);
    if( resource && resource->resource->IsReady() ){
        return true;
    }
    return false;
}

/*!
 * リスナー設定
 * @author Miyake Shunsuke
 * @since 2011.07.25
 */
TbResult TbSoundManager::SetCurrentListener( TbSoundListener* listener )
{
    if( !IsInitialized() ){
        return TB_E_FAIL;
    }
    m_CurrentListener = listener;
    if( m_CurrentListener ){
        m_CurrentListener->setActive( TB_TRUE );
    }
    return TB_S_OK;
}

/*! 
 * 音源追加
 * @param source　追加する音源
 * @param layerIndex 追加するレイヤー番号
 * @author Miyake Shunsuke
 * @since 2011.07.20
 */
TbResult TbSoundManager::AddSource( TbSoundSource* source , TbUint32 layerIndex )
{
    // 必ずメインスレッドから呼ばれる
    if( !IsInitialized() ){
        return TB_E_FAIL;
    }
    TbScopedLock lock(m_SourceMutex);
    if( source->getNext() ){
        // 登録済み
        return TB_S_OK;
    }
    if( layerIndex < m_Param.layerCount ){
        source->Retain();
        source->setBusy(TB_TRUE);
        source->setLayer( layerIndex , m_LayerInfo[ layerIndex ].top );
        if( !m_LayerInfo[ layerIndex ].top ){
            m_LayerInfo[ layerIndex ].top = source;
        }
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*! 
 * 音源レイヤー影響度設定
 * @param layer レイヤー番号
 * @param weight 影響度
 * @author Miyake Shunsuke
 * @since 2011.07.20
 */
TbResult TbSoundManager::SetSourceLayerWeight( TbUint32 layerIndex , TbFloat32 weight )
{
    if( layerIndex < m_Param.layerCount ){
        m_LayerInfo[ layerIndex ].weight = weight;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*! 
 * 音源レイヤー影響度取得
 * @param layer レイヤー番号
 * @author Miyake Shunsuke
 * @since 2011.07.20
 */
TbFloat32 TbSoundManager::GetSourceLayerWeight( TbUint32 layerIndex ) const
{
    if( layerIndex < m_Param.layerCount ){
        return m_LayerInfo[ layerIndex ].weight;
    }
    return 0.0f;
}

/*!
 * 処理
 * @author teco
 */
void TbSoundManager::updateSource()
{
    // ソース定期処理
    TbSoundSource* source = NULL;
    for (TbUint32 i = 0; i < m_Param.layerCount; ++i) {
        source = m_LayerInfo[i].top;
        if (source) {
            TbBool hasNext = TB_FALSE;
            do {
                TbSoundSource* next = source->getNext();
                hasNext = next && next != m_LayerInfo[i].top;
                if (!source->Update()) {
                    removeSource(source);
                }
                source = next;
            } while (hasNext);
        }
    }
}

/*!
* 音源除去
* @param source　除去する音源
* @author Miyake Shunsuke
* @since 2011.07.20
*/
void TbSoundManager::removeSource(TbSoundSource* source)
{
    TbScopedLock lock(m_SourceMutex);
    if (source && source->GetLayer() < m_Param.layerCount) {
        if (m_LayerInfo[source->GetLayer()].top == source) {
            if (source->getNext() == source) {
                m_LayerInfo[source->GetLayer()].top = NULL;
            }
            else {
                m_LayerInfo[source->GetLayer()].top = source->getNext();
            }
        }
        source->removeLayer();
        source->setBusy(TB_FALSE);
        source->Release();
    }
}

/*!
 * サウンドバッファ追加
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbBool TbSoundManager::addResource( const TbSoundResourceInfo& resourceInfo )
{
    if( !m_ResourceList.IsFull() ){
        m_ResourceList.PushBack(resourceInfo);
        return TB_TRUE;
    }
    return TB_FALSE;
}

/*!
 * サウンドバッファ削除
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbResult TbSoundManager::deleteResource( const TbChar8* name )
{
    // 線形検索( miyake_todo:要最適化 )
    ResourceList::Iterator ite = m_ResourceList.Begin();
    ResourceList::Iterator endIte = m_ResourceList.End();
    for( ; ite != endIte; ++ite ){
        if( (*ite).name == name ){
            if( (*ite).resource->IsLockedStream() ){
                // 使用中のリソースは削除できない
                TB_ASSERT_MSG_FORMAT(0,"[SOUND] %s is used yet!" , name );
                return TB_E_FAIL;
            }
            TB_SAFE_DELETE( (*ite).resource );
            m_ResourceList.Erase(ite);
            return TB_S_OK;
        }
    }
    return TB_E_FAIL;
}

/*!
 * スレッド処理
 * @author Miyake Shunsuke
 * @since 2012.04.02
 */
TbThreadUpdateResult TbSoundManager::onProcStreamThread()
{
//    updateSource();
    m_SourceThread.Sleep(16); // 60Hzで動かす
    return TB_THREAD_UPDATE_RESULT_PENDING;
}

}

