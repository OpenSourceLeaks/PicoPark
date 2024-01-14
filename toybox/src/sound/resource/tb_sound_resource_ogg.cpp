/*!
 * OGG用リソース
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "sound/resource/tb_sound_resource_ogg.h"

#include "base/io/tb_file_all.h"
#include "base/util/tb_function.h"
#include "sound/tb_sound_manager.h"
#include "base/io/stream/tb_file_stream.h"

namespace toybox
{

namespace
{

enum Phase
{
    PHASE_BEGIN , 
    PHASE_FORMAT = 10 , 
    PHASE_DATA = 20 ,
    PHASE_LOOP_DATA = 25 ,
    PHASE_STREAM_FIRST_BLOCK = 30 ,
    PHASE_FINISH  = 40 ,
    PHASE_END , 
    PHASE_MAX = PHASE_END - PHASE_BEGIN
};

//! 読み込み
static size_t ovRead( void* buf , size_t size, size_t maxCount, void* ptr )
{
    TbFile* file = reinterpret_cast<TbFile*>(ptr);
    size_t readSize = size * maxCount;
    TbUint32 result = 0;
    TbResult res = file->Read( readSize , buf , readSize , &result );
    return result;
}

//! シーク
static int ovSeek( void* ptr, ogg_int64_t offset, int flag )
{
    TbFile* file = reinterpret_cast<TbFile*>(ptr);
    TbIOSeekType type = TB_IO_SEEK_TYPE_END;
    if( SEEK_CUR == flag ) {
        type = TB_IO_SEEK_TYPE_CURRENT;
    }else if( SEEK_SET == flag ) {
        type = TB_IO_SEEK_TYPE_BEGIN;
    }
    int result = TB_SUCCEEDED(file->Seek( static_cast<TbSint32>(offset) , type )) ? 0 : -1;
    return result;
}

//! クローズ
static int ovClose( void* ptr )
{
    TbFile* file = reinterpret_cast<TbFile*>(ptr);
    return TB_SUCCEEDED(file->Close()) ? 0 : 1;
}

//! 位置通達
static long ovTell( void* ptr )
{
    TbFile* file = reinterpret_cast<TbFile*>(ptr);
    return file->GetSeek();
}

}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundResourceOgg::TbSoundResourceOgg( const char* filepath , TbSoundType type , const TbSoundLoadSetting* setting )
    : Super(type,setting)
    , m_Phase(PHASE_BEGIN)
    , m_BitArray()
    , m_FileStream()
    , m_File(filepath,TB_FILE_OPERATE_READ,TB_FILE_SYNC,TbSoundManager::GetInstance().GetRootPathId())
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbSoundResourceOgg::~TbSoundResourceOgg()
{
}

/*!
 * 手動読み込み
 * @author teco
 */
TbBool TbSoundResourceOgg::Initialize()
{
    switch( m_Phase )
    {
    case PHASE_BEGIN:
        {
            if( TB_SUCCEEDED(m_File.Open() ) ){
                m_File.SetCompletedCallback( TbCreateMemFunc(this,&Self::onRead) );
                m_Phase = PHASE_FORMAT;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
    case PHASE_FORMAT:
        {
            // ヘッダー読み込み            
            ::ov_callbacks cb;
            cb.read_func = ovRead;
            cb.seek_func = ovSeek;
            cb.tell_func = ovTell;
            cb.close_func = ovClose;
            TbBool result = ::ov_open_callbacks(&m_File,
                                                &m_OvFile,
                                                nullptr,
                                                0,
                                                cb);
            if( TB_SUCCEEDED(result) ){
                ++m_Phase;
            }else{
                SetError();
                return TB_TRUE;
            }
        }
        // break;

    case PHASE_FORMAT+1:
        {
            vorbis_info* info = ov_info(&m_OvFile, -1);
            if( !info ) {
                SetError();
                TB_RESQUE_RET(info,TB_TRUE);
            }

            TbSoundFormat format;
            format.type = TB_SOUND_FORMAT_TYPE_PCM;
            format.channel = info->channels;
            format.samplesPerSec = info->rate;
            format.bitsPerSample = 16;
            SetFormat(format);

            // 全サンプル数
            m_TotalSample = static_cast<TbUint32>(::ov_pcm_total(&m_OvFile,-1));
            m_Phase = PHASE_DATA;
        }
        // break;
        
    case PHASE_DATA:
        {
            const TbSoundFormat& format = GetFormat();
            const TbSoundLoadSetting& setting = GetLoadSetting();
            // Skipを考慮した総サンプル数とバッファサイズ
            TbUint32 offsetSample = GetSample(setting.skipSec);
            TbUint32 offsetSize = GetBufferSizeBySec(setting.skipSec);
            m_TotalSample -= offsetSample;
            size_t bufferSize = GetBufferSizeBySample(m_TotalSample);
            SetBuffer( bufferSize , offsetSize );
            if( TB_SOUND_TYPE_STREAM == GetType() ){
                // ストリーミング再生準備
                TbUint32 firstBufferSize = TbSoundStreamBuffer::GetFirstBufferSize(format.samplesPerSec , format.bitsPerSample , format.channel);
                TbUint32 readSize = TbMin( firstBufferSize , GetBufferSize() );
                SetStreamFirstBuffer(readSize);

                if( 0 < offsetSample ){
                    ::ov_pcm_seek(&m_OvFile,offsetSample);
                }
                if( TB_SUCCEEDED(read(readSize,GetStreamFirstBuffer(),readSize)) ){
                    if( 0.0f < setting.loopFirstSec){
                        m_Phase = PHASE_LOOP_DATA;
                    }else{
                        m_Phase = PHASE_STREAM_FIRST_BLOCK;
                    }
                }else{
                    SetError();
                    return TB_TRUE;
                }
            }else{
                // 静的再生
                if( TB_SUCCEEDED(read(bufferSize,GetStaticBuffer(),bufferSize)) ){
                    ++m_Phase;
                }else{
                    SetError();
                    return TB_TRUE;
                }
            }
        }
        break;
    case PHASE_DATA+1:
        {
            // 静的再生完了
            ::ov_clear(&m_OvFile);
            m_Phase = PHASE_FINISH;
            SetReady();
        }
        break;
    case PHASE_LOOP_DATA:
        {
            TB_ASSERT(TB_SOUND_TYPE_STREAM == GetType());

            // ループ再生時間設定
            const TbSoundFormat& format = GetFormat();
            const TbSoundLoadSetting& setting = GetLoadSetting();
            TB_ASSERT(setting.skipSec<setting.loopFirstSec);
            TbUint32 loopFirstSample = GetSample(setting.loopFirstSec);

            // ループのオフセット値はSkipからの差分
            TbUint32 offsetSize = GetBufferSizeBySec(setting.skipSec);
            TbUint32 loopFirstSize = GetBufferSizeBySec(setting.loopFirstSec) - offsetSize;
            SetLoopFirstBufferOffset(loopFirstSize);

            TbUint32 readSize = GetStreamFirstBufferSize();
            ::ov_pcm_seek(&m_OvFile, loopFirstSample );
            if (TB_SUCCEEDED(read(readSize, GetStreamLoopFirstBuffer(), readSize))) {
                m_Phase = PHASE_STREAM_FIRST_BLOCK;
            } else {
                SetError();
                return TB_TRUE;
            }
        }
        break;

    case PHASE_STREAM_FIRST_BLOCK:
        {
            
            const TbSoundFormat& format = GetFormat();
            m_FileStream.Initialize( &m_OvFile ,
                                     GetFormat() , 
                                     GetBufferSize() ,
                                     format.bitsPerSample/8 ,
                                     GetBufferOffset() );
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
 * 読みこみ
 * @author teco
 */
TbResult TbSoundResourceOgg::read( TbSizeT size , void* dst , TbSizeT dstSize ) 
{
    int bitstream = 0;
    TbSint32 restSize = dstSize;
    TbSint32 resultSize = 0;
    while( restSize > 0 ){
        int readSize =::ov_read( &m_OvFile , reinterpret_cast<char*>(dst)+resultSize ,restSize , 0 , sizeof(TbUint16) , 1 , &bitstream );
        if( readSize <= 0 ){
            break;
        }
        resultSize += readSize;
        restSize -= readSize;
    }
    return resultSize > 0 ? TB_S_OK : TB_E_FAIL;
}

/*!
 * 読み込み完了
 * @author teco
 */
void TbSoundResourceOgg::onRead()
{
}

}
