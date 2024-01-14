/*! 
 * NUI(Natural User Interface)のカラーバッファクラス
 * @author Miyake Shunsuke
 * @since 2011.07.21
 */

#ifndef _INCLUDED_TB_NUI_DEPTH_FRAME_H_
#define _INCLUDED_TB_NUI_DEPTH_FRAME_H_

#include <nui/tb_nui_types.h>
#include <base/math/tb_geometry.h>

#include "./depend/tb_nui_depth_frame_depend.h"

namespace toybox
{

class TbNuiDepthFrame
{
public:

    // コンストラクタ
    explicit TbNuiDepthFrame( TbNuiImageResolutionType resolutionType );

    // デストラクタ
    ~TbNuiDepthFrame();

public:

    // ストリームを開く
    TbResult OpenStream();

    // ストリームを閉じる
    TbResult CloseStream();

    // 更新
    void Update();

public: // 情報取得

    // 準備できているか
    TbBool IsReady() const;

    // 改造度タイプ取得
    TbNuiImageResolutionType GetResolutionType() const {
        return m_ResolutionType;
    }

    // 解像度取得
    const TbDimensionS32& GetResolution() const {
        return m_Resolution;
    }

    // ピッチ取得
    const TbUint32 GetPitch() const {
        return m_Pitch;
    }

    // ピクセル毎のバイト数
    TbUint32 GetBytesPerPixel() const {
        return m_BytePerPixel;
    }

public: // 情報取得

    // ロック
    TbResult Lock();

    // ロック解除
    TbResult Unlock();

    // バッファ取得
    void* GetBuffer() { return m_Buffer; }

    // 深度取得
    TbUint16 GetDepth( TbSint32 x , TbSint32 y , TbSint16* playerIndex = NULL );

private:

    void*                       m_Buffer;       //!< バッファ
    TbUint32                    m_BufferSize;
    TbNuiImageResolutionType    m_ResolutionType;
    TbDimensionS32              m_Resolution;
    TbUint32                    m_Pitch;
    TbUint32                    m_BytePerPixel;
    TbNuiDepthFrameDepend       m_Depend;

};

}

#endif
