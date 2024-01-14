/*!
 * @file
 * @brief WiiU を利用したファイルマッパークラスのヘッダーファイル
 * @author Miyake Shunsuke
 */
 
#ifndef _INCLUDED_TB_FILE_MAPPER_WIIU_H_
#define _INCLUDED_TB_FILE_MAPPER_WIIU_H_

#include <base/io/tb_file_mapper.h>

namespace toybox
{

class TbFileMapperWiiU : public TbFileMapper , public TbSingletonRef<TbFileMapperWiiU>
{
public:

    // コンストラクタ
    TbFileMapperWiiU();
    
    // デストラクタ
    ~TbFileMapperWiiU();

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
    
public:
    
    FSClient* GetClient() {
        return &m_Client;
    }

    FSCmdBlock* GetCmdBlock() {
        return &m_CmdBlock;
    }

private:
    
    static void stateChangeCallbackStatic(FSClient *client, FSVolumeState state, void* context);
    
private:
    
    // 状態変化コールバック
    void stateChangeCallback(FSClient *client, FSVolumeState state );

    // ファイルシステム情報取得
    void getFileSystems(   TbFileMapper::PathList& result , 
                            const TbStaticString512& dirPath , 
                            TbFilePathId pathId , 
                            TbUint32 searchFlag , 
                            TbUint32 mask , 
                            const TbStaticString512& baseDirPath );
    
private:
    
    FSClient    m_Client;
    FSCmdBlock  m_CmdBlock;
    
};

}

#endif
