/*!
 * VMDモーション用リソース
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_RESOURCE_MOTION_VMD_H_
#define _INCLUDED_TB_RESOURCE_MOTION_VMD_H_

#include <base/io/resource/tb_resource_data.h>
#include <base/io/resource/tb_resource_handle.h>
#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/motion/tb_motion_data_vmd.h>

namespace toybox
{

class TbResourceDataMotionVmd : public TbResourceData
{
public:

    // コンストラクタ
    TbResourceDataMotionVmd() {}

    // デストラクタ
    virtual ~TbResourceDataMotionVmd() {}

public:

    // セットアップ
    // セットアップ
    virtual Result Setup( const char* filePath , 
                          TbUint32 listType , 
                          TbFileSyncMode syncMode , 
                          const void* buf , 
                          TbUint32 bufSize )
    {
        m_Data.Load(buf,bufSize);
        return RESULT_COMPLETE;
    }

public:

    // モーションデータ取得
    const TbMotionDataVmd& GetMotion() const {
        return m_Data;
    }

private:
    TbMotionDataVmd m_Data;
};

typedef TbResource<TbResourceDataMotionVmd> TbResourceMotionVmd;
class TbResourceHandleMotionVmd : public TbResourceHandle<TbResourceDataMotionVmd>
{
public:
    // モーションデータ取得
    const TbMotionDataVmd* GetMotion() const {
        const TbResourceDataMotionVmd* data = GetData();
        if( data ) {
            return &data->GetMotion();
        }
        return nullptr;
    }
};

}

#endif
