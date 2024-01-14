/*!
 * @file
 * @brief レンダービュークラス（ウィンドウハンドル別）
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_RENDER_VIEW_H_
#define _INCLUDED_TB_RENDER_VIEW_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_render_parameter.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbWindow;
class TbDepthStencil;
class TbRenderTarget;

//! 生成パラメータ
struct TbRenderViewParameter
{
    enum
    {
        FLAG_BIT_MULTIHEAD = TB_BIT(0), // マルチヘッドフラグ
    };
    TbSint32            flags;                      //!< フラグ
    TbWindow*           window;                     //!< 表示先ウィンドウ
    TbUint32            backBufferWidth;            //!< バックバッファの横幅
    TbUint32            backBufferHeight;           //!< バックバッファの縦幅
    TbColorFormat       backBufferFormat;           //!< バックバッファのフォーマット
    TbUint32            backBufferNum;              //!< バックバッファ数
    TbMultiSampleType   multiSampleType;            //!< マルチサンプル種類
    TbSint32            multiSampleQuality;         //!< マルチサンプルクオリティ
    TbBool              enableCreateDepthStencil;   //!< 深度ステンシル生成
    TbColorFormat       autoDepthStencilsFormat;    //!< 自動生成用深度ステンシル用カラーフォーマット
    TbBool              isFullscreen;               //!< フルスクリーン設定
};

}

#include "./depend/tb_render_view_depend.h"

namespace toybox
{

class TbRenderDevice;

/*!
 * @class
 */
class TbRenderView
{
    friend class TbRenderViewDepend;
private:

    // コンストラクタ
    TbRenderView( const TbRenderViewParameter& param , TbSint32 index = -1 );

    // デストラクタ
    ~TbRenderView();

public:

    // 画面に表示
    void Present();

    // エラー
    TbBool IsError() const { 
        return m_BitArray.Test( FLAG_ERROR ); 
    }

public:

    // 対応ウィンドウ取得
    TbWindow* GetWindow() { 
        return m_Param.window; 
    }
    
    // レンダーターゲット取得
    TbRenderTarget* GetRenderTarget( TbUint32 backBufferIndex ); 

    // 画面表示領域取得
    const TbRectS32& GetPresentRegion() const {
        return m_PresentRegion;
    }

    // 深度ステンシル取得
    TbDepthStencil* GetDepthStencil() { 
        return m_DepthStencil; 
    }

    const TbDepthStencil* GetDepthStencil() const { 
        return m_DepthStencil; 
    }

public:

    // フロントバッファの横幅取得
    TbUint32 GetFrontBufferWidth();

    // フロントバッファのピッチ幅取得
    TbUint32 GetFrontBufferPitch();

    // フロントバッファの縦幅取得
    TbUint32 GetFrontBufferHeight();

    // フロントバッファのフォーマット取得
    TbColorFormat GetFrontBufferFormat();
    
    // フロントバッファ読み込み
    TbResult ReadFrontBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect = nullptr , TbBool* isDownToUp = nullptr );

public:

    // バックバッファの横幅取得
    TbUint32 GetBackBufferWidth();

    // バックバッファのピッチ幅取得
    TbUint32 GetBackBufferPitch();

    // バックバッファの縦幅取得
    TbUint32 GetBackBufferHeight();

    // バックバッファのフォーマット取得
    TbColorFormat GetBackBufferFormat();
    
    // バックバッファ読み込み
    TbResult ReadBackBuffer( void* readBuffer , TbUint32 readBufferSize , const TbRectS32* readRect = nullptr , TbBool* isDownToUp = nullptr );

public:

    // 依存部分取得
    TbRenderViewDepend& GetDepend() { 
        return m_Depend; 
    } 

    // 依存部分取得
    const TbRenderViewDepend& GetDepend() const { 
        return m_Depend; 
    }

private:

    enum {
        FLAG_ERROR , 
    };

private:

    TbBitArray16          m_BitArray;             //!< フラグ
    TbRenderViewParameter m_Param;                //!< 生成パラメータ
    TbRenderViewDepend    m_Depend;               //!< 依存コード
    TbDepthStencil*       m_DepthStencil;         //!< 自動生成された深度ステンシル
    TbColorFormat         m_FrontBufferFormat;    //!< フロントバッファのフォーマット
    TbRectS32             m_PresentRegion;        //!< 表示領域

private: // 生成破棄はデバイスが行う

    friend class TbRenderDevice;

};

}

#endif
