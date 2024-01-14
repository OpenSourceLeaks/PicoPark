/*!
 * ファイルクラス
 * @author Miyake_Shunsuke
 * @since 2013.11.12
 */

#include "tb_fwd.h"
#include "base/io/tb_file.h"
#include "base/io/tb_file_manager.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2013.11.16
 */
TbFile::TbFile()
    : m_FileImpl(NULL)
{
}

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2013.11.16
 */
TbFile::TbFile( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
    : m_FileImpl(NULL)
{
    m_FileImpl = TbFileManager::GetInstance().CreateFileImpl(path,operateMode,syncMode,pathId);
    TB_ASSERT(m_FileImpl);
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2013.11.16
 */
TbFile::~TbFile()
{
    Release();
}

/*!
 * 実体を解放
 * @author Miyake_Shunsuke
 * @since 2013.11.16
 */
void TbFile::Release()
{
    if( m_FileImpl ){
        TbFileManager::GetInstance().DeleteFileImpl(m_FileImpl);
        m_FileImpl = NULL;
    }
}

/*!
 * 開く
 * @author Miyake_Shunsuke
 * @since 2013.11.16
 */
TbResult TbFile::Create( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId )
{
    if( m_FileImpl ){
        // 開き済みのものは削除
        TbFileManager::GetInstance().DeleteFileImpl(m_FileImpl);
        m_FileImpl = NULL;
    }
    m_FileImpl = TbFileManager::GetInstance().CreateFileImpl(path,operateMode,syncMode,pathId);
    if( !TB_VERIFY(m_FileImpl) ){
        return TB_E_FAIL;
    }
    return TB_S_OK;
}

}