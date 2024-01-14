/*!
 * サウンドストリームリーダー
 * @author Miyake Shunsuke
 * @since 2012.03.30
 */

#ifndef _INCLUDED_TB_SOUND_RESOURCE_H_
#define _INCLUDED_TB_SOUND_RESOURCE_H_

#include <sound/tb_sound_types.h>
#include <sound/tb_sound_manager.h>
#include <base/io/stream/tb_stream.h>

namespace toybox
{

class TbFile;

/*!
 * サウンドリソースデータ部基底クラス
 * @author Miyake_Shunsuke
 * @since 2013.11.26
 */
class TbSoundResource
{
public:
    
    typedef TbSoundResource Self;
    
public:

    // コンストラクタ
    explicit TbSoundResource( TbSoundType type , const TbSoundLoadSetting* setting ) 
        : m_Type(type)
        , m_Buffer(nullptr)
        , m_BufferOffset(0U)
        , m_BufferSize(0U)
        , m_FirstBufferSize(0U)
        , m_LoopFirstBuffer(nullptr)
        , m_LoopFirstBufferOffset(0)
        , m_Format()
        , m_Stream(nullptr)
        , m_LoadSetting()
    {
        if( setting ){
            m_LoadSetting = *setting;
        }
    }

    // デストラクタ
    virtual ~TbSoundResource() {
        TbSoundManager::GetInstance().FreeBuffer(m_Buffer);
        if (m_LoopFirstBuffer) {
            TbSoundManager::GetInstance().FreeBuffer(m_LoopFirstBuffer);
        }
    }

public:

    // 初期化中
    virtual TbBool Initialize() = 0;

public:

    //! タイプ取得
    TbSoundType GetType() const {
        return m_Type;
    }

    //! ファイル先頭からバッファまでのオフセット
    TbUint32 GetBufferOffset() const {
        return m_BufferOffset;
    }

    //! バッファサイズ取得
    TbUint32 GetBufferSize() const {
        return m_BufferSize;
    }
    
    //! フォーマット取得
    const TbSoundFormat& GetFormat() const {
        return m_Format;
    }

    //! 利用状態か
    TbBool IsReady() const {
        return m_BitArray.Test(FLAG_READY);
    }

    //! エラーが発生したか
    TbBool IsError() const {
        return m_BitArray.Test(FLAG_ERROR);
    }

    // 読み込みオプション
    const TbSoundLoadSetting& GetLoadSetting() const {
        return m_LoadSetting;
    }

public: // 静的再生用

    //! バッファ取得
    TbUint8* GetStaticBuffer() {
        if( TB_SOUND_TYPE_STATIC == m_Type ) {
            return m_Buffer;
        }
        return NULL;
    }

    //! バッファ取得
    const TbUint8* GetStaticBuffer() const {
        if( TB_SOUND_TYPE_STATIC == m_Type ) {
            return m_Buffer;
        }
        return NULL;
    }

public: // ストリーミング再生用

    // ストリーミング用先頭バッファ取得
    TbUint8* GetStreamFirstBuffer() {
        if( TB_SOUND_TYPE_STREAM == m_Type ) {
            return m_Buffer;
        }
        return NULL;
    }

    // ストリーミング用先頭バッファ取得
    const TbUint8* GetStreamFirstBuffer() const {
        if( TB_SOUND_TYPE_STREAM == m_Type ) {
            return m_Buffer;
        }
        return NULL;
    }

    // ストリーミング用先頭バッファ取得
    TbUint8* GetStreamLoopFirstBuffer() {
        if (TB_SOUND_TYPE_STREAM == m_Type) {
            if(m_LoopFirstBuffer) {
                return m_LoopFirstBuffer;
            }
            return m_Buffer;
        }
        return NULL;
    }

    // ストリーミング用先頭バッファ取得
    const TbUint8* GetStreamLoopFirstBuffer() const {
        if (TB_SOUND_TYPE_STREAM == m_Type) {
            return m_LoopFirstBuffer;
        }
        return NULL;
    }

    // ストリーミング用先頭バッファサイズ取得
    TbUint32 GetStreamFirstBufferSize() const {
        return m_FirstBufferSize;
    }

    // ループ開始バッファオフセット設定
    TbUint32 GetLoopFirstBufferOffset() const {
        return m_LoopFirstBufferOffset;
    }

    // ストリームがロックされている
    TbBool IsLockedStream() const {
        return m_BitArray.Test(FLAG_LOCK_STREAM);
    }

    // ストリームを獲得
    TbStream* LockStream() {
        if( m_Stream && !m_BitArray.Test(FLAG_LOCK_STREAM) ){
            m_BitArray.SetBit(FLAG_LOCK_STREAM);
            return m_Stream;
        }
        return NULL;
    }
    
    // ストリームを獲得を解除
    void UnlockStream() {
        if( m_Stream ) {
            m_BitArray.SetBit(FLAG_LOCK_STREAM,TB_FALSE);
            m_Stream->SetCompletedReadCallback(TbStream::CallbackType());
        }
    }

protected:

    // バッファ設定
    void SetBuffer( TbUint32 size , TbUint32 offset ) {
        m_BufferSize = size;
        m_BufferOffset = offset;
        if( TB_SOUND_TYPE_STATIC == m_Type ){
            TbSoundManager::GetInstance().FreeBuffer(m_Buffer);
            m_Buffer = reinterpret_cast<TbUint8*>(TbSoundManager::GetInstance().AllocBuffer(size, TB_SOUND_BUFFER_ALIGN_SIZE));
        }
    }

    // ループ開始バッファオフセット設定
    void SetLoopFirstBufferOffset(TbUint32 offset) {
        m_LoopFirstBufferOffset = offset;
    }

    // 先頭バッファ設定
    void SetStreamFirstBuffer( TbUint32 size ) {
        m_FirstBufferSize = size;
        if( TB_SOUND_TYPE_STREAM == m_Type ){
            m_Buffer = reinterpret_cast<TbUint8*>(TbSoundManager::GetInstance().AllocBuffer(size, TB_SOUND_BUFFER_ALIGN_SIZE));
            if (0.0f < m_LoadSetting.loopFirstSec)
            {
                m_LoopFirstBuffer = reinterpret_cast<TbUint8*>(TbSoundManager::GetInstance().AllocBuffer(size, TB_SOUND_BUFFER_ALIGN_SIZE));
            }
        }
    }

    // 時間指定でのサンプル取得
    TbUint32 GetSample( TbFloat32 sec )
    {
        const TbSoundFormat& format = GetFormat();
        TbUint32 sample = static_cast<TbFloat32>(format.samplesPerSec) * sec;
        return sample;
    }

    // 時間指定でのバッファサイズ取得
    TbUint32 GetBufferSizeBySample(TbUint32 sample)
    {
        const TbSoundFormat& format = GetFormat();
        TbUint32 bytePerSample = format.bitsPerSample / 8;
        TbUint32 pos = sample * bytePerSample * format.channel;
        return pos;
    }

    // 時間指定でのバッファサイズ取得
    TbUint32 GetBufferSizeBySec( TbFloat32 sec)
    {
        const TbSoundFormat& format = GetFormat();
        TbUint32 bytePerSample = format.bitsPerSample / 8;
        TbUint32 sample = GetSample(sec);
        TbUint32 pos = sample * bytePerSample * format.channel;
        return pos;
    }

    // フォーマット設定
    void SetFormat( const TbSoundFormat& format ) {
        m_Format = format;
    }


    //! 利用状態か
    void SetReady() {
        m_BitArray.SetBit(FLAG_READY);
    }

    //! エラーが発生したか
    void SetError() {
        m_BitArray.SetBit(FLAG_ERROR);
    }

    // ストリーミング再生用ストリーム設定
    void SetStream( TbStream* stream ){
        m_Stream = stream;
    }
    
private:
    
    enum Flag {
        FLAG_LOCK_STREAM ,
        FLAG_READY , 
        FLAG_ERROR ,
        FLAG_MAX
    };

private:

    TbBitArray32            m_BitArray;         // ビット配列
    TbSoundType             m_Type;             // タイプ
    TbUint8*                m_Buffer;           // バッファ
    TbUint32                m_BufferOffset;     // バッファまでのオフセット位置
    TbUint32                m_BufferSize;       // バッファサイズ
    TbUint32                m_FirstBufferSize;  // ストリーミング用先頭バッファサイズ
    TbUint8*                m_LoopFirstBuffer;  // ループ開始バッファ
    TbUint32                m_LoopFirstBufferOffset; // ループ開始バッファまでのオフセット位置
    TbSoundFormat           m_Format;           // フォーマット
    TbStream*               m_Stream;           // ストリーミング再生用ストリーム
    TbSoundLoadSetting      m_LoadSetting;      // 読み込みオプション

};

}

#endif
