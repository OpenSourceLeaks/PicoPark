/*!
 * セーブファイル関数郡
 * @author teco
 */

#include "tb_fwd.h"
#include "base/save/tb_save_file.h"
#include "base/io/tb_file_util.h"
#include <nn/fs/fs_Result.h>

namespace toybox
{

namespace
{
    void save( TbFile* file ,const void* buffer, TbUint32 size )
    {
        nn::Result ret = nn::fs::CreateFile(file->GetFilePath(), size);
        if (ret.IsFailure()) {
            if (!nn::fs::ResultPathAlreadyExists::Includes(ret)) {
                return;
            }
        }
        file->Open();
        file->Write(buffer, size);
    }
};

/*!
 * 全読み込み
 * @author teco
 */
TbResult TbSaveFile::Read( TbFileAllocMode allocMode )
{
    if (TB_FAILED(m_File.Open(m_FilePath.GetBuffer(),
                              TB_FILE_OPERATE_READ , 
                              m_SyncMode, TB_FILE_PATH_ID_PRESET_SAVE))) {
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
    // m_SyncModeを無視して非同期処理
    if (TB_FAILED(m_File.Create(m_FilePath.GetBuffer(),
                                TB_FILE_OPERATE_WRITE,
                                TB_FILE_SYNC, 
                                TB_FILE_PATH_ID_PRESET_SAVE))) {
        return TB_E_FAIL;
    }
    return m_File.RequestUserFuncAsync( TbBind(save,&m_File,buffer,size) );
}

}