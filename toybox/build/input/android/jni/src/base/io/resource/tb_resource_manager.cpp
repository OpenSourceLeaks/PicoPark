/*!
 * リソース管理クラス
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#include "tb_fwd.h"
#include "base/string/tb_string_util.h"
#include "base/io/resource/tb_resource_manager.h"

namespace toybox
{

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
TbResourceManager::~TbResourceManager()
{
    // 
    TbResourceBase* resource = NULL;
    for( TbUint32 i = 0; i < m_ListCount; ++i ){
        TbArray<ResourceInfo>::Iterator ite = m_ResourceList[i].list.Begin();
        TbArray<ResourceInfo>::Iterator endIte = m_ResourceList[i].list.End();
        for( ; ite != endIte; ++ite ){
            resource = (*ite).resource;
            // 本来ここに来る前に開放されているべき
            TB_ASSERT(!resource);
            while( !resource->Release() ){}
        }
        m_ResourceList[i].list.Clear();
    }
    TB_SAFE_DELETE_ARRAY(m_ResourceList);
}

/*!
 * ファイルパスを設定
 * @param listType リストタイプ
 * @param pathId パスID
 * @author Miyake Shunsuke
 * @since 2011.12.31
 */
TbResult TbResourceManager::SetFilePath( const TbUint32 listType , TbFilePathId pathId )
{
    if( listType >= m_ListCount ){
        return TB_E_FAIL;
    }
    m_ResourceList[listType].pathId = pathId;
    return TB_S_OK;
}

/*!
 * ファイルパスを設定
 * @author Miyake_Shunsuke
 * @since 2012.04.13
 */
TbFilePathId TbResourceManager::GetFilePath( const TbUint32 listType ) const
{
    if( listType >= m_ListCount ){
        return TB_FILE_PATH_ID_NONE;
    }
    return m_ResourceList[listType].pathId;
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
void TbResourceManager::Update()
{
    for( TbUint32 i = 0; i < m_ListCount; ++i ){
        TbArray<ResourceInfo>::Iterator ite = m_ResourceList[i].list.Begin();
        TbArray<ResourceInfo>::Iterator endIte = m_ResourceList[i].list.End();
        for( ; ite != endIte; ++ite ){
            (*ite).resource->update();
        }
    }
}

/*!
 * リソースのIndex番号取得
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
TbSint32 TbResourceManager::getResourceIndex( const char* fileName , const TbUint32 listType ) const
{
    if( listType >= m_ListCount ){
        return -1;
    }
    const TbUint32 hash = TbCrc32::GetHash(reinterpret_cast<const TbUint8*>(fileName),TbStrCharCount(fileName));
    ResourceInfo info = { hash , NULL };
    const TbSint32 result = TbBinarySearchIndex(m_ResourceList[listType].list.Begin(),
                                                m_ResourceList[listType].list.End(),
                                                info,
                                                &Self::less );
    return result;
}

/*!
 * リソースを除去
 * @author Miyake Shunsuke
 * @since 2011.12.21
 */
TbResult TbResourceManager::removeResource( const char* name , const TbUint32 listType )
{
    TbSint32 index = getResourceIndex(name,listType);
    if( index >= 0 ){
        m_ResourceList[listType].list.Erase(index);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

}