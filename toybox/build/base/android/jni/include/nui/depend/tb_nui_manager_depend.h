/*!
 * 依存分岐
 */

#if TB_NUIAPI_IS_KINECT
    #include "./kinect/tb_nui_manager_kinect.h"
#elif TB_NUIAPI_IS_KINECT2
    #include "./kinect2/tb_nui_manager_kinect2.h"
#else
    #error "unknown nuiapi"
#endif
