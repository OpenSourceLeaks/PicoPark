/*!
 * 物理系
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_PHYSICS_OBJECT_H_
#define _INCLUDED_SIDE_ACTION_PHYSICS_OBJECT_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/render/3dapi/tb_color.h>
#include <physics/tb_physics_2d_all.h>

class SideActionPhysicsArea : public ActionActor
{
public:
    typedef SideActionPhysicsArea Self;
    typedef ActionActor Super;
public:
    // コンストラクタ
    SideActionPhysicsArea() {
        m_EdgeShape.SetDensity(1.0f);
        m_EdgeShape.SetFriction(1.0f);
        m_Body.GetBody().SetShape(&m_EdgeShape);
        m_Body.GetBody().SetLayer(SIDE_ACTION_PHYSICS_LAYER_AREA);
        AddComponent(&m_Body);
    }
    // デストラクタ
    virtual ~SideActionPhysicsArea() {
    }
public:
    // エリア範囲設定
    void SetArea( const TbRectF32& rect ) {
        m_Rect = rect;
        m_EdgeShape.SetRect( rect );
    }
public:
    // 描画
    virtual void OnDraw( TbFloat32 deltatime )
    {
#if 1
        toybox::TbDraw2dBegin();
        toybox::TbDrawLineRectParam param;
        param.pos = m_Body.GetBody().GetPos();
        param.SetRect( m_Rect );
        param.color = toybox::TbColor::Blue();
        toybox::TbDrawLine( param );
        toybox::TbDraw2dEnd();
#endif
    }
public:
    ActionActorPhysicsBody  m_Body;
    TbPhysicsShapeEdge2d    m_EdgeShape;
    TbRectF32               m_Rect;
};

class SideActionPhysicsRect : public ActionActor
{
public:

    typedef SideActionPhysicsRect Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionPhysicsRect();

    // デストラクタ
    virtual ~SideActionPhysicsRect();

public:

    // 矩形設定
    void SetRect( const TbRectF32& rect ) {
        m_Rect = rect;
        m_EdgeShape.SetRect( rect );
    }

public:

    // 描画
    virtual void OnDraw( TbFloat32 deltatime );

public:

    ActionActorPhysicsBody  m_Body;
    TbPhysicsShapeEdge2d    m_EdgeShape;
    TbRectF32               m_Rect;

};

#endif
