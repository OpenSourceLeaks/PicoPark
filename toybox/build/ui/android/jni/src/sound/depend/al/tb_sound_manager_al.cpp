/*!
 * OpenAL
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#include "tb_fwd.h"
#include "sound/tb_sound_manager.h"
#include "sound/tb_sound_source.h"
#include "sound/resource/tb_sound_resource_wav.h"
#include "sound/depend/al/tb_sound_util_al.h"
#include "base/string/tb_string_util.h"

#if TB_SOUNDAPI_IS_AL 

namespace toybox
{

namespace
{
    static TbFloat32 LAYER_WEIGHT_DEFAULT = 1.0f;
}

// 依存クラス

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbSoundManagerDepend::TbSoundManagerDepend()
    : m_Device( NULL )
    , m_Context( NULL )
    , m_SourceList()
    , m_BufferList()
{
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbSoundManagerDepend::TbSoundManagerDepend( const TbSoundInitParam& initParam )
    : m_Device( NULL )
    , m_Context( NULL )
    , m_SourceList()
    , m_BufferList()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbSoundManagerDepend::~TbSoundManagerDepend()
{
    alDeleteSources( m_SourceList.GetCapacity() , m_SourceList.GetRawArray() ); 
    alDeleteBuffers( m_BufferList.GetCapacity() , m_BufferList.GetRawArray() ); 
    alcMakeContextCurrent(NULL);
    alcDestroyContext(m_Context);
    alcCloseDevice(m_Device);
}

/*!
 * バッファ確保
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbResult  TbSoundManagerDepend::AllocBuffer( TbUint32 count , ALuint* bufferList )
{
    if( !TB_VERIFY(bufferList && count > 0 ) ){
        return TB_E_INVALIDARG;
    }
    TbUint32 currentCount = m_BufferList.GetCount();
    if( count <= currentCount ){
        for( TbUint32 i = 0; i < count; ++i ){
            bufferList[i] = m_BufferList[currentCount-1-i];
            m_BufferList.PopBack();
        }
    }
    return TB_S_OK;
}

/*!
 * バッファ解放
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
void TbSoundManagerDepend::FreeBuffer( TbUint32 count , ALuint* bufferList )
{
    if( !TB_VERIFY(bufferList && count > 0 ) ){
        return;
    }
    for( TbUint32 i = 0; i < count; ++i ){
        m_BufferList.PushBack( bufferList[i] );
    }
}

/*!
 * 音源確保
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbResult  TbSoundManagerDepend::AllocSource( TbUint32 count , ALuint* sourceList )
{
    if( !TB_VERIFY(sourceList && count > 0 ) ){
        return TB_E_INVALIDARG;
    }
    TbUint32 currentCount = m_SourceList.GetCount();
    if( count <= currentCount ){
        for( TbUint32 i = 0; i < count; ++i ){
            sourceList[i] = m_SourceList[currentCount-1-i];
            m_SourceList.PopBack();
        }
    }
    return TB_S_OK;
}

/*!
 * 音源解放
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
void TbSoundManagerDepend::FreeSource( TbUint32 count , ALuint* sourceList )
{
    if( !TB_VERIFY(sourceList && count > 0 ) ){
        return;
    }
    m_SourceList.PushBackArray( count , sourceList );
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.04.03
 */
TbResult TbSoundManagerDepend::initialize( const TbSoundInitParam& initParam )
{
    m_Device = alcOpenDevice( NULL );
    TB_ASSERT( m_Device );
    if( m_Device ){
        m_Context = alcCreateContext( m_Device , NULL );
        // 現在に設定
        alcMakeContextCurrent( m_Context );
    }

    // 音源とバッファを先に確保しておく
    ALuint sources[TB_SOUND_AL_SOURCE_MAX];
    ALuint buffers[TB_SOUND_AL_BUFFER_MAX];
    alGenSources( m_SourceList.GetCapacity() , sources );
    alGenBuffers( m_BufferList.GetCapacity() , buffers );
    m_SourceList.PushBackArray( TB_SOUND_AL_SOURCE_MAX , sources );
    m_BufferList.PushBackArray( TB_SOUND_AL_BUFFER_MAX , buffers );

    return TB_S_OK;
}

/*!
 * アロケーション
 * @author teco
 */
void* TbSoundManager::AllocBuffer(TbSizeT size, TbSizeT align)
{
    return new TbUint8[size];
}

/*!
* 解放
* @author teco
*/
void TbSoundManager::FreeBuffer(void* ptr)
{
    TbUint8* buf = reinterpret_cast<TbUint8*>(ptr);
    TB_SAFE_DELETE_ARRAY(buf);
}

/*!
* 音量更新
* @author teco
*/
void TbSoundManager::updateVolume()
{
}

/*!
* 定期処理
* @author teco
*/
void TbSoundManager::updateDepend()
{
}

}

#endif