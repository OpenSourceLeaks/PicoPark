/*!
 * リソース(OGG用)
 * @author teco
 */

#ifndef _INCLUDED_TB_SOUND_RESOURCE_OGG_H_
#define _INCLUDED_TB_SOUND_RESOURCE_OGG_H_

#include <sound/resource/tb_sound_resource.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/tb_file.h>

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

namespace toybox
{

class TbFile;

class TbOggFileStream : public TbStream
{
    typedef TbStream Super;
    typedef TbOggFileStream  Self;

public:

    // コンストラクタ
    TbOggFileStream()
        : m_File(nullptr)
    {}

    // デストラクタ
    virtual ~TbOggFileStream()
    {}

private: // コピー禁止

    TB_DISALLOW_COPY_AND_ASSIGN( TbOggFileStream );

public:

    // 参照しているファイル取得
    const OggVorbis_File* GetFile() const {
        return m_File;
    }

    // 初期化
    void Initialize( OggVorbis_File* file , 
                     const TbSoundFormat& format , 
                     TbUint32 pcmSize , 
                     TbUint32 bytePerSample , 
                     TbUint32 offsetPos ) 
    {
        m_File = file;
        m_Format = format;
        m_BytePerSample = bytePerSample;
        m_OffsetBufferPos = offsetPos;
        SetSize( pcmSize );
    }

public:
    
    // 読み込み
    virtual TbResult Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count) {
        int bitstream  = 0;
        TbSint32 restSize = count;
        TbSint32 resultSize = 0;
        while( restSize > 0 ){
            int readSize =::ov_read( m_File , reinterpret_cast<char*>(buf)+resultSize ,restSize , 0 , sizeof(TbUint16) , 1 , &bitstream );
            if( readSize <= 0 ){
                break;
            }
            resultSize += readSize;
            restSize -= readSize;
        }
        if( 0 < resultSize ) {
            SetSeek(GetSeek()+resultSize);
            CallCompletedReadCallback();
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }
    
    // 書き込み
    virtual TbResult Write(const void* buf,TbUint32 count) { TB_ASSERT(0); return TB_E_FAIL; }
    
    // シーク
    virtual TbResult Seek( TbSint32 size , TbIOSeekType type ) {
        TB_ASSERT(TB_IO_SEEK_TYPE_BEGIN==type);
        TbSint32 pos = (size+m_OffsetBufferPos) / (m_BytePerSample * m_Format.channel);
        TbSint32 ret = ::ov_pcm_seek(m_File,pos);
        SetSeek(size);
        return TB_S_OK;
    }

private:

    // 初期化
    void setup();

private:

    TbSoundFormat       m_Format;
    OggVorbis_File*     m_File;             // ファイル
    TbUint32            m_BytePerSample;    // 1サンプリング毎のバイト数
    TbUint32            m_OffsetBufferPos;    // 開始位置

};

class TbSoundResourceOgg : public TbSoundResource
{
public:

    typedef TbSoundResource Super;
    typedef TbSoundResourceOgg Self;

public:

    // コンストラクタ
    explicit TbSoundResourceOgg( const char* filename , TbSoundType type , const TbSoundLoadSetting* setting );

    // デストラクタ
    virtual ~TbSoundResourceOgg();

public:

    // 初期化中
    virtual TbBool Initialize();

private:

    // 読み込み完了
    void onRead();

    // 読みこみ
    TbResult read( TbSizeT size , void* dst , TbSizeT dstSize );

private:

    TbSint32        m_Phase;        // フェーズ
    TbBitArray32    m_BitArray;     // ビット配列
    TbUint32        m_TotalSample;
    TbOggFileStream m_FileStream;   // ストリーミング用ストリーム
    TbFile          m_File;         // ファイル

private:

    OggVorbis_File  m_OvFile;

};

}

#endif
