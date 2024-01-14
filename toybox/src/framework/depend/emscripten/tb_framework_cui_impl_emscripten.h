/*!
 * フレームワークのEmscripten用コード
 * @author teco
 */

#ifdef _INCLUDED_TB_FRAMEWORK_CUI_IMPL_EMSCRIPTEN_H_
#   error "Invalid Include"
#else
#define _INCLUDED_TB_FRAMEWORK_CUI_IMPL_EMSCRIPTEN_H_

void mainLoop() 
{
    using namespace toybox;

    if( s_Impl->IsEnableMainLoop() ) {
        s_Impl->Update();
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

    // エントリー
    TbFrameworkCui::OnEntryPoint( argv , argc );

    // メインループ
    if( s_Impl ) {
        emscripten_set_main_loop(mainLoop, 60, 1);
    }

    // 終了
    TbFrameworkCui::OnExitPoint();

    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFrameworkCui::Finalize();
    }
    
    return 0;
}

#endif
