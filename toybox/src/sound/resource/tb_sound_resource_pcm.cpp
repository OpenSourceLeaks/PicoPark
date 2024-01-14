/*!
 * PCM用サウンドリソース
 * @author Miyake Shunsuke
 * @since 2012.03.30
 */

#include "tb_fwd.h"
#include "base/io/tb_file_all.h"
#include "base/util/tb_function.h"
#include "sound/resource/tb_sound_resource_pcm.h"
#include "sound/tb_sound_manager.h"
#include "base/io/stream/tb_file_stream.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundResourcePcm::TbSoundResourcePcm( TbSoundType type , const TbSoundLoadSetting* setting )
    : Super(type,setting)
    , m_BitArray()
    , m_Stream()
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbSoundResourcePcm::~TbSoundResourcePcm()
{
}

/*!
 * セットアップ
 * @author Miyake Shunsuke
 */
void TbSoundResourcePcm::Setup(const TbSoundPcmData& data)
{
    TbSoundFormat format;
    format.type = TB_SOUND_FORMAT_TYPE_PCM;
    format.channel = data.channel;
    format.samplesPerSec = data.samplesPerSec;
    format.bitsPerSample = data.bitsPerSample;
    SetBuffer(data.bufSize, 0);
    SetFormat(format);

    if (TB_SOUND_TYPE_STATIC == GetType())
    {
        TbMemCopy( GetStaticBuffer() , data.buffer , data.bufSize );
    }
    else
    {
        SetStream(&m_Stream);

        TbUint32 firstBufferSize = TbSoundStreamBuffer::GetFirstBufferSize(format);
        TbUint32 readSize = TbMin(firstBufferSize, GetBufferSize());
        SetStreamFirstBuffer(readSize);
        TbMemCopy(GetStreamFirstBuffer(), data.buffer , readSize);

        char* streamBuf = const_cast<char*>(reinterpret_cast<const char*>(data.buffer) + readSize);
        m_Stream.Initialize(streamBuf, data.bufSize- readSize);
    }
}

/*!
 * 手動読み込み
 * @author Miyake Shunsuke
 */
TbBool TbSoundResourcePcm::Initialize()
{
    SetReady();
    return TB_TRUE;
}

}
