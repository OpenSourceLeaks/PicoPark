/*!
 * @file
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_SAVE_NX_H_
#define _INCLUDED_TB_FILE_SAVE_NX_H_

#include "tb_file_save_nx.h"
#include <base/container/tb_array.h>

namespace toybox
{

class TbFileManager;

/*!
 * @class 
 */
class TbFileSaveNX : public TbFileNX
{
    typedef TbFileNX Super;
    friend class TbFileManager;
    friend class TbFileMapperSaveNX;

public:

    // ファイルサイズ取得
    static TbUint32 GetFileSize(const char* path, TbFilePathId pathId) {
        return TbFileNX::GetFileSize(path,pathId);
    }

    // ファイル存在しているか
    static TbBool ExistsFile(const char* path, TbFilePathId pathId) {
        return TbFileNX::ExistsFile(path, pathId);
    }

protected:

    // コンストラクタ
    TbFileSaveNX(const char* path, TbFileOperateMode operateMode, TbFileSyncMode syncMode, TbFilePathId pathId)
        : Super(path,operateMode,syncMode,pathId)
    {}

    // デストラクタ
    virtual ~TbFileSaveNX()
    {}

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbFileSaveNX);

public:

    // 閉じる
    virtual TbResult Close() TB_OVERRIDE;

};

}

#endif
