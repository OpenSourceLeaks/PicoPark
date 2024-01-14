/*!
 * WAV用ストリームリーダー
 * @author Miyake Shunsuke
 * @since 2012.03.30
 */

#include "tb_fwd.h"
#include "base/io/tb_file_all.h"
#include "base/util/tb_function.h"
#include "sound/resource/tb_sound_resource_wav.h"
#include "sound/tb_sound_manager.h"
#include "base/io/stream/tb_file_stream.h"

namespace toybox
{

namespace
{

enum Phase
{
    PHASE_BEGIN , 
    PHASE_HEADER = 10 , 
    PHASE_CHUNK = 20 , 
    PHASE_CHUNK_FORMAT = 30 , 
    PHASE_CHUNK_DATA = 40 , 
    PHASE_CHUNK_END = 50 ,
    PHASE_STREAM_FIRST_BLOCK = 60 ,
    PHASE_FINISH  =70 ,
    PHASE_END , 
    PHASE_MAX = PHASE_END - PHASE_BEGIN
};

const TbUint16 FORMAT_PCM = 1;
const TbUint32 CHANNEL_BIT_FRONT_LEFT            = 0x1;
const TbUint32 CHANNEL_BIT_FRONT_RIGHT           = 0x2;
const TbUint32 CHANNEL_BIT_FRONT_CENTER          = 0x4;
const TbUint32 CHANNEL_BIT_LOW_FREQUENCY         = 0x8;
const TbUint32 CHANNEL_BIT_BACK_LEFT             = 0x10;
const TbUint32 CHANNEL_BIT_BACK_RIGHT            = 0x20;
const TbUint32 CHANNEL_BIT_FRONT_LEFT_OF_CENTER  = 0x40;
const TbUint32 CHANNEL_BIT_FRONT_RIGHT_OF_CENTER = 0x80;
const TbUint32 CHANNEL_BIT_BACK_CENTER      = 0x100;
const TbUint32 CHANNEL_BIT_SIDE_LEFT        = 0x200;
const TbUint32 CHANNEL_BIT_SIDE_RIGHT       = 0x400;
const TbUint32 CHANNEL_BIT_TOP_CENTER       = 0x800;
const TbUint32 CHANNEL_BIT_TOP_FRONT_LEFT   = 0x1000e;
const TbUint32 CHANNEL_BIT_TOP_FRONT_CENTER = 0x2000;
const TbUint32 CHANNEL_BIT_TOP_FRONT_RIGHT  = 0x4000;
const TbUint32 CHANNEL_BIT_TOP_BACK_LEFT    = 0x8000;
const TbUint32 CHANNEL_BIT_TOP_BACK_CENTER  = 0x10000;
const TbUint32 CHANNEL_BIT_TOP_BACK_RIGHT   = 0x20000;

}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.31
 */
TbSoundResourceWav::TbSoundResourceWav( const char* filepath , TbSoundType type , const TbSoundLoadSetting* setting )
    : Super(type,setting)
    , m_Phase(PHASE_BEGIN)
    , m_BitArray()
    , m_Header()
    , m_Chunk()
    , m_FormatInfo()
    , m_FileStream()
    , m_File(filepath,TB_FILE_OPERATE_READ,TB_FILE_ASYNC,TbSoundManager::GetInstance().GetRootPathId())
    , m_FileOffset(0)
{
    TbMemClear(&m_Header,sizeof(m_Header));
    TbMemClear(&m_Chunk,sizeof(m_Chunk));
    TbMemClear(&m_FormatInfo,sizeof(m_FormatInfo));
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.31
 */
TbSoundResourceWav::~TbSoundResourceWav()
{
}

/*!
 * 手動読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.31
 */
TbBool TbSoundResourceWav::Initialize()
{
    if( m_BitArray.Test(FLAG_WAIT_READ) ){
//        m_File.Seek(m_FileOffset,TB_IO_SEEK_TYPE_BEGIN);
        return TB_FALSE;
    }

    switch( m_Phase )
    {
    case PHASE_BEGIN:
        {
            if( TB_SUCCEEDED(m_File.Open() ) ){
                m_File.SetCompletedCallback( TbCreateMemFunc(this,&Self::onRead) );
                m_Phase = PHASE_HEADER;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
    case PHASE_HEADER:
        {
            // ヘッダー読み込み
            m_BitArray.SetBit(FLAG_WAIT_READ,TB_TRUE);
            if( TB_SUCCEEDED(read(&m_Header)) ){
                ++m_Phase;
            }else{
                SetError();
                return TB_TRUE;
            }
            if( m_BitArray.Test(FLAG_WAIT_READ) ){
                break;
            }
        }
    case PHASE_HEADER+1:
        {
            // ヘッダーチェック
            if( TbChunkTag4(m_Header.riff) == TbChunkTag4("RIFF") &&
                TbChunkTag4(m_Header.wave) == TbChunkTag4("WAVE") )
            {
                m_Phase = PHASE_CHUNK;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
    case PHASE_CHUNK:
        {
            // チャンク読み込み
            m_BitArray.SetBit(FLAG_WAIT_READ,TB_TRUE);
            if( TB_SUCCEEDED(read(&m_Chunk)) ){
                ++m_Phase;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
        break;
    case PHASE_CHUNK+1:
        {
            if( TbChunkTag3(m_Chunk.tag) == TbChunkTag3("fmt") ){
                m_Phase = PHASE_CHUNK_FORMAT;
            }else if( TbChunkTag4(m_Chunk.tag) == TbChunkTag4("data") ){
                m_Phase = PHASE_CHUNK_DATA;
            }else{
                m_FileOffset += m_Chunk.size;
                m_File.Seek(m_FileOffset,TB_IO_SEEK_TYPE_BEGIN);
                m_Phase = PHASE_CHUNK;
            }
        }
        break;
    case PHASE_CHUNK_FORMAT:
        {
            // チャンク読み込み
            m_BitArray.SetBit(FLAG_WAIT_READ,TB_TRUE);
            if( TB_SUCCEEDED(read(m_Chunk.size,&m_FormatInfo,sizeof(m_FormatInfo))) ){
                ++m_Phase;
            }else{
                SetError();
                return TB_TRUE;
            }
            if( m_BitArray.Test(FLAG_WAIT_READ) ){
                break;
            }
        }
        break;
    case PHASE_CHUNK_FORMAT+1:
        {
            TbSoundFormat format;
            if( getSoundFormat( format , m_FormatInfo ) ){
                SetFormat( format );
                m_Phase = PHASE_CHUNK_END;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
        break;
    case PHASE_CHUNK_DATA:
        {
            SetBuffer( m_Chunk.size , m_File.GetSeek() );
            if( TB_SOUND_TYPE_STREAM == GetType() ){
                // ストリーミング再生準備
                const TbSoundFormat& format = GetFormat();
                TbUint32 firstBufferSize = TbSoundStreamBuffer::GetFirstBufferSize(format.samplesPerSec , format.bitsPerSample , format.channel);
                TbUint32 readSize = TbMin( firstBufferSize , GetBufferSize() );
                SetStreamFirstBuffer(readSize);
                m_BitArray.SetBit(FLAG_WAIT_READ,TB_TRUE);
                if( TB_SUCCEEDED(read(readSize,GetStreamFirstBuffer(),readSize)) ){
                    m_Phase = PHASE_STREAM_FIRST_BLOCK;
                }else{
                    SetError();
                    return TB_TRUE;
                }
            }else{
                // 静的再生
                m_BitArray.SetBit(FLAG_WAIT_READ,TB_TRUE);
                if( TB_SUCCEEDED(read(m_Chunk.size,GetStaticBuffer(),m_Chunk.size)) ){
                    ++m_Phase;
                }else{
                    SetError();
                    return TB_TRUE;
                }
                if( m_BitArray.Test(FLAG_WAIT_READ) ){
                    break;
                }
            }
        }
        break;
    case PHASE_CHUNK_DATA+1:
        {
            // 静的再生完了
            m_File.Close();
            m_Phase = PHASE_FINISH;
            SetReady();
            return TB_TRUE;
        }
        break;
    case PHASE_CHUNK_END:
        {
            // アライメントを補正
            if( m_Chunk.size & 1 ){
                m_File.Seek( 1 , TB_IO_SEEK_TYPE_CURRENT );
            }
            m_Phase = PHASE_CHUNK;
        }
        break;
    case PHASE_STREAM_FIRST_BLOCK:
        {
            m_FileStream.Initialize( &m_File , GetBufferOffset() , GetBufferSize() );
            SetStream(&m_FileStream);
            m_Phase = PHASE_FINISH;
            SetReady();
        }
        break;
    case PHASE_FINISH:
        return TB_TRUE;
        break;
    };
    return TB_FALSE;
}

/*!
 * 読み込み完了
 * @author Miyake Shunsuke
 * @since 2012.03.31
 */
void TbSoundResourceWav::onRead()
{
    m_BitArray.SetBit(FLAG_WAIT_READ,TB_FALSE);
}

/*!
 * フォーマット取得
 * @author Miyake Shunsuke
 * @since 2012.04.01
 */
TbBool TbSoundResourceWav::getSoundFormat( TbSoundFormat& dst , const FormatInfo& src )
{
    TbBool result = TB_FALSE;
    dst.type = TB_SOUND_FORMAT_TYPE_INVALID;
    if( TB_VERIFY(src.formatId == FORMAT_PCM) )
    {
        dst.type = TB_SOUND_FORMAT_TYPE_PCM;
        dst.channel = src.channelCount;
        dst.bitsPerSample = src.bitsPerSample;
        dst.samplesPerSec = src.samplesPerSec;
        result = TB_TRUE;
    }
    return result;
}

}
