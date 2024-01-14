/*!
 * フレームワークのWiiU用コード
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_WIIU_H_
#define _INCLUDED_TB_FRAMEWORK_WIIU_H_

namespace
{

u32 SaveCallback(void* unused)
{
    // Finish any saving before calling OSSavesDone_ReadyToRelease
    OSSavesDone_ReadyToRelease();
    return 0;
}
    
}


/*!
 * メイン関数
 * @author teco
 */
int main( int argc, char **argv  )
{
    ProcUIInitEx(&SaveCallback,nullptr);
    using namespace toybox;
    
    static const TbUint32 ARG_MAX = 10;
    const char* argv[ARG_MAX];
    TbMemClear(argv);
    TbUint32 argc = 0;

    TbDisplayManagerDepend::Initialize(TB_GX2_WINDOW_WIDTH,
                                       TB_GX2_WINDOW_HEIGHT,
                                       TB_COLOR_FORMAT_A8B8G8R8,
                                       59.94f);

    // エントリー
    TbFramework::OnEntryPoint( argv , argc );

    // メインループ
    if( s_Impl ) {
        while( s_Impl->IsEnableMainLoop() ) {
            s_Impl->Update();
            s_Impl->Draw();
            s_Impl->Post();
            
            ProcUIStatus status = ProcUIProcessMessages(TB_TRUE);
            if (status == PROCUI_STATUS_EXIT) {
                break;
            }else if (status == PROCUI_STATUS_RELEASING){
                // フォアグラウンドを開放
                ProcUIDrawDoneRelease();
            }
        }
    }

    // 終了
    TbFramework::OnExitPoint();

    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFramework::Finalize();
    }
    
    ProcUIShutdown();
    return 0;
}

#endif
