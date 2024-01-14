/*!
 * サウンド管理クラス
 * @author Miyake Shunsuke
 * @since 2011.07.13
 */

#ifndef _INCLUDED_TB_SOUND_MANAGER_H_
#define _INCLUDED_TB_SOUND_MANAGER_H_

#include <sound/tb_sound_types.h>
#include <sound/tb_sound_stream_buffer.h>
#include <base/container/tb_array.h>
#include <base/util/tb_singleton.h>
#include <base/io/tb_file_types.h>
#include <base/thread/tb_func_thread.h>
#include <base/thread/tb_mutex.h>
#include "./depend/tb_sound_manager_depend.h"

namespace toybox
{

class TbSoundShelterManager; // 遮蔽管理クラス
class TbSoundSource;
class TbSoundSourceHandle;
class TbSoundListener;

class TbSoundManager : public TbSingletonRef<TbSoundManager>
{
public:
    typedef TbSoundManager Self;
public:

    // ファイル名からファイルタイプ取得
    static TbSoundFileType GetFileTypeFromFileName( const TbChar8* filename );

public:
    
    // コンストラクタ
    TbSoundManager();

    // コンストラクタ
    explicit TbSoundManager( const TbSoundInitParam& initParam );
    
    // デストラクタ
    ~TbSoundManager();
    
public:
    
    // 初期化
    TbResult Initialize( const TbSoundInitParam& initParam );
    
    // 初期化済み
    TbBool IsInitialized() const {
        return m_BitArray.Test(FLAG_INITIALIZED);
    }

    // 更新
    void Update();

    // ルートパスID取得
    TbFilePathId GetRootPathId() const;

public: // 簡易再生

        // 簡易再生
    TbResult PlaySound(const char* name);

    // 簡易再生
    TbResult PlaySound( const char* name , TbFloat32 volume );

public:

    // サウンドの読み込み
    TbResult LoadSound( const TbChar8* filename , const TbChar8* name , TbSoundType type , const TbSoundLoadSetting* optionSetting = nullptr );

    // サウンドの読み込み
    TbResult LoadSound(const TbSoundPcmData& data,
                        const TbChar8* name, 
                        TbSoundType type, 
                        const TbSoundLoadSetting* optionSetting = nullptr);

    // サウンドの読み込み解除
    TbResult UnloadSound( const TbChar8* name );

    // 全て利用可能状態か
    TbBool IsReadySoundAll() const;

    // 利用可能状態かどうか
    TbBool   IsReadySound( const TbChar8* name ) const {
        return IsReadySound( GetSoundId(name) );
    }

    // 利用可能状態かどうか
    TbBool   IsReadySound( TbSoundId id ) const;

    // サウンドID取得
    TbSoundId GetSoundId( const TbChar8* name ) const;

public: // リスナー設定

    // リスナー設定
    TbResult SetCurrentListener( TbSoundListener* listener );

    //! 現在設定されているリスナー取得
    TbSoundListener* GetCurrentListener() { return m_CurrentListener; }
    
    //! 現在設定されているリスナー取得
    const TbSoundListener* GetCurrentListener() const { return m_CurrentListener; }

public: // ストリーミング設定
    
    // 利用可能なストリーミングバッファ取得
    TbSoundStreamBuffer* LockStreamBuffer();

    // 利用可能なストリーミングバッファ取得
    TbBool UnlockStreamBuffer( TbSoundStreamBuffer* buffer );

public: // 音源設定

    // 音源追加
    TbResult AddSource( TbSoundSource* source , TbUint32 layerIndex = 0 );

    // 音源レイヤー影響度設定(0.0f-1.0f) 主に感情表現や、割り込みによる一時消音などの設定を目的
    TbResult SetSourceLayerWeight( TbUint32 layerIndex , TbFloat32 weight );

    // 音源レイヤー影響度取得
    TbFloat32 GetSourceLayerWeight( TbUint32 layerIndex ) const;

    // マスター音量設定
    void SetMasterVolume( TbFloat32 volume );

    // マスター音量取得
    TbFloat32 GetMasterVolume() const {
        return m_MasterVolume;
    }

public: // 遮蔽

public: // 依存

    TbSoundManagerDepend& GetDepend() { return m_Depend; }
    const TbSoundManagerDepend& GetDepend() const { return m_Depend; }

public:

    // 名前からバッファ取得
    TbSoundResourceInfo* GetResource( const TbChar8* name );

    // 名前からバッファ取得
    const TbSoundResourceInfo* GetResource( const TbChar8* name ) const;

    // IDからバッファ取得
    TbSoundResourceInfo* GetResource( const TbSoundId id );

    // IDからバッファ取得
    const TbSoundResourceInfo* GetResource( const TbSoundId id ) const;

public: // サウンドバッファ専用アロケーション

    // アロケーション
    void* AllocBuffer( TbSizeT size , TbSizeT align );

    //  解放
    void FreeBuffer( void* ptr );

private:

    // レイヤー情報
    struct LayerInfo
    {
        LayerInfo() : bitArray() , weight(1.0f) , top(NULL) {}
        ~LayerInfo() {}
        TbBitArray32 bitArray;
        TbFloat32 weight;
        TbSoundSource* top;
    };

    // 初期化フラグ
    enum Flag
    {
        FLAG_INITIALIZED , 
    };

    typedef TbArray<TbSoundResourceInfo> ResourceList;
    
private:

    // 処理
    void updateSource();
    
    // 音源除去
    void removeSource(TbSoundSource* source);

    // サウンドリソース追加
    TbBool   addResource( const TbSoundResourceInfo& bufferInfo );

    // サウンドリソース削除
    TbResult deleteResource( const TbChar8* name );

    // 依存定期処理
    TB_DEPEND void updateDepend();

    // 音量更新
    TB_DEPEND void updateVolume();

    // スレッド処理
    TbThreadUpdateResult onProcStreamThread();
    
private:

    TbBitArray32            m_BitArray;          //!< ビット配列
    TbSoundInitParam        m_Param;             //!< 初期化パラメータ
    TbSoundId               m_IssueSoundId;      //!< 発行用サウンドId
    TbUint32                m_ListType;          //!< リストタイプ
    TbFloat32               m_MasterVolume;      //!< マスター音量

private: // 再生用情報

    ResourceList            m_ResourceList;      //!< サウンドバッファリスト
    TbSoundListener*        m_CurrentListener;   //!< リスナー
    LayerInfo*              m_LayerInfo;         //!< レイヤー情報

private: // 簡易再生用

    TbSoundSourceHandle*          m_SimplePlaySources;             //!< 簡易再生用ソース

private: // サウンドスレッド

    TbFuncThread            m_SourceThread;
    TbMutex                 m_SourceMutex;

private: 

    TbSoundManagerDepend    m_Depend;            //!< 依存クラス

};

// 簡易SE再生関数
inline void TbPlaySound( const char* name )
{
    TbSoundManager::GetInstance().PlaySound(name);
}

// 簡易SE再生関数
inline void TbPlaySound(const char* name, TbFloat32 volume )
{
    TbSoundManager::GetInstance().PlaySound(name, volume);
}

}

#endif
