/*!
 * CUI用フレームワークのNX用コード
 * @author teco
 */

#ifdef _INCLUDED_TB_FRAMEWORK_CUI_IMPL_NX_H_
#   error "Invalid Include"
#else
#define _INCLUDED_TB_FRAMEWORK_CUI_IMPL_NX_H_

 /*!
 * メイン関数
 * @author teco
 */
extern "C" void nnMain()
{
    using namespace toybox;

    static const TbUint32 CMD_ARG_MAX = 10;
    const char* argv[CMD_ARG_MAX];
    TbMemClear(argv);
    TbUint32 argc = 0;

    // エントリー
    TbFrameworkCui::OnEntryPoint( argv , argc );

    // メインループ
    if( s_Impl ) {
        while( s_Impl->IsEnableMainLoop() ) {
            s_Impl->Update();
        }
    }

    // 終了
    TbFrameworkCui::OnExitPoint();

    // TbFramework::Finalize()のタイミングはユーザーで呼んで欲しいけど一応救済
    if( !TB_VERIFY(!s_Impl) ) {
        TbFrameworkCui::Finalize();
    }
}

#endif
