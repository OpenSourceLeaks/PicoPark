/*!
 * 依存分岐
 */

#if TB_NUIAPI_IS_KINECT
    #include "./kinect/tb_nui_image_frame.h"
    typedef toybox::TbNuiImageFrame TbNuiColorFrameDepend;
#elif TB_NUIAPI_IS_KINECT2
    #include "./kinect2/tb_nui_color_frame_kinect2.h"
#else
    #error "unknown nuiapi"
#endif
