/*!
 * リソースハンドル
 * 生成、破棄をコントロールしやすくした
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#ifndef _INCLUDED_TB_RESOURCE_H_
#define _INCLUDED_TB_RESOURCE_H_

#include <base/io/tb_file.h>
#include <base/io/resource/tb_resource_types.h>
#include <base/util/tb_type_id.h>
#include <base/util/tb_ref_counter.h>
#include <base/util/tb_function.h>
#include <base/string/tb_static_string.h>

namespace toybox
{

class TbResourceData;

// リソース基底クラス
class TbResourceBase : public TbRefCounter
{
    friend class TbResourceManager;

public:

    // コンストラクタ
    TbResourceBase();

protected:

    // デストラクタ
    virtual ~TbResourceBase();

public:

    // 有効化
    void Load();

    // 無効化
    void Unload();

public:

    // タイプID取得
    TbTypeId GetTypeId() const {
        return m_TypeId;
    }

    // 使える状態か
    TbBool IsReady() const {
        return m_BitArray.Test(FLAG_READY);
    }

    // ビジー状態か
    TbBool IsBusy() const;

protected:

    // データ生成
    virtual TbResourceData* CreateData() = 0;

    // 基底クラス型でデータ取得
    TbResourceData* GetBaseData() {
        return m_Data;
    }

    // 基底クラス型でデータ取得
    const TbResourceData* GetBaseData() const {
        return m_Data;
    }

private:

    enum Flag {
        FLAG_READY ,               // 準備OK
        FLAG_ERROR ,               // エラー
        FLAG_ENABLE_READ_AUTO ,    // 自動でファイルを読み込む
        FLAG_ENABLE_RELEASE_FILE , // ファイルリソース解放しても良い
    };

private: // リソース管理クラスから呼び出される

    // 初期化
    TbResult initialize( const char*    fileName , 
                         TbUint32       listType ,
                         TbFileSyncMode syncMode , 
                         TbFilePathId   pathId , 
                         TbTypeId       typeId , 
                         TbResourceType type ); 

    // 更新
    void update();

private:

    TbBitArray32            m_BitArray;  //!< ビット配列
    TbUint32                m_Phase;     //!< フェーズ
    TbUint32                m_ListType;  //!< リストタイプ
    TbStaticString256       m_FileName;  //!< ファイルパス
    TbFileSyncMode          m_SyncMode;  //!< 同期モード
    TbResourceType          m_Type;      //!< タイプ
    TbTypeId                m_TypeId;    //!< タイプID
    TbFile                  m_File;      //!< ファイル
    TbResourceData*         m_Data;      //!< データ

};

// リソース
template<typename ResourceData>
class TbResource : public TbResourceBase
{
public:
    typedef ResourceData Data;
public:

    // 読み込んだデータ取得
    ResourceData* GetData() {
        return static_cast<ResourceData*>(GetBaseData());
    }

    // 読み込んだデータ取得
    const ResourceData* GetData() const {
        return static_cast<const ResourceData*>(GetBaseData());
    }

protected:

    // データ生成
    virtual TbResourceData* CreateData() {
        return TB_NEW ResourceData();
    };

};

}

#endif
