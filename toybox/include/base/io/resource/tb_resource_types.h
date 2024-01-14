/*!
 * リソース共通型
 * @author Miyake Shunsuke
 * @since 2012.03.31
 */

#ifndef _TB_RESOURCE_TYPES_H_
#define _TB_RESOURCE_TYPES_H_

enum TbResourceType
{
    TB_RESOURCE_TYPE_BEGIN ,                            
    TB_RESOURCE_TYPE_FILE = TB_RESOURCE_TYPE_BEGIN ,    // ファイル
    TB_RESOURCE_TYPE_ARCHIVE ,                          // アーカイブ内ファイル
    TB_RESOURCE_TYPE_END , 
    TB_RESOURCE_TYPE_MAX = TB_RESOURCE_TYPE_END - TB_RESOURCE_TYPE_BEGIN
};

#endif