/*!
 * 物理系
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_PHYSICS_OBJECT_H_
#define _INCLUDED_SIDE_ACTION_PHYSICS_OBJECT_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/render/3dapi/tb_color.h>
#include <physics/tb_physics_2d_all.h>

class SideActionPhysicsArea : public CrActionActor
{
public:
    typedef SideActionPhysicsArea Self;
    typedef CrActionActor Super;
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

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        TbFloat32 x = static_cast<TbFloat32>(userData.AsSint32(0));
        TbFloat32 y = static_cast<TbFloat32>(userData.AsSint32(1));
        TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(2));
        TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(3));
        SetArea(TbRectF32(x, y, w, h));
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
    CrActionActorPhysicsBody  m_Body;
    TbPhysicsShapeEdge2d    m_EdgeShape;
    TbRectF32               m_Rect;
};

class SideActionPhysicsRect : public CrActionActor
{
public:

    typedef SideActionPhysicsRect Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionPhysicsRect();

    // デストラクタ
    virtual ~SideActionPhysicsRect();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

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

    CrActionActorPhysicsBody  m_Body;
    TbPhysicsShapeEdge2d    m_EdgeShape;
    TbRectF32               m_Rect;

};

#endif
