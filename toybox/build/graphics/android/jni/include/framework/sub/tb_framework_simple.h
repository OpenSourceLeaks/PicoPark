/*!
 * 1ウィンドウに入力系統を所持したシンプルなフレームワーク
 * @author Miyake Shunsuke
 * @since 2011.03.27
 */

#ifndef _INCLUDED_TB_FRAMEWORK_SIMPLE_H_
#define _INCLUDED_TB_FRAMEWORK_SIMPLE_H_

#include <framework/tb_framework_types.h>
#include <base/math/tb_geometry.h>
#include <graphics/render/3dapi/tb_3dapi.h>

namespace toybox
{

class TbFrameworkSimple
{
public:

    typedef TbFrameworkSimple Self;

    // レンダリングパス
    enum RenderPass 
    {
        RENDER_PASS_OFFSCREEN_1 , 
        RENDER_PASS_OFFSCREEN_2 , 
        RENDER_PASS_OFFSCREEN_3 , 
        RENDER_PASS_OFFSCREEN_4 , 
        RENDER_PASS_2D_BACK  , 
        RENDER_PASS_3D       , // 3D描画用
        RENDER_PASS_2D       , // 2D描画用
        RENDER_PASS_DEBUG_2D , // デバッグ2D用
        RENDER_PASS_MAX      , // 
        RENDER_PASS_ONSCREEN_BEGIN = RENDER_PASS_2D_BACK
    };

    // 初期値
    struct InitParam
    {
        enum Flag
        {
            FLAG_USE_MOUSE                    , 
            FLAG_USE_KEYBOARD                 ,
            FLAG_USE_PAD                      , 
            FLAG_USE_TOUCH                    , 
            FLAG_USE_LOG                      ,
            FLAG_FULLSCREEN                   , 
        };
        enum WindowSizeType
        {
            WINDOW_SIZE_DIRECT , 
            WINDOW_SIZE_DISPLAY_ASPECT_SCALE_WIDTH , 
        };
        typedef TbFrameworkInitParam::AllocParam AllocParam;
        static const TbSizeT ALLOC_TYPE_MAX = TbFrameworkInitParam::ALLOC_TYPE_MAX;
        
        AllocParam      allocParam[ALLOC_TYPE_MAX]; // アロケータパラメータ
        TbBool          disableRenderPass[RENDER_PASS_MAX];
        TbUint32        renderViewMax;              // 最大ビュー数
        TbUint32        taskLineMax;                // タスクライン数
        TbUint32        windowX;
        TbUint32        windowY;
        TbUint32        windowWidth;                // ウィンドウ横幅
        TbUint32        windowHeight;               // ウィンドウ高さ
        WindowSizeType  windowSizeType;             // ウィンドウサイズタイプ
        const char*     windowTitle;                // ウィンドウタイトル
        TbBitArray32    bitArray;                   // ビット配列
        const char*     projectName;                   // プロジェクト名
        const char*     rootPath;                   // ルートパス
        const char*     savePath;                   // セーブパス
    };

public:

    // デフォルトパラメータを設定
    static void GetDefaultInitParam( InitParam& param );

public:

    // 初期化(デフォルト設定)
    static TbResult Initialize();

    // 初期化
    static TbResult Initialize( const InitParam& param );

    // 終了処理
    static TbResult Finalize();

    // メインループ開始
    static TbResult StartMainLoop( void (*begin)() , 
                                   void (*main)() , 
                                   void (*draw)() , 
                                   void (*end)() ,
                                   void (*drawDirect)() = nullptr);

public:

    // 2D画面サイズ取得
    static TbDimensionS32 GetScreenSize();

    // クリアカラー設定
    static void     SetClearColor( const TbClearColor& color );

    // 画面に文字表示
    static void     PrintfScreen( TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... );

    // フェードカラー設定
    static void     SetFadeColor( const TbColorU32& color );

    // フェードカラー設定
    static TbColorU32     GetFadeColor();

    // フェードイン
    static void     FadeIn( TbFloat32 sec );

    // フェードアウト
    static void     FadeOut( TbFloat32 sec );

    // フェード中か
    static TbBool   IsFading();

private:

    // インスタンス作成禁止
    TbFrameworkSimple();
    ~TbFrameworkSimple();

};

}

#endif