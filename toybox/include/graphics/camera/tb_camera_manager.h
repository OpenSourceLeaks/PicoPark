/*!
 * カメラ管理クラス
 * @author Miyake_Shunsuke
 * @since 2014.04.29
 */

#ifndef _INCLUDED_TB_CAMERA_MANAGER_H_
#define _INCLUDED_TB_CAMERA_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <graphics/camera/tb_camera.h>

namespace toybox
{

class TbCameraManager : public TbSingletonRef<TbCameraManager>
{
public:
    
    static const TbUint32 CAMERA_MAX = 256;
    
public:
    
    // コンストラクタ
    TbCameraManager() {}
    
    // デストラクタ
    ~TbCameraManager() {}
    
public:
    
    // カメラ取得
    TbCamera* GetCamera( TbUint32 index ) {
        if( index < TB_ARRAY_COUNT_OF(m_Camera) ){
            return &m_Camera[index];
        }
        return nullptr;
    }
    
    // カメラ取得
    const TbCamera* GetCamra( TbUint32 index ) const {
        if( index < TB_ARRAY_COUNT_OF(m_Camera) ){
            return &m_Camera[index];
        }
        return nullptr;
    }
    
private:
    
    TbCamera m_Camera[CAMERA_MAX];
    
};

}

#endif
