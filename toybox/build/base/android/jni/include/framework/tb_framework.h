/*!
 * アプリケーションを作りやすくするためのフレームワーク
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#ifndef _INCLUDED_TB_FRAMEWORK_H_
#define _INCLUDED_TB_FRAMEWORK_H_

#include <framework/tb_framework_types.h>

namespace toybox
{

class TbFramework
{
public:

    typedef TbFramework Self;

public: // アプリケーション側で実装

    // エントリーポイント
    static void OnEntryPoint( const TbChar8** argList , TbUint32 argCount );

    // 終了ポイント
    static void OnExitPoint();

public: // アプリケーション側で実装   (プラットフォームによっては呼ばれない)

    // 一時停止によって呼ばれる 
    static void OnPause();

    // 復帰ポイント(プラットフォームによっては呼ばれない)
    static void OnResume();

    // 画面リサイズ時
    static void OnResizeDisplay( TbSint32 width , TbSint32 height );

public:

    // デフォルトの初期化パラメータ取得
    static void GetDefaultInitParam( TbFrameworkInitParam& param );

    // 初期化(デフォルト設定)
    static TbResult Initialize();

    // 初期化(カスタム設定可能)
    static TbResult Initialize( const TbFrameworkInitParam& param );

    // 初期化(カスタム設定を外部ファイルから指定)
    static TbResult Initialize( const char* initFilename );
    
    // 初期化済みか
    static TbBool IsInitialized();

    // メインループ開始
    static TbResult StartMainLoop( void (*begin)() ,
                                   void (*main)() , 
                                   void (*draw)() , 
                                   void (*end)() ,
                                   void (*drawDirect)() = nullptr );

    // メインループ終了
    static TbResult FinishMainLoop();

    // 終了処理
    static TbResult Finalize();

private:

    TbFramework();
    ~TbFramework();

};

}

#endif