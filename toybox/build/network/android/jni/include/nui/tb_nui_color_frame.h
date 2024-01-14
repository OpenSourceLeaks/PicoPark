/*! 
 * NUI(Natural User Interface)のカラーバッファクラス
 * @author Miyake Shunsuke
 * @since 2011.07.21
 */

#ifndef _INCLUDED_TB_NUI_COLOR_FRAME_H_
#define _INCLUDED_TB_NUI_COLOR_FRAME_H_

#include <nui/tb_nui_types.h>
#include <base/math/tb_geometry.h>
#include <graphics/render/3dapi/tb_color.h>

#include "./depend/tb_nui_color_frame_depend.h"

namespace toybox
{

class TbNuiColorFrame
{
public:

    // コンストラクタ
    explicit TbNuiColorFrame( TbNuiImageResolutionType resolutionType );

    // デストラクタ
    ~TbNuiColorFrame();

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

    // バッファをロックして取得
    TbResult Lock();

    // バッファのロックを解除
    TbResult Unlock();

    // カラー取得
    TbUint32 GetColor( TbUint32 x , TbUint32 y ) const;

public: // 依存部分

    TbNuiColorFrameDepend& GetDepend() { return m_Depend; }
    const TbNuiColorFrameDepend& GetDepend() const { return m_Depend; }

private:

    void*                       m_Buffer;       //!< バッファ
    TbUint32                    m_BufferSize;
    TbNuiImageResolutionType    m_ResolutionType;
    TbDimensionS32              m_Resolution;
    TbUint32                    m_Pitch;
    TbUint32                    m_BytePerPixel;
    TbNuiColorFrameDepend       m_Depend;

};

}

#endif
