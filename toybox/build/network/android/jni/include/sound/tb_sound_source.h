/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_H_
#define _INCLUDED_TB_SOUND_SOURCE_H_

#include <sound/tb_sound_types.h>
#include <base/util/tb_ref_counter.h>
#include <base/math/tb_vector4.h>
#include <base/util/tb_function.h>
#include <sound/tb_sound_stream_buffer.h>
#include "./depend/tb_sound_source_depend.h"

namespace toybox
{

class TbSoundListener;
class TbSoundSourceShape;
class TbStream;

/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.15
 */
class TbSoundSource : public TbRefCounter
{
    friend class TbSoundManager;
    typedef TbStaticFunction64<void (TbSoundSource& source)> FinishedCallback; // 終了コールバック
    typedef TbSoundSource Self;
    typedef TbSoundSourceDepend Depend;
public:

    // コンストラクタ
    TbSoundSource();

    // コンストラクタ
    TB_DEPEND explicit TbSoundSource( TbSoundId soundId );

    // コンストラクタ
    TB_DEPEND explicit TbSoundSource( const char* name );

protected:

    // デストラクタ
    TB_DEPEND ~TbSoundSource();

public:

    // リセット
    TbResult Reset( TbSoundId soundId );

    // リセット
    TbResult Reset( const char* name );

    // 再生
    void Play();

    // 一時停止
    void Pause() {
        m_ReqState = STATE_PAUSE;
    }

    // ストップ
    void Stop() {
        m_ReqState = STATE_STOP;
    }

    // 再生中
    TB_DEPEND TbBool IsPlaying() const;

    // ポーズ中
    TB_DEPEND TbBool IsPaused() const;

    // ビジー状態か
    TbBool  IsBusy() {
        return m_IsBusy.Load();
    }

public:

    // 更新
    TB_DEPEND TbBool Update();

    // 影響度計算
    TbFloat32 CalcWeight( TbSoundListener* listener ) const;

public:

    // サウンドID取得
    TbSoundId   GetSoundId() const {
        if (m_ResouceInfo) {
            return m_ResouceInfo->id;
        }
        return TB_SOUND_ID_INVALID;
    }

    // 再生の準備はできている
    TbBool IsReady() const;

    // エラー状態
    TbBool IsError() const {
        return m_BitArray.Test(FLAG_ERROR);
    }

    // 再生停止中
    TbBool IsStopping() const;

    // ループ設定
    void SetEnableLoop( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_LOOP,isEnable);
    }

    // ループ設定
    TbBool IsEnableLoop() const {
        return m_BitArray.Test(FLAG_LOOP);
    }

    // 再生時間取得
    TbFloat64 GetPlayTimeSec() const;

    // タイプ取得
    TbSoundType GetSoundType() const;

    // 位置設定
    TB_DEPEND TbResult SetPos( const TbVector4& pos );

    //! 位置取得
    const TbVector4& GetPos() const { 
        return m_Pos; 
    }

    // 音量設定
    TB_DEPEND void SetVolume( TbFloat32 volume );

    // 音量取得
    TbFloat32 GetVolume() const {
        return m_Volume;
    }

    // 速度設定
    TB_DEPEND TbResult SetVelo( const TbVector4& velo );

    //! 速度取得
    const TbVector4& GetVelo() const { 
        return m_Velo; 
    }

    //! 形状設定
    TbResult SetShape( TbSoundSourceShape* shape );

    //! 形状取得
    const TbSoundSourceShape* GetShape() const { 
        return m_Shape; 
    }

    // 所属レイヤー設定
    void SetLayer( TbUint32 layer ) {
        // 既にレイヤーに登録されていたら変更不可
        if( TB_VERIFY(!m_Next) ) {
            m_LayerIndex = layer;
        }
    }

    // 所属レイヤー取得
    TbUint32 GetLayer() const {
        return m_LayerIndex; 
    }

public: // 依存部分アクセス

    TbSoundSourceDepend& GetDepend() { return m_Depend; }
    const TbSoundSourceDepend& GetDepend() const { return m_Depend; }

protected:

    // フラグ
    enum Flag
    {
        FLAG_READY , 
        FLAG_ERROR ,
        FLAG_LOOP ,
    };

    // 状態
    enum State
    {
        STATE_INVALID ,
        STATE_PLAY    , 
        STATE_PAUSE   , 
        STATE_STOP    ,
        STATE_MAX     , 
    };

private:

    // リセット
    TB_DEPEND TbResult reset( TbSoundResourceInfo* resourceInfo );

    // 再生(ループ開始位置から)
    TB_DEPEND TbResult play(TbBool isFromLoop);

    // 一時停止
    TB_DEPEND TbResult pause();

    // ストップ
    TB_DEPEND TbResult stop();

    // リセット
    TbResult reset();

    // セットアップ
    void setup();

    // 状態設定
    void setState( State state ) {
        m_State = state;
    }

    // レイヤー設定
    TbResult setLayer( TbUint32 layerIndex , TbSoundSource* next );

    // レイヤーから除去
    TbResult removeLayer();

    // 次取得
    TbSoundSource* getNext() {
        return m_Next;
    }

    // 前取得
    TbSoundSource* getPrev() {
        return m_Prev;
    }

    // 再生済みサンプル数取得
    TbUint32 getPlayedSampleCount() const;

    // ビジー状態
    void setBusy(TbBool isBusy) {
        m_IsBusy = isBusy;
    }

private: // ストリーミング再生用

    // 最初にストリーミングキューに乗せるバッファの読み込みサイズ
    TbUint32 getFirstStreamBufferReadSize() const;

    // ループ時の先頭バッファオフセット
    TbUint32 getLoopFirstBufferOffset() const;

    // ストリームセットアップ
    TbBool activateStream(TbBool isFromLoop);

    // ストリーム非有効化
    void deactivateStream();

    // ストリームバッファ読み込み
    TbResult readStreamRingBlockBuffer( TbUint32 blockCount );

    // ストリームバッファ読み込み完了
    TB_DEPEND void onReadStreamBuffer();

private:

    // ストリーミング用パラメータ
    struct StreamingParam
    {
        StreamingParam() 
            : readSize(0U)
            , readBlockCount(0U)
            , readTotalBlockCount(0U)
            , isEnd(TB_FALSE)
            , stream(NULL)
            , buffer(NULL)
        {}
        TbUint32                readSize;                 //!< 読み込みサイズ
        TbUint32                readBlockCount;           //!< 読み込みブロック数
        TbUint32                readTotalBlockCount;      //!< 読み込んだ累積ブロック数
        TbBool                  isEnd;                    //!< 最後のブロック
        TbStream*               stream;                   //!< ストリーム
        TbSoundStreamBuffer*    buffer;                   //!< 読みこみバッファ
    };

private:

    TbBitArray32            m_BitArray;                 //!< ビット配列
    TbAtomicS32             m_State;                    //!< 状態
    TbAtomicS32             m_ReqState;                 //!< リクエスト状態
    TbFloat32               m_Volume;                   //!< 音量
    TbVector4               m_Pos;                      //!< 位置情報
    TbVector4               m_Velo;                     //!< 速度情報
    TbSoundResourceInfo*    m_ResouceInfo;              //!< リソースインフォ

private: // 遮蔽用

    TbSoundSourceShape* m_Shape;      //!< 形状情報

private: // ストリーミング再生用

    StreamingParam      m_StreamingParam;

private: // レイヤー用

    TbAtomicBool        m_IsBusy;    //!< 次を持っているか(スレッドセーフ用)
    TbUint32            m_LayerIndex; //!< 所属レイヤー
    TbSoundSource*      m_Next;       //!< 次
    TbSoundSource*      m_Prev;       //!< 前

private:

    TbSoundSourceDepend m_Depend;     //!< 依存データ
    
};

}

#endif
