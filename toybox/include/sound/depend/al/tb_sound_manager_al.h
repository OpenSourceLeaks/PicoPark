/*!
 * XAudio依存のサウンド管理クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_MANAGER_XAUDIO_H_
#define _INCLUDED_TB_SOUND_MANAGER_AL_H_

#include <sound/tb_sound_types.h>
#include <base/container/tb_static_array.h>

namespace toybox
{

class TbSoundManagerDepend
{
    friend class TbSoundManager;
public:
    
    // コンストラクタ
    TbSoundManagerDepend();

    // コンストラクタ
    explicit TbSoundManagerDepend( const TbSoundInitParam& initParam );

    // デストラクタ
    ~TbSoundManagerDepend(); 

public:

    //! コンテキスト取得
    ALCcontext* GetContext() { return m_Context; }

    //! コンテキスト取得
    const ALCcontext* GetContext() const { return m_Context; }

    // デバイス取得
    ALCdevice* GetDeviceAL() { return m_Device; }

    // デバイス取得
    const ALCdevice* GetDeviceAL() const { return m_Device; }

    // バッファ確保
    TbResult  AllocBuffer( TbUint32 count , ALuint* bufferList );

    // バッファ解放
    void FreeBuffer( TbUint32 count , ALuint* bufferList );

    // 音源確保
    TbResult  AllocSource( TbUint32 count , ALuint* sourceList );

    // 音源解放
    void FreeSource( TbUint32 count , ALuint* sourceList );

private:

    // 初期化
    TbResult initialize( const TbSoundInitParam& initParam );

private:

    ALCcontext*                                     m_Context;      //!< コンテキスト
    ALCdevice*                                      m_Device;       //!< デバイス
    TbStaticArray<ALuint,TB_SOUND_AL_BUFFER_MAX>    m_BufferList;   //!< バッファリスト

    TbStaticArray<ALuint,TB_SOUND_AL_SOURCE_MAX>    m_SourceList;   //!< 音源リスト

};

}

#endif
