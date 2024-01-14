/*!
 * 骨格ビューワ
 * @author Miyake Shunsuke
 * @since 2011.08.14
 */

#ifndef _INCLUDED_SKELETON_VIEWER_H_
#define _INCLUDED_SKELETON_VIEWER_H_

#include <nui/tb_nui_types.h>

class SkeletonViewer
{
    typedef SkeletonViewer Self;
public:
    enum ExportFileFormat {
        EXPORT_FILE_FORMAT_BMP , 
        EXPORT_FILE_FORMAT_TGA , 
        EXPORT_FILE_FORMAT_MAX , 
    };
public:
    // コンストラクタ
    SkeletonViewer();
    // デストラクタ
    ~SkeletonViewer();
public:
    // 更新
    void Update();
    // カラー用テクスチャファイル出力
    TbResult ExportColorImage( ExportFileFormat format );
    // プレイヤースケルトンインデックス取得
    TbSint32 GetPlayerSkeletonIndex() const { 
        return m_PlayerSkeletonIndex; 
    }
    // プレイヤーカラーマスク可能設定
    void SetEnableMaskPlayerColor( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_MASK_COLOR , isEnable ); 
    }
    // プレイヤーカラーマスク可能取得
    TbBool IsEnableMaskPlayerColor() const { 
        return m_BitArray.Test( FLAG_MASK_COLOR ); 
    }
    // プレイヤー深度マスク可能設定
    void SetEnableMaskPlayerDepth( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_MASK_DEPTH , isEnable ); 
    }
    // プレイヤー深度マスク可能取得
    TbBool IsEnableMaskPlayerDepth() const { 
        return m_BitArray.Test( FLAG_MASK_DEPTH ); 
    }
    // カラー用テクスチャ取得
    toybox::TbTexture* GetColorTexture() { 
        return m_ColorTexture; 
    }
    // 深度用テクスチャ取得
    toybox::TbTexture* GetDepthTexture() { 
        return m_DepthTexture; 
    }
    // 骨格用頂点バッファ取得
    toybox::TbVertexBuffer* GetSkeletonVertexBuffer( TbUint32* stride );
    // 骨格用頂点フォーマット取得
    toybox::TbVertexFormat* GetSkeletonVertexFormat() { 
        return m_SkeletonVertexFormat; 
    }
private:

    enum {
        FLAG_MASK_COLOR , // カラーマスク
        FLAG_MASK_DEPTH , // 深度マスク
    };

    struct VertexType
    {
        TbFloat32 x , y , z;
        DWORD color;//toybox::TbColor color;
    };

    static const TbSint32 VERTEX_COUNT = 38;

private:

    // プレイヤーのスケルトンインデックスを更新
    void updatePlayerSkeletonIndex();

    // カラーテクスチャ更新
    void updateColorTexture();

    // 深度テクスチャ更新
    void updateDepthTexture();

    // 骨格の頂点バッファ更新
    void updateSkeletonVertexBuffer();

    // 骨格位置座標を頂点バッファに書き込み
    void writePlayerSkeletonPosToVertexBuffer( TbSint32 index , toybox::TbNuiSkeletonPos pos1 , toybox::TbNuiSkeletonPos pos2 );

private:
    TbUint32                m_Phase;
    toybox::TbBitArray32    m_BitArray;               // ビット配列
    toybox::TbNuiManager*   m_NuiManager;             // NUI管理クラス
    TbUint32                m_PlayerSkeletonIndex;    // プレイやースケルトンインデックス
    toybox::TbVertexBuffer* m_SkeletonVertexBuffer;   // 骨格用頂点バッファ
    toybox::TbVertexFormat* m_SkeletonVertexFormat;   // 骨格用頂点フォーマット
    VertexType              m_SkeletonVertex[VERTEX_COUNT];
    toybox::TbTexture*      m_ColorTexture;           // カラーテクスチャ
    toybox::TbTexture*      m_DepthTexture;           // 深度テクスチャ
};

#endif
