/*!
 * @file
 * @brief ファイルストリームラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#include <tb_fwd.h>
#include "base/io/stream/tb_file_stream.h"
#include "base/io/tb_file.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbFileStream::TbFileStream()
    : Super()
    , m_BeginSeekPos(0)
    , m_File(NULL)
{
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbFileStream::TbFileStream( TbFile* file )
    : Super()
    , m_BeginSeekPos(0)
    , m_File(file)
{
    Initialize(file);
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbFileStream::TbFileStream( TbFile* file , TbUint32 begin , TbUint32 size )
    : Super()
    , m_BeginSeekPos(begin)
    , m_File(file)
{
    Initialize( file , begin ,size );
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbFileStream::~TbFileStream()
{
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
void TbFileStream::Initialize( TbFile* file )
{
    m_File = file;
    if( file ){
        SetSize( file->GetFileSize() );
        setup();
    }
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
void TbFileStream::Initialize( TbFile* file , TbUint32 begin , TbUint32 size )
{
    m_BeginSeekPos = begin;
    m_File = file;
    if( file ){
        SetSize( size );
        setup();
    }
}


/*!
 * 読み込み
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbResult TbFileStream::Read(TB_OUT void* buf,TbUint32 offset,TbUint32 count)
{
    if( !m_File ){
        return TB_E_FAIL;
    }
    SetReading(TB_TRUE);
    TbResult result = m_File->Read(count,buf,count,nullptr);
    if( !TB_SUCCEEDED(result) ){
        SetReading(TB_FALSE);
    }
    return result;
}

/*!
 * 書き込み
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbResult TbFileStream::Write(const void* buf,TbUint32 count)
{
    if( !m_File ){
        return TB_E_FAIL;
    }
    SetWriting(TB_TRUE);
    TbResult result = m_File->Write(buf,count);
    if( !TB_SUCCEEDED(result) ){
        SetWriting(TB_FALSE);
    }
    return result;
}

/*!
 * シーク
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
TbResult TbFileStream::Seek( TbSint32 size , TbIOSeekType type )
{
    if( !m_File ){
        return TB_E_FAIL;
    }
    switch( type ){
    case TB_IO_SEEK_TYPE_BEGIN:
        m_File->Seek(m_BeginSeekPos+size,type);
        break;
    case TB_IO_SEEK_TYPE_CURRENT:
        m_File->Seek(size,type);
        break;
    case TB_IO_SEEK_TYPE_END:
        m_File->Seek(m_BeginSeekPos+GetSize()+size,TB_IO_SEEK_TYPE_BEGIN);
        break;
    default:
        return TB_E_FAIL;
    }
    SetSeek(m_File->GetSeek()-m_BeginSeekPos);
    return TB_S_OK;
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
void TbFileStream::setup()
{
    if( !m_File ){
        return;
    }
    switch( m_File->GetOperateMode() )
    {
    case TB_FILE_OPERATE_READ:
        m_File->SetCompletedCallback( TbCreateMemFunc(this,&Self::onCompletedRead) );
        break;
    case TB_FILE_OPERATE_WRITE:
        m_File->SetCompletedCallback( TbCreateMemFunc(this,&Self::onCompletedWrite) );
        break;
    default:
        return;
    }
}

/*!
 * 読み込み完了
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
void TbFileStream::onCompletedRead()
{
    if( m_File ){
        SetSeek( m_File->GetSeek()-m_BeginSeekPos );
    }
    SetReading(TB_FALSE);
    CallCompletedReadCallback();
}

/*!
 * 書き込み完了
 * @author Miyake Shunsuke
 * @since 2012.03.07
 */
void TbFileStream::onCompletedWrite()
{
    SetWriting(TB_FALSE);
    CallCompletedWriteCallback();
}

}
