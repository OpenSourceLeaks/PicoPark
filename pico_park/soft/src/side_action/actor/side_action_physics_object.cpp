/*!
 * 物理系
 * @author teco
 */

#include "fwd.h"
#include "side_action_physics_object.h"

////////////////////////////////////
//  SideActionPhysicsRect
////////////////////////////////////

// コンストラクタ
SideActionPhysicsRect::SideActionPhysicsRect() 
{
    m_EdgeShape.SetDensity(1.0f);
    m_EdgeShape.SetFriction(1.0f);
    m_Body.GetBody().SetShape(&m_EdgeShape);
    m_Body.GetBody().SetLayer(SIDE_ACTION_PHYSICS_LAYER_NORMAL);
    AddComponent(&m_Body);
}

// デストラクタ
SideActionPhysicsRect::~SideActionPhysicsRect()
{
}

// セットアップ
void SideActionPhysicsRect::Setup(const CrActorFactory::UserData& userData)
{
    TbFloat32 x = static_cast<TbFloat32>(userData.AsSint32(0));
    TbFloat32 y = static_cast<TbFloat32>(userData.AsSint32(1));
    TbFloat32 w = static_cast<TbFloat32>(userData.AsSint32(2));
    TbFloat32 h = static_cast<TbFloat32>(userData.AsSint32(3));
    SetRect(TbRectF32(x, y, w, h));
}

// 描画
void SideActionPhysicsRect::OnDraw( TbFloat32 deltatime )
{
    toybox::TbDraw2dBegin();
    toybox::TbDrawLineRectParam param;
    param.pos = m_Body.GetBody().GetPos();
    param.SetRect( m_Rect );
    param.color = toybox::TbColor::Blue();
    toybox::TbDrawLine( param );
    toybox::TbDraw2dEnd();
}
