/*!
 * 実際のファイル読みこみ以外に
 * アーカイブ、ネットワーク読みこみ、DLC対応などに対応するための
 * ファイルを仮想レイヤーにマッピングしてTbFileインタフェースで操作できるようにするためのクラス
 * @author Miyake_Shunsuke
 * @since 2014.06.24
 */

#ifndef _INCLUDED_TB_FILE_MAPPER_H_
#define _INCLUDED_TB_FILE_MAPPER_H_

#include <base/container/tb_array.h>
#include <base/io/tb_path.h>

namespace toybox
{

class TbFileImpl;

class TbFileMapper
{
public:

    typedef TbArray<TbStaticPath512> PathList;

public:

    // コンストラクタ
    TbFileMapper( TbBool isEnableWrite , TbBool needCheckExists ) : m_Priority(0) {
        m_BitArray.SetBit(FLAG_ENABLE_WRITE,isEnableWrite);
        m_BitArray.SetBit(FLAG_CHECK_EXISTS,needCheckExists);
    }

public:

    // 優先度
    void SetPriority( TbSint32 priority ) {
        m_Priority = priority;
    }

    // 優先度
    TbSint32 GetPriority() const {
        return m_Priority;
    }

    // 書き込み可能
    TbBool IsEnableWrite() const {
        return m_BitArray.Test(FLAG_ENABLE_WRITE);
    }

    // 存在チェックをする必要がある
    TbBool NeedCheckExists() const {
        return m_BitArray.Test(FLAG_CHECK_EXISTS);
    }

public:

    // 対応しているかパスIDかどうか
    virtual TbBool IsEnable( TbFilePathId pathId ) const = 0;
    
    // ファイルサイズ取得
    virtual TbUint32 GetFileSize( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ファイル存在するか
    virtual TbBool ExistsFile( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ディレクトリは存在するか
    virtual TbBool ExistsDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ファイル実体生成
    virtual TbFileImpl* CreateFileImpl( const char* path , TbFileOperateMode operateMode , TbFileSyncMode syncMode , TbFilePathId pathId ) = 0;

    // ディレクトリ作成
    virtual TbResult CreateDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ディレクトリ削除
    virtual TbResult DeleteDirectory( const char* path , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ファイル一覧取得
    virtual void GetFiles( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ディレクトリ一覧取得
    virtual void GetDirectories( PathList& result ,const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

    // ファイル、ディレクトリ一覧取得
    virtual void GetFileSystemEntries( PathList& result , const char* path , TbUint32 searchFlag = 0 , TbFilePathId pathId = TB_FILE_PATH_ID_PRESET_ROOT ) = 0;

private:

    enum Flag 
    {
        FLAG_CHECK_EXISTS , // このマッパーを利用する歳はファイル存在チェックをする必要があるか
        FLAG_ENABLE_WRITE , // 書き込み可能
    };

private:

    TbSint32        m_Priority;    // 優先度
    TbBitArray32    m_BitArray;    //

};

}

#endif
