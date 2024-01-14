/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_HANDLE_H_
#define _INCLUDED_TB_SOUND_SOURCE_HANDLE_H_

#include <sound/tb_sound_source.h>

namespace toybox
{

/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.15
 */
class TbSoundSourceHandle
{
public:

    // コンストラクタ
    TbSoundSourceHandle();

    // コンストラクタ
    explicit TbSoundSourceHandle( TbSoundId soundId );

    // コンストラクタ
    explicit TbSoundSourceHandle( const char* name );

    // デストラクタ
    ~TbSoundSourceHandle();

public:

    // リセット
    TbResult Reset( TbSoundId soundId ) {
        if (m_Source) {
            return m_Source->Reset(soundId);
        }
        return TB_S_OK;
    }

    // リセット
    TbResult Reset( const char* name ) {
        if (m_Source) {
            return m_Source->Reset(name);
        }
        return TB_S_OK;
    }

    // 再生
    void Play() {
        if (m_Source) {
            m_Source->Play();
        }
    }

    // 一時停止
    void Pause() {
        if (m_Source) {
            return m_Source->Pause();
        }
    }

    // ストップ
    void Stop() {
        if (m_Source) {
            m_Source->Stop();
        }
    }

    // 再生中
    TbBool IsPlaying() const {
        if (m_Source) {
            return m_Source->IsPlaying();
        }
        return TB_FALSE;
    }


    // ポーズ中
    TbBool IsPaused() const {
        if (m_Source) {
            return m_Source->IsPaused();
        }
        return TB_FALSE;
    }

    // ビジー状態か
    TbBool IsBusy() const {
        if (m_Source) {
            return m_Source->IsBusy();
        }
        return TB_FALSE;
    }

public:

    // サウンドID取得
    TbSoundId   GetSoundId() const {
        if (m_Source) {
            return m_Source->GetSoundId();
        }
        return TB_SOUND_ID_INVALID;
    }

    // 再生の準備はできている
    TbBool IsReady() const {
        if (m_Source) {
            return m_Source->IsReady();
        }
        return TB_FALSE;
    }

    // エラー状態
    TbBool IsError() const {
        if (m_Source) {
            return m_Source->IsError();
        }
        return TB_FALSE;
    }

    // 再生停止中
    TbBool IsStopping() const {
        if (m_Source) {
            return m_Source->IsStopping();
        }
        return TB_FALSE;
    }

    // ループ設定
    void SetEnableLoop( TbBool isEnable ) {
        if (m_Source) {
            m_Source->SetEnableLoop(isEnable);
        }
    }

    // ループ設定
    TbBool IsEnableLoop() const {
        if (m_Source) {
            return m_Source->IsEnableLoop();
        }
        return TB_FALSE;
    }

    // 再生時間取得
    TbFloat64 GetPlayTimeSec() const{
        if (m_Source) {
            return m_Source->GetPlayTimeSec();
        }
        return 0.0;
    }

    // タイプ取得
    TbSoundType GetSoundType() const {
        if (m_Source) {
            return m_Source->GetSoundType();
        }
        return TB_SOUND_TYPE_INVALID;
    }

    // 位置設定
    TbResult SetPos( const TbVector4& pos ) {
        if (m_Source) {
            return m_Source->SetPos(pos);
        }
        return TB_S_OK;
    }

    //! 位置取得
    TbVector4 GetPos() const {
        if (m_Source) {
            return m_Source->GetPos();
        }
        return TbVector4();
    }

    // 音量設定
    void SetVolume( TbFloat32 volume ) {
        if (m_Source) {
            m_Source->SetVolume(volume);
        }
    }

    // 音量取得
    TbFloat32 GetVolume() const {
        if (m_Source) {
            return m_Source->GetVolume();
        }
        return 0.0f;
    }

    // 速度設定
    TbResult SetVelo( const TbVector4& velo ) {
        if (m_Source) {
            m_Source->SetVelo(velo);
        }
        return TB_S_OK;
    }

    //! 速度取得
    TbVector4 GetVelo() const {
        if (m_Source) {
            return m_Source->GetVelo();
        }
        return TbVector4();
    }

    //! 形状設定
    TbResult SetShape( TbSoundSourceShape* shape ) {
        if (m_Source) {
            m_Source->SetShape(shape);
        }
        return TB_S_OK;
    }

    //! 形状取得
    const TbSoundSourceShape* GetShape() const {
        if (m_Source) {
            return m_Source->GetShape();
        }
        return nullptr;
    }

private:

    TbSoundSource*      m_Source;     //!< 実体
    
};

}

#endif
