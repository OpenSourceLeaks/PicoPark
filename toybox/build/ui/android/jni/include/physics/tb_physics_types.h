/*!
 * 物理系定義
 * @author Miyake Shunsuke
 * @since 2015.03.06
 */
 
#ifndef _INCLUDED_TB_PHYSICS_TYPES_H_
#define _INCLUDED_TB_PHYSICS_TYPES_H_

// 物理系API一覧
#define TB_PHYSICSAPI_BOX2D 1

// 現在ビルド対象のAPI
#define TB_PHYSICSAPI_TARGET TB_PHYSICSAPI_BOX2D

#define TB_PHYSICSAPI_IS_BOX2D (TB_PHYSICSAPI_TARGET==TB_PHYSICSAPI_BOX2D)

#if TB_PHYSICSAPI_IS_BOX2D
#include <Box2D/Box2D.h>
#define TB_PHYSICS_BOX2D_SCALE (0.01f)
#endif

#endif
