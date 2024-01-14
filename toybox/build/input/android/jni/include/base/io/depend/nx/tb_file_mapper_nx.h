/*!
 * @file
 * @brief Win32 APIを利用したファイルマッパークラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_MAPPER_NX_H_
#define _INCLUDED_TB_FILE_MAPPER_NX_H_

#include <base/io/tb_file_mapper.h>

namespace toybox
{

class TbFileMapperNX : public TbFileMapper
{
public:

    // コンストラクタ
    TbFileMapperNX();

    // デストラクタ
    virtual ~TbFileMapperNX();

public:
    
    // 対応しているかパスIDかどうか
    virtual TbBool IsEnable( TbFilePathId pathId ) const TB_OVERRIDE {
        return TB_TRUE;
    }

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ファイル存在するか
    virtual TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ディレクトリは存在するか
    virtual TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ファイル実体生成
    virtual TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId ) TB_OVERRIDE;

    // ディレクトリ作成
    virtual TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ディレクトリ削除
    virtual TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ファイル一覧取得
    virtual void GetFiles( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ディレクトリ一覧取得
    virtual void GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

    // ファイル、ディレクトリ一覧取得
    virtual void GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) TB_OVERRIDE;

private:

    TbUint8* m_MountRomCache;
    nn::account::UserHandle m_UserHandle;

};

}

#endif
