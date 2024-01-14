/*! 
 * NUI(Natural User Interface)のカラーバッファクラス
 * @author Miyake Shunsuke
 * @since 2011.07.21
 */

#ifndef _INCLUDED_TB_NUI_IMAGE_FRAME_H_
#define _INCLUDED_TB_NUI_IMAGE_FRAME_H_

#include <nui/tb_nui_types.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbNuiImageFrame
{
public:
    static const TbUint32 FRAME_MAX = 3; // トリプルバッファ
public:

    // コンストラクタ
    TbNuiImageFrame( TbNuiImageType type , TbNuiImageResolutionType resolutionType );

    // デストラクタ
    ~TbNuiImageFrame();

private: // コピー禁止

public:

    // ストリームを開く
    TbResult OpenStream();

    // ストリームを閉じる
    TbResult CloseStream();

    // 更新
    void Update();

public: // 情報取得

    // 準備できているか
    TbBool IsReady() const { 
        return m_BitArray.Test( FLAG_READY ); 
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
    TbUint32 GetBytePerPixel() const { 
        return m_BytePerPixel; 
    }

    // バッファをロックして取得
    void* LockBuffer();

    // バッファのロックを解除
    TbResult UnlockBuffer();

public:

    //ハンドル取得
    HANDLE GetStreamHandle() { return m_StreamHandle; }

    // 現在のイメージ
    const NUI_IMAGE_FRAME* GetCurrentImageFrame() const;

    // 現在のイメージインデックスへのオフセット
    TbResult SetCurrentImageOffset( TbUint32 offset ){
        if( TB_VERIFY(offset < FRAME_MAX) ){
            m_CurrentImageFrameIndexOffset = offset;
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

private:

    enum { 
        FLAG_READY , 
        FLAG_LOCK_BUFFER , 
        FLAG_MAX , 
    };

private:

    TbBitArray32             m_BitArray;        //!< ビット配列
    TbNuiImageType           m_Type;            //!< タイプ
    TbNuiImageResolutionType m_ResolutionType;  //!< 解像度タイプ
    TbDimensionS32           m_Resolution;      //!< 解像度
    TbUint32                 m_Pitch;           //!< ピッチ
    TbUint32                 m_BytePerPixel;   //!< ピクセル毎のバイト数
    const NUI_IMAGE_FRAME*   m_ImageFrame[ FRAME_MAX ];
    TbUint32                 m_CurrentImageFrameIndex;
    TbUint32                 m_CurrentImageFrameIndexOffset;
    HANDLE                   m_StreamHandle;

};

}

#endif
