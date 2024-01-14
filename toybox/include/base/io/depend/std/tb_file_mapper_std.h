/*!
 * @file
 * @brief 標準関数を利用したファイルマッパークラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_MAPPER_STD_H_
#define _INCLUDED_TB_FILE_MAPPER_STD_H_

#include <base/io/tb_file_mapper.h>

namespace toybox
{

class TbFileMapperStd : public TbFileMapper
{
public:

    // コンストラクタ
    TbFileMapperStd() 
        : TbFileMapper(TB_TRUE,TB_FALSE) {}

public:
    
    // 対応しているかパスIDかどうか
    virtual TbBool IsEnable( TbFilePathId pathId ) const {
        return TB_TRUE;
    }

    // ファイルサイズ取得
    virtual TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル存在するか
    virtual TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリは存在するか
    virtual TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル実体生成
    virtual TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId );

    // ディレクトリ作成
    virtual TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリ削除
    virtual TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル一覧取得
    virtual void GetFiles( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ディレクトリ一覧取得
    virtual void GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

    // ファイル、ディレクトリ一覧取得
    virtual void GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT );

};

}

#endif
