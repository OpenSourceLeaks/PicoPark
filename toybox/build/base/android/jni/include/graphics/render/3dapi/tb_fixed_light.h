/*!
 * ライトクラス
 * @author Miyake Shunsuke
 * @since 2011.06.16
 */

#ifndef _INCLUDED_TB_FIXED_LIGHT_H_
#define _INCLUDED_TB_FIXED_LIGHT_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/light/tb_light.h>
#include "./depend/tb_fixed_light_depend.h"

namespace toybox
{

/*!
 * 固定機能用ライト
 * @author Miyake Shunsuke
 * @since 2011.06.18
 */
class TbFixedLight
{
public:

    // コンストラクタ
    TbFixedLight();

    // デストラクタ
    ~TbFixedLight();

public:

     // 種類設定
    TbResult SetType( TbLightType type );
    
    // 種類取得
    const TbLightType& GetType() const { return m_Light.GetType(); }

public: // 共通設定
    
    // 色
    TbResult SetColor( const TbColor& color );
    const TbColor& GetColor() const { return m_Light.GetColor(); }
    
    // 減衰率設定
    
public: // 点光源設定
   
    // 位置設定
    TbResult SetPosition( const TbVector4& pos );
    const TbVector4& GetPosition() const { return m_Light.GetPosition(); }
    
    // 半径設定
    TbResult SetRadius( TbFloat32 radius );
    TbFloat32 GetRadius() const { return m_Light.GetRadius(); }
    
public: // 指向性光源設定
    
    // 方向設定
    TbResult SetDirection( const TbVector4& dir );
    const TbVector4& GetDirection() const { return m_Light.GetDirection(); }
    
public: // 依存部分

    TbFixedLightDepend& GetDepend() { return m_Depend; }
    const TbFixedLightDepend& GetDepend() const { return m_Depend; }

private:
    TbLight m_Light;
    TbFixedLightDepend m_Depend;
};

}

#endif
