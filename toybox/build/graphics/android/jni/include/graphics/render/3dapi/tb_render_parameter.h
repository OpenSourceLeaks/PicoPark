/*!
 * @file
 * @brief レンダリングエンジン用パラメータ
 *        Toyboxはレンダリングエンジンを動的には切り替えれない
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_RENDER_ENGINE_PARAMETER_H_
#define _INCLUDED_TB_RENDER_ENGINE_PARAMETER_H_

/*!
 * サーフェスフォーマット
 */
enum TbSurfaceFormat
{
    TB_SURFACE_FORMAT_NONE,     // 無効フォーマット

    TB_SURFACE_FORMAT_A8R8G8B8, // アルファ付き32bit ARGBピクセルフォーマット
    TB_SURFACE_FORMAT_X8R8G8B8, // 32bit RGBピクセルフォーマット

    TB_SURFACE_FORMAT_D16,      // 深度バッファ16bit
    TB_SURFACE_FORMAT_D32,      // 深度バッファ32bit
    TB_SURFACE_FORMAT_D24S8,    // 深度バッファ24bit ステンシルバッファ8bit
};

/*!
 * マルチサンプリングのレベル
 */
enum TbMultiSampleType
{
    TB_MULTISAMPLE_NONE,
    TB_MULTISAMPLE_NONMASKABLE, // qualityが1の時は最適なモードが選択されます
    TB_MULTISAMPLE_2_SAMPLES,
    TB_MULTISAMPLE_3_SAMPLES,
    TB_MULTISAMPLE_4_SAMPLES,
    TB_MULTISAMPLE_5_SAMPLES,
    TB_MULTISAMPLE_6_SAMPLES,
    TB_MULTISAMPLE_7_SAMPLES,
    TB_MULTISAMPLE_8_SAMPLES,
    TB_MULTISAMPLE_9_SAMPLES,
    TB_MULTISAMPLE_10_SAMPLES,
    TB_MULTISAMPLE_11_SAMPLES,
    TB_MULTISAMPLE_12_SAMPLES,
    TB_MULTISAMPLE_13_SAMPLES,
    TB_MULTISAMPLE_14_SAMPLES,
    TB_MULTISAMPLE_15_SAMPLES,
    TB_MULTISAMPLE_16_SAMPLES,
};

/*!
 * スワップバッファ方式
 */
enum TbSwapEffect
{
    TB_SWAPEFFECT_DISCARD,  // 
    TB_SWAPEFFECT_FLIP,     // スワップチェーンを複数持っててフロントバッファを切り替える
    TB_SWAPEFFECT_COPY,     // バックバッファをフロントバッファにコピーする
};

/*!
 * レンダリングエンジンへの初期化パラメータ
 */
struct TbRenderDeviceParameter
{
    TbUint32            backBufferWidth;        //!< バックバッファの横幅
    TbUint32            backBufferHeight;       //!< バックバッファの縦幅
    TbSurfaceFormat     backBufferFormat;      //!< バックバッファのフォーマット
    TbUint32            backBufferNum;          //!< バックバッファ数
    TbMultiSampleType   multiSampleType;        //!< マルチサンプル種類
    TbSint32            multiSampleQuality;     //!< マルチサンプルクオリティ
    TbSwapEffect        swapEffect;             //!< スワップバッファー方式
    TbBool              isFullscreen;           //!< フルスクリーン設定
    TbSurfaceFormat     depthStencilFormat;      //!< 深度ステンシルフォーマット
    TbSint32            flags;                  //!< フラグ
    TbUint32            fullscreenRefreshRate;  //!< フルスクリーンでのリフレッシュレート
    TbUint32            presentationInterval;   //!< スワップの書き換えタイミング
};

#endif
