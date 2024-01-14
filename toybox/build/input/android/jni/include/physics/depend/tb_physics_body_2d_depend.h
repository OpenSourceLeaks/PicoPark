/*!
 * 依存分岐
 */

#if TB_PHYSICSAPI_IS_BOX2D
    #include "./box2d/tb_physics_body_2d_box2d.h"
#else
    #error "unknown physicsapi"
#endif
