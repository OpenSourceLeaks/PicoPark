/*!
 * @file
 * @brief DX9用描画デバイスの依存クラス
 * @author Miyake Shunsuke
 * @since 2010.05.05
 */

#ifndef _INCLUDED_TB_RENDER_DEVICE_DX9_H_
#define _INCLUDED_TB_RENDER_DEVICE_DX9_H_

#include <graphics/render/3dapi/tb_render_parameter.h>
#include <graphics/render/3dapi/tb_color.h>
#include "./tb_lost_resource_dx9.h"

namespace toybox
{

typedef TbColorU32 TbClearColor;
class TbShader;
class TbVertexFormat;
class TbWindow;
struct TbRenderViewParameter;
class TbRenderDevice;

/*!
 * @brief DX9用描画デバイスの依存クラス
 */
class TbRenderDeviceDepend
{
public:

    // コンストラクタ
    TbRenderDeviceDepend();
    
    // デストラクタ
    ~TbRenderDeviceDepend();

public:

    // セットアップ
    TbBool Initialize( TbRenderDevice* device , TbUint32 adapterIndex , TbWindow* focusWindow , TbUint32 flags , const TbRenderViewParameter& parameter );

    // デバイスロスト通知
    void NotifyDeviceLost();

    // デバイスロスト状態か
    TbBool IsDeviceLost() { 
        return m_BitArray.Test(FLAG_DEVICELOST); 
    }

    //! デバイス取得
    TB_INLINE LPDIRECT3DDEVICE9 GetDeviceDX9() { 
        return m_DeviceDX9; 
    }

    //! デバイス取得
    TB_INLINE const LPDIRECT3DDEVICE9 GetDeviceDX9() const { 
        return m_DeviceDX9; 
    }

    //! Direct3D9オブジェクト取得
    LPDIRECT3D9 GetDirect3D9() {
        return m_Direct3D9;
    }

    //! Direct3D9オブジェクト取得
    const LPDIRECT3D9 GetDirect3D9() const {
        return m_Direct3D9;
    }

    //! 現在設定されてる頂点フォーマット取得
    inline const TbVertexFormat* GetCurrentVertexFormat() const { 
        return m_CurrentVertexFormat; 
    }

    //! 現在設定されてる頂点フォーマット設定
    void SetCurrentVertexFormat( const TbVertexFormat* format );

    //! 表面設定( 時計回りかどうか )
    void SetFrontFaceClockWise( TbBool isFrontFaceClockWise ) { 
        m_BitArray.SetBit( FLAG_FRONT_FACE_CW , isFrontFaceClockWise ); 
    }

    //! 表面は時計回りかどうか
    TbBool IsFrontFaceClockWise() const {
        return m_BitArray.Test( FLAG_FRONT_FACE_CW ); 
    }

    //! ロストリソースルート取得
    TbLostResourceDx9& GetLostResourceRoot(){ 
        return m_LostResourceRoot; 
    }

private:

    enum {
        FLAG_DEVICELOST , // デバイスロスト状態にあるか
        FLAG_FRONT_FACE_CW , 
    };

private:

    //! リソースを解放する
    void releaseResource();

    //1 リセット
    TbBool reset( TbBool isFullscreen );

    //! リソースを再構築
    void recoverResource();

    // フルスクリーン設定
    TbBool setFullscreen( TbBool isFullscreen );

private:

    LPDIRECT3D9             m_Direct3D9;            //!< Direct3D9へのインタフェース
    LPDIRECT3DDEVICE9       m_DeviceDX9;            //!< DirectX9デバイス
    D3DPRESENT_PARAMETERS   m_Param;                //!< 生成パラメータ
    D3DPRESENT_PARAMETERS   m_FullscreenParam;      //!< フルスクリーン用生成パラメータ
    TbBitArray32            m_BitArray;             //!< ビット配列
    const TbVertexFormat*   m_CurrentVertexFormat;  //!< 現在設定されてる頂点フォーマット設定
    TbLostResourceDx9       m_LostResourceRoot;     //!< ロストリソースルート(リングチェーンで繋がります)

private:

    friend class TbRenderDevice;

};

}

#endif