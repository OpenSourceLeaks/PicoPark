/*!
 * リソース管理クラスのテンプレート関数定義
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

namespace toybox
{

/*!
 * リソース取得
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
template<typename Type>
Type* TbResourceManager::FindResource( const char* fileName , const TbUint32 listType )
{
    TbSint32 index = getResourceIndex( fileName , listType );
    if( index < 0 ){
        return nullptr;
    }
    TbResourceBase* resource = m_ResourceList[listType].list[index].resource;
    if( resource->GetTypeId() != TbGetTypeId<Type>() ){
        return nullptr;
    }
    return static_cast<Type*>(resource);
}

/*!
 * リソース生成
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
template<typename Type>
Type* TbResourceManager::CreateResource( const char* fileName , const TbUint32 listType , TbFileSyncMode syncMode )
{
    if( !TB_VERIFY( listType < m_ListCount ) ){
        return NULL;
    }
    Type* resource = FindResource<Type>( fileName , listType );
    if( resource ){
        resource->Retain();
    }else if( !m_ResourceList[listType].list.IsFull() ){
        resource = TB_NEW Type();
        if( TB_SUCCEEDED( resource->initialize( fileName , listType , syncMode , m_ResourceList[listType].pathId , TbGetTypeId<Type>() , TB_RESOURCE_TYPE_FILE ) ) )
        {
            ResourceInfo info;
            info.hash = TbCrc32::GetHash(reinterpret_cast<const TbUint8*>(fileName),TbStrCharCount(fileName));
            info.resource = resource;
            TbSint32 insert = TbLowerBoundIndex( m_ResourceList[listType].list.Begin() , 
                                                 m_ResourceList[listType].list.End() , 
                                                 info , 
                                                 less );
            m_ResourceList[listType].list.Insert( insert , info );
        }else{
            TB_ASSERT(!"failed to initialize");
            TB_SAFE_RELEASE(resource);
        }
    }
    return resource;
}

}
