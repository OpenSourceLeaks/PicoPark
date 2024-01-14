/*!
 * リソース管理クラス
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#ifndef _INCLUDED_TB_RESOURCE_MANAGER_H_
#define _INCLUDED_TB_RESOURCE_MANAGER_H_

#include <base/io/resource/tb_resource.h>
#include <base/io/resource/tb_resource_data.h>
#include <base/io/resource/tb_resource_types.h>
#include <base/container/tb_array.h>
#include <base/container/tb_static_array.h>
#include <base/util/tb_singleton.h>
#include "base/algorithm/tb_search.h"
#include <base/crypt/tb_crc32.h>
#include <base/thread/tb_mutex.h>
#include <base/thread/tb_scoped_lock.h>
#include <base/thread/tb_wait_notify_thread.h>

namespace toybox
{

// リソースインデックス不正値
#define TB_RESOURCE_INDEX_INVALID (-1)

class TbResourceManager : public TbSingletonRef<TbResourceManager>
{
    typedef TbResourceManager Self;
    friend class TbResourceBase;
public:
    
    // 初期化パラメータ
    struct InitParam 
    {
        TbUint32 listTypeCount;
        TbUint32 resourceMaxPerList;
    };
    
    // 初期化パラメータ(拡張版)
    template<TbUint32 LIST_TYPE_COUNT>
    struct InitParamEx
    {
        TbUint32 resourceMaxPerList[LIST_TYPE_COUNT];
    };
    
public:

    //! コンストラクタ
    explicit TbResourceManager( const InitParam& param )
        : m_ListCount(param.listTypeCount)
        , m_ResourceList(NULL)
    {
        m_ResourceList = TB_NEW ResourceTypeInfo[m_ListCount];
        for( TbUint32 i = 0; i < m_ListCount; ++i ){
            m_ResourceList[i].list.SetCapacity(param.resourceMaxPerList);
        }
    }
    
    //! コンストラクタ
    template<TbUint32 LIST_TYPE_COUNT>
    TbResourceManager( const InitParamEx<LIST_TYPE_COUNT>& param )
        : m_ListCount(LIST_TYPE_COUNT)
        , m_ResourceList(NULL)
    {
        m_ResourceList = TB_NEW ResourceTypeInfo[m_ListCount];
        for( TbUint32 i = 0; i < m_ListCount; ++i ){
            m_ResourceList[i].list.SetCapacity(param.resourceMaxPerList[i]);
        }
    }

    // デストラクタ
    ~TbResourceManager();

public:
    
    // 更新
    void Update();

    // ファイルパスを設定
    TbResult SetFilePath( const TbUint32 listType , TbFilePathId pathId );

    // ファイルパスを設定
    TbFilePathId GetFilePath( const TbUint32 listType ) const;

    // リソース取得
    template<typename Type>
    Type* FindResource( const char* fileName , const TbUint32 listType = 0 );

    // リソース生成
    template<typename Type>
    Type* CreateResource( const char* fileName , const TbUint32 listType = 0 , TbFileSyncMode syncMode = TB_FILE_SYNC );

private:
    
    // リソース情報
    struct ResourceInfo {
        TbUint32 hash;
        TbResourceBase* resource;
    };

    // リソース種類別情報
    struct ResourceTypeInfo
    {
        ResourceTypeInfo() 
            : list()
            , pathId( TB_FILE_PATH_ID_NONE ) 
        {}
        
        ~ResourceTypeInfo()
        {}

        TbArray<ResourceInfo> list;
        TbFilePathId pathId;
    };

private:

    static TbBool less( const ResourceInfo& left , const ResourceInfo& right ){
        return left.hash < right.hash;
    }

private:

    // リソースセットアップ
    TbBool setupResource( TbResourceBase* resource ,  const char* fileName , const TbUint32 listType , TbResourceType resourceType );
    
    // インデックス取得
    TbSint32 getResourceIndex( const char* fileName , const TbUint32 listType ) const;
    
    // リソースを除去
    TbResult removeResource( const char* name , const TbUint32 listType );
    
private:

    TbUint32               m_ListCount;     // リスト数
    ResourceTypeInfo*      m_ResourceList;  // リソースリスト

};

// リソース取得
template<typename Type>
Type* TbFindResource( const char* fileName , const TbUint32 listType = 0 )
{
    return TbResourceManager::GetInstance().FindResource<Type>(fileName,listType);
}

}

#include "./tb_resource_manager_inl.h"

#endif
