/*!
 * 物理系の定義
 * @author Miyake Shunsuke
 * @since 2011.05.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_SCENE_2D_H_
#define _INCLUDED_TB_PHYSICS_SCENE_2D_H_

#include <physics/tb_physics_types.h>

#include <base/math/tb_vector2.h>

#include "./depend/tb_physics_scene_2d_depend.h"

namespace toybox
{

class TbPhysicsBody2d;

struct TbPhysicsRay2d
{
    TbVector2 p1;
    TbVector2 p2;
};

struct TbPhysicsRayCastResult
{
    TbPhysicsRayCastResult()
        : body(nullptr)
    {}
    TbVector2 pos;
    TbVector2 normal;
    TbPhysicsBody2d* body;
};

class TbPhysicsScene2d
{
public:

    // コンストラクタ
    TbPhysicsScene2d();

    // デストラクタ
    ~TbPhysicsScene2d();

private:

    // コピー代入禁止
    TB_DISALLOW_COPY_AND_ASSIGN(TbPhysicsScene2d);

public:

    // 重力設定
    void SetGravity( const TbVector2& gravity );

public:

    // 時間進行
    void Step( float deltaSec );

    // レイを飛ばす
    TbBool RayCast( const TbPhysicsRay2d& ray , TbPhysicsRayCastResult* result = nullptr );
    
public: // 依存コード触る先は責任を

    TbPhysicsScene2dDepend& GetDepend() {
        return m_Depend;
    }

    const TbPhysicsScene2dDepend& GetDepend() const {
        return m_Depend;
    }

private:

    TbPhysicsScene2dDepend  m_Depend;

};

}

#endif
