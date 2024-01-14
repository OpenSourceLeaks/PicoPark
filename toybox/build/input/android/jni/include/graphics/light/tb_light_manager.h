/*!
 * ライト管理クラス
 * @author Miyake_Shunsuke
 * @since 2014.04.29
 */

#ifndef _INCLUDED_TB_LIGHT_MANAGER_H_
#define _INCLUDED_TB_LIGHT_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <graphics/light/tb_light.h>

namespace toybox
{

class TbLightManager : public TbSingletonRef<TbLightManager>
{
public:
    
    static const TbUint32 LIGHT_MAX = 256;
    
public:
    
    // コンストラクタ
    TbLightManager() {}
    
    // デストラクタ
    ~TbLightManager() {}
    
public:
    
    // ライト取得
    TbLight* GetLight( TbUint32 index ) {
        if( index < TB_ARRAY_COUNT_OF(m_Light) ){
            return &m_Light[index];
        }
        return nullptr;
    }
    
    // ライト取得
    const TbLight* GetLight( TbUint32 index ) const {
        if( index < TB_ARRAY_COUNT_OF(m_Light) ){
            return &m_Light[index];
        }
        return nullptr;
    }
    
private:
    
    TbLight m_Light[LIGHT_MAX];
    
};

}

#endif
