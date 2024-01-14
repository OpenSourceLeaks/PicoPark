/*!
 * CUI用フレームワーク
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#ifndef _INCLUDED_TB_FRAMEWORK_CUI_H_
#define _INCLUDED_TB_FRAMEWORK_CUI_H_

#include "framework/tb_framework_types.h"

namespace toybox
{

class TbFrameworkCui
{
public:

    typedef TbFrameworkCui Self;

public:

    // デフォルトの初期化パラメータ取得
    static void GetDefaultInitParam( TbFrameworkCuiInitParam& param );

public: // アプリケーション側で実装

    // エントリーポイント
    static void OnEntryPoint( const TbChar8** argList , TbUint32 argCount );

    // 終了ポイント
    static void OnExitPoint();

public:

    // 初期化
    static TbResult Initialize();

    // 初期化(カスタム設定可能)
    static TbResult Initialize( const TbFrameworkCuiInitParam& param );

    // メインループ
    static TbResult StartMainLoop( void (*begin)() , void (*main)() , void (*end)() );

    // メインループ終了
    static TbResult FinishMainLoop();

    // 終了処理
    static TbResult Finalize();

private:

    TbFrameworkCui();
    ~TbFrameworkCui();

};

}

#endif