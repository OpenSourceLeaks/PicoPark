/*!
* フレームワーク(Flash用)
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */

#include "tb_fwd.h"
#include "framework/tb_framework.h"
#include "graphics/3dapi/tb_render_device.h"
#include "graphics/display/tb_display_manager.h"
#include "./tb_framework_entry_flash.h"

namespace
{
    AS3::ui::flash::display::Stage s_Stage;
    AS3::ui::flash::display::Stage3D s_Stage3d;
}

namespace toybox
{

// 何もしないダミー関数(これを実装する.cppをリンクするために)
void TbDummyFunc()
{
}

}


/*!
 * If we fail to create the Context3D we display a warning
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */
static AS3::ui::var context3DError(void *arg, AS3::ui::var as3Args)
{
    AS3::ui::flash::text::TextFormat fmt = AS3::ui::flash::text::TextFormat::_new();
    fmt->size = AS3::ui::internal::new_int(24);
    fmt->align = AS3::ui::flash::text::TextFormatAlign::CENTER;

    AS3::ui::flash::text::TextField tf = AS3::ui::flash::text::TextField::_new();
    tf->defaultTextFormat = fmt;
    tf->width = s_Stage->stageWidth;
    tf->height = s_Stage->stageHeight;
    tf->multiline = true;
    tf->wordWrap = true;
    tf->text = 
        "\nUnable to create a Stage3D context. Usually this means you ran the swf "
        "directly in a web browser, use the HTML wrapper instead so the wmode "
        "gets set correctly to 'direct'.";
    s_Stage->addChild(tf);
    return AS3::ui::internal::_undefined;
}
    
/*!
 * After a Context3D is created this function will be called.
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */
static AS3::ui::var initContext3D(void *arg, AS3::ui::var as3Args)
{
    using namespace toybox;
    AS3::ui::flash::display3D::Context3D ctx3d = s_Stage3d->context3D;
    ctx3d->enableErrorChecking = true;
    TbRenderDeviceDepend::SetContext(ctx3d);
    
    // エントリー
    TbFramework::OnEntryPoint( NULL , 0 );

    s_Stage->addEventListener(AS3::ui::flash::events::Event::ENTER_FRAME, AS3::ui::Function::_new(enterFrame, NULL));
    return AS3::ui::internal::_undefined;
}

/*!
 * メイン関数
 * @author Miyake Shunsuke
 * @since 2013.04.29
 */
int main()
{
    using namespace toybox;
    // Setup the s_Stage
    s_Stage = AS3::ui::internal::get_Stage();
    s_Stage->scaleMode = AS3::ui::flash::display::StageScaleMode::NO_SCALE;
    s_Stage->align = AS3::ui::flash::display::StageAlign::TOP_LEFT;
    s_Stage->frameRate = 60;
    
    // 暫定：ディスプレイとStageのサイズは一致させておく
    TbDisplayManagerDepend::Initialize(s_Stage->stageWidth,s_Stage->stageHeight,TB_COLOR_FORMAT_UNKNOWN,60);

    // Ask for a Stage3D context to be created
    s_Stage3d = AS3::ui::var(AS3::ui::var(s_Stage->stage3Ds)[0]);
    TbRenderDeviceDepend::SetStage3d(s_Stage3d);
    s_Stage3d->addEventListener(AS3::ui::flash::events::Event::CONTEXT3D_CREATE, AS3::ui::Function::_new(initContext3D, NULL));
    s_Stage3d->addEventListener(AS3::ui::flash::events::ErrorEvent::ERROR, AS3::ui::Function::_new(context3DError, NULL));
    s_Stage3d->requestContext3D(AS3::ui::flash::display3D::Context3DRenderMode::AUTO,
                          AS3::ui::flash::display3D::Context3DProfile::BASELINE_CONSTRAINED);

#if 0 // どこですれば良い？
    // 終了
    TbFramework::OnExitPoint();
    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFramework::Finalize();
    }
#endif
    
    AS3_GoAsync();
}
