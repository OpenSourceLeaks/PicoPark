/*!
 * フレームワークのNX用コード
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_NX_H_
#define _INCLUDED_TB_FRAMEWORK_NX_H_

/*!
 * メイン関数
 * @author teco
 */
extern "C" void nnMain()
{
    using namespace toybox;
    nn::time::Initialize();

    static const TbUint32 CMD_ARG_MAX = 10;
    const char* argv[CMD_ARG_MAX];
    TbMemClear(argv);
    TbUint32 argc = 0;
    
    // ディスプレイは固定
    TbDisplayManagerDepend::Initialize( TB_GFX_WINDOW_WIDTH,
                                        TB_GFX_WINDOW_HEIGHT,
                                        TB_COLOR_FORMAT_A8B8G8R8,
                                        59.94f);

    // エントリー
    TbFramework::OnEntryPoint( argv , argc );

    // メインループ
    if( s_Impl ) {
        while( s_Impl->IsEnableMainLoop() ) 
        {
            TB_PROFILER_FRAME("ToyboxMainThread");

            s_Impl->Update();
            s_Impl->Draw();
            s_Impl->Post();
        }
    }

    // 終了
    TbFramework::OnExitPoint();

    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFramework::Finalize();
    }
    nn::time::Finalize();
}

#endif
