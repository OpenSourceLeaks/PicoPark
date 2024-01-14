/*!
 * フレームワークのEmscripten用コード
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_EMSCRIPTEN_H_
#define _INCLUDED_TB_FRAMEWORK_EMSCRIPTEN_H_

void mainLoop() 
{
    using namespace toybox;

    if( s_Impl->IsEnableMainLoop() ) {
        s_Impl->Update();
        s_Impl->Draw();
        s_Impl->Post();
    }
}

/*!
 * メイン関数
 * @author teco
 */
int main()
{
    using namespace toybox;

    static const TbUint32 ARG_MAX = 10;
    const char* argv[ARG_MAX];
    TbMemClear(argv);
    TbUint32 argc = 0;

    TbDisplayManagerDepend::Initialize(640,480,TB_COLOR_FORMAT_R5G6B5,60.0f);

    // エントリー
    TbFramework::OnEntryPoint( argv , argc );

    // メインループ
    if( s_Impl ) {
        // ===================================================================================
        // 第二引数(fps)を0以外を入れてGL使うと、JSコンソールに以下のログが出るので値を0にする
        // ===================================================================================
        // Looks like you are rendering without using requestAnimationFrame for the main loop. 
        // You should use 0 for the frame rate in emscripten_set_main_loop in order to use requestAnimationFrame, 
        // as that can greatly improve your frame rates!
        // ===================================================================================
        emscripten_set_main_loop(mainLoop, 0, 1);
    }

#if 0
    // 終了
    TbFramework::OnExitPoint();

    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFramework::Finalize();
    }
#endif
    
    return 0;
}

#endif
