/*!
 * リソース
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#ifndef _INCLUDED_TB_RESOURCE_BASE_H_
#define _INCLUDED_TB_RESOURCE_BASE_H_

#include <base/io/tb_file.h>
#include <base/io/resource/tb_resource_types.h>
#include <base/util/tb_type_id.h>
#include <base/util/tb_ref_counter.h>

namespace toybox
{

// リソースベース
class TbResourceData : public TbRefCounter
{
    friend class TbResourceManager;
public:

    // 自分自身
    typedef TbResourceData Self;

    enum Result
    {
        RESULT_COMPLETE , 
        RESULT_INCOMPLETE ,
        RESULT_ERROR ,
    };

protected:

    // コンストラクタ
    TbResourceData() {}

    // デストラクタ
    virtual ~TbResourceData() {}
    
public:

    // セットアップ (読み込みから完了済み)
    virtual Result Setup( const char* filePath , TbUint32 listType , TbFileSyncMode syncMode , const void* buf , TbUint32 bufSize ) = 0;

};

}

#endif
