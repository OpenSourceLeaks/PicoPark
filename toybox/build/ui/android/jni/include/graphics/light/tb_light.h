/*!
 * ライトクラス
 * @author Miyake Shunsuke
 * @since 2011.06.16
 */

#ifndef _INCLUDED_TB_LIGHT_H_
#define _INCLUDED_TB_LIGHT_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector4.h>

namespace toybox
{

// ライト種類
enum TbLightType
{
    TB_LIGHT_TYPE_POINT      , // 点光源
    TB_LIGHT_TYPE_DIRECTION  , // 指向性光源
    TB_LIGHT_TYPE_SPOT       , // スポットライト
    TB_LIGHT_TYPE_HEMISPHERE , // 半球ライト
    TB_LIGHT_TYPE_MAX
};

class TbLight
{
public: 

    // コンストラクタ
    TbLight();

    // デストラクタ
    ~TbLight();

public:
    
    // 種類設定
    TbResult SetType( TbLightType type );

    // 種類取得
    const TbLightType& GetType() const { return m_Type; }

public: // 共通設定
    
    // 色取得
    TbResult SetColor( const TbColor& color ) { 
        m_Color = color; return TB_S_OK; 
    }

    // 色取得
    const TbColor& GetColor() const { 
        return m_Color; 
    }
        
public: // 点光源設定
   
    // 位置設定
    TbResult SetPosition( const TbVector4& pos );
    const TbVector4& GetPosition() const { return m_Pos; }
    
    // 半径設定
    TbResult SetRadius( TbFloat32 radius );
    const TbFloat32& GetRadius() const { return m_Radius; }
    
public: // 指向性光源設定
    
    // 方向設定
    TbResult SetDirection( const TbVector4& dir );
    
    // 方向取得
    const TbVector4& GetDirection() const { return m_Direction; } 

private: // メンバ変数

    TbLightType m_Type;          // タイプ
    TbColor     m_Color;         // 色

    TbVector4   m_Pos;           // 位置
    TbFloat32   m_Radius;        // 半径

    TbVector4   m_Direction;     // 方向
    
};

}

#endif
