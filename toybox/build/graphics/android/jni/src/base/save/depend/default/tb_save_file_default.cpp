/*!
 * セーブファイル関数郡
 * @author teco
 */

#include "tb_fwd.h"
#include "base/save/tb_save_file.h"
#include "base/io/tb_file_util.h"

namespace toybox
{

/*!
 * 全読み込み
 * @author teco
 */
TbResult TbSaveFile::Read( TbFileAllocMode allocMode )
{
    if (TB_FAILED(m_File.Open(m_FilePath.GetBuffer(), TB_FILE_OPERATE_READ , m_SyncMode, TB_FILE_PATH_ID_PRESET_SAVE))) {
        return TB_E_FAIL;
    }
    return m_File.Read(allocMode);
}

/*!
 * 書き込み
 * @author teco
 */
TbResult TbSaveFile::Write(const void* buffer, TbUint32 size)
{
    if (TB_FAILED(m_File.Open(m_FilePath.GetBuffer(), TB_FILE_OPERATE_WRITE, m_SyncMode, TB_FILE_PATH_ID_PRESET_SAVE))) {
        return TB_E_FAIL;
    }
    return m_File.Write(buffer,size);
}


}