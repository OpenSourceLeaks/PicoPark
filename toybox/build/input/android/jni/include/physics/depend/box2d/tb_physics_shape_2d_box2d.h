/*!
 * 2D形状の依存型
 * @author teco
 */
 
#ifndef _INCLUDED_TB_PHYSICS_SHAPE_2D_BOX2D_H_
#define _INCLUDED_TB_PHYSICS_SHAPE_2D_BOX2D_H_

namespace toybox
{

struct TbPhysicsShapeCircle2dDepend
{
    b2CircleShape   circle;
};

struct TbPhysicsShapePolygon2dDepend
{
    b2PolygonShape  polygon;
};

struct TbPhysicsShapeEdge2dDepend
{
};

}

#endif
