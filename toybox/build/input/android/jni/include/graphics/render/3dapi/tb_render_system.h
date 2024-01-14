/*!
 * レンダーシステム
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_RENDER_SYSTEM_H_
#define _INCLUDED_TB_RENDER_SYSTEM_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>

#include "./depend/tb_render_system_depend.h"

namespace toybox
{

class TbRenderSystem : public TbSingletonRef<TbRenderSystem>
{
public:
    
    // コンストラクタ
    TbRenderSystem();
    
    // ですとr化歌
    ~TbRenderSystem();
    
public:
    
    // 初期化
    void Initialize();
    
    // 後始末
    void Finalize();
    
public: // 依存コードアクセス
    
    TbRenderSystemDepend& GetDepend() {
        return m_Depend;
    }
    
    const TbRenderSystemDepend& GetDepend() const {
        return m_Depend;
    }
    
public:

    TbRenderSystemDepend m_Depend;

};

}

#endif