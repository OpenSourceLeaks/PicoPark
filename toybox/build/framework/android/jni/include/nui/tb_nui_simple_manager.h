/*!
 * NUIのシンプル管理クラス
 * @author teco
 */

#ifndef _INCLUDED_TB_NUI_SIMPLE_MANAGER_H_
#define _INCLUDED_TB_NUI_SIMPLE_MANAGER_H_

#include <nui/tb_nui_types.h>
#include <nui/tb_nui_manager.h>
#include <base/util/tb_singleton.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_vector4.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/render/3dapi/tb_vertex_buffer.h>
#include <graphics/render/3dapi/tb_vertex_format.h>

namespace toybox
{

class TbNuiSimpleManager : public TbSingletonRef<TbNuiSimpleManager>
{
    typedef TbNuiSimpleManager Self;

public:

    enum ExportFileFormat {
        EXPORT_FILE_FORMAT_BMP , 
        EXPORT_FILE_FORMAT_TGA , 
        EXPORT_FILE_FORMAT_MAX , 
    };

public:
    
    // コンストラクタ
    TbNuiSimpleManager( TbUint32 playerMax );

    // デストラクタ
    virtual ~TbNuiSimpleManager();

public:

    // 更新
    void Update();

public:

    // 明るさ設定
    void SetBright( TbSint32 bright ) {
        m_Bright = bright;
    }

    // 明るさ取得
    TbSint32 GetBright() const {
        return m_Bright;
    }

public:

    // カラー用テクスチャ取得
    toybox::TbTexture* GetColorTexture( TbSizeT historyIndex = 0 ) { 
        TbSizeT index = (m_ColorTextureFrontIndex + historyIndex)%TB_ARRAY_COUNT_OF(m_ColorTexture);
        return m_ColorTexture[index];
    }

    // カラー用UV取得
    TbRectF32 GetColorUV() const {
        return m_ColorUV;
    }

    // 深度用テクスチャ取得
    toybox::TbTexture* GetDepthTexture() { 
        return m_DepthTexture; 
    }

    // 深度用UV取得
    TbRectF32 GetDepthUV() const {
        return m_DepthUV;
    }

    // 骨格用頂点バッファ取得
    toybox::TbVertexBuffer* GetSkeletonVertexBuffer( TbUint32 playerIndex , TB_OUT TbUint32* stride );

    // 骨格用頂点バッファを更新するか
    void SetEnableUpdateSkeletonVertexBuffer( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_SKELETON_VERTEX,isEnable);
    }

    // 骨格用頂点バッファが更新可能か
    TbBool IsEnableUpdateSkeletonVertexBuffer() const {
        return m_BitArray.Test(FLAG_ENABLE_SKELETON_VERTEX);
    }

    // 骨格用頂点フォーマット取得
    const toybox::TbVertexFormat* GetSkeletonVertexFormat() { 
        return &TbVertexFormat::XYZ_COLOR; 
    }

public:

    // プレイヤー数取得
    TbUint32 GetPlayerSkeletonCount() const { 
        return m_PlayerSkeletonCount; 
    }

    // 最大プレイヤースケルトン数取得
    TbUint32 GetPlayerSkeletonMax() const { 
        return m_PlayerSkeletonMax; 
    }

    // 最大プレイヤースケルトン数設定
    void SetPlayerSkeletonMax( TbUint32 max );

    // プレイヤースケルトンインデックス取得
    TbSint32 GetPlayerSkeletonIndex( TbUint32 playerIndex ) const;

    // プレイヤーをトラッキングしている
    TbBool IsTrackingPlayer( TbUint32 playerIndex ) const {
        return playerIndex < m_PlayerSkeletonCount;
    }

    // カラーバッファにマッピングされた骨格座標取得
    const toybox::TbVector4* GetPlayerSkeletonColorPos( TbUint32 playerIndex ) const;

    // カラーテクスチャ更新可能
    void SetEnableUpdateColorTexture( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_COLOR_TEXTURE,isEnable);
    }

    // カラーテクスチャ更新可能
    TbBool IsEnableUpdateColorTexture() const {
        return m_BitArray.Test(FLAG_ENABLE_COLOR_TEXTURE);
    }

    // 深度テクスチャ更新可能
    void SetEnableUpdateDepthTexture( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_DEPTH_TEXTURE,isEnable);
    }

    // 深度テクスチャ更新可能
    TbBool IsEnableUpdateDepthTexture() const {
        return m_BitArray.Test(FLAG_ENABLE_DEPTH_TEXTURE);
    }


    // カラーテクスチャにマスク可能設定
    void SetEnableColorPlayerMask( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_COLOR_MASK , isEnable ); 
    }

    // カラーテクスチャにプレイヤーマスク可能取得
    TbBool IsEnableColorPlayerMask() const { 
        return m_BitArray.Test( FLAG_COLOR_MASK ); 
    }

    // 深度テクスチャにプレイヤーマスク可能設定
    void SetEnableDepthPlayerMask( TbBool isEnable ) { 
        m_BitArray.SetBit( FLAG_DEPTH_MASK , isEnable ); 
    }

    // 深度テクスチャにプレイヤーマスク可能取得
    TbBool IsEnableDepthPlayerMask() const { 
        return m_BitArray.Test( FLAG_DEPTH_MASK ); 
    }

    // 深度用マスクカラー設定
    void SetDepthMaskColor( const TbColorU32& color ) {
        m_DepthMaskColor = color;
    }

    // 深度マスクカラー有効設定
    void SetEnableDepthMaskColor( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEPTH_MASK_COLOR , isEnable ); 
    }

    // 深度マスクカラー有効設定
    TbBool IsEnableDepthMaskColor() const {
        return m_BitArray.Test(FLAG_ENABLE_DEPTH_MASK_COLOR);
    }

    // プレイヤー別マスク可能設定
    void SetEnablePlayerMask( TbUint32 playerIndex , TbBool isEnable );

    // プレイヤー別マスク可能設定
    TbBool IsEnablePlayerMask( TbUint32 playerIndex );

public: // マスク補正

    // マスク補正デバッグ可能か
    void SetEnableDebugMaskCorrect( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEBUG_CORRECT_MASK, isEnable ); 
    }

    // マスク補正デバッグ可能か
    TbBool IsEnableDebugMaskCorrect() const { 
        return m_BitArray.Test( FLAG_ENABLE_DEBUG_CORRECT_MASK ); 
    }

    // マスク補正パラメータ
    void SetMaskCorrectLeftLoopX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftLoop.x = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectLeftLoopX() const {
        return m_PlayerMaskCorrectParam.leftLoop.x;
    }

    // マスク補正パラメータ
    void SetMaskCorrectLeftLoopY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftLoop.y = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectLeftLoopY() const {
        return m_PlayerMaskCorrectParam.leftLoop.y;
    }

    // マスク補正パラメータ
    void SetMaskCorrectLeftUnitX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftUnit.x = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectLeftUnitX() const {
        return m_PlayerMaskCorrectParam.leftUnit.x;
    }

    // マスク補正パラメータ
    void SetMaskCorrectLeftUnitY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.leftUnit.y = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectLeftUnitY() const {
        return m_PlayerMaskCorrectParam.leftUnit.y;
    }

    // マスク補正パラメータ
    void SetMaskCorrectRightLoopX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightLoop.x = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectRightLoopX() const {
        return m_PlayerMaskCorrectParam.rightLoop.x;
    }

    // マスク補正パラメータ
    void SetMaskCorrectRightLoopY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightLoop.y = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectRightLoopY() const {
        return m_PlayerMaskCorrectParam.rightLoop.y;
    }

    // マスク補正パラメータ
    void SetMaskCorrectRightUnitX( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightUnit.x = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectRightUnitX() const {
        return m_PlayerMaskCorrectParam.rightUnit.x;
    }

    // マスク補正パラメータ
    void SetMaskCorrectRightUnitY( TbSint32 value ) {
        m_PlayerMaskCorrectParam.rightUnit.y = value;
    }

    // マスク補正パラメータ
    TbSint32 GetMaskCorrectRightUnitY() const {
        return m_PlayerMaskCorrectParam.rightUnit.y;
    }

    // マスク補正パラメータ
    void SetMaskCorrectBeginRatio( TbFloat32 ratio ) {
        m_PlayerMaskCorrectParam.beginRatio = ratio;
    }

    // マスク補正パラメータ
    TbFloat32 GetMaskCorrectBeginRatio() const {
        return m_PlayerMaskCorrectParam.beginRatio;
    }

    // マスク補正パラメータ
    void SetMaskCorrectCheckYBeginRatio( TbFloat32 ratio ) {
        m_PlayerMaskCorrectParam.checkYBeginRatio = ratio;
    }

    // マスク補正パラメータ
    TbFloat32 GetMaskCorrectCheckYBeginRatio() const {
        return m_PlayerMaskCorrectParam.checkYBeginRatio;
    }

    // マスク補正パラメータ
    void SetMaskCorrectErrorOffset( TbFloat32 offset ) {
        m_PlayerMaskCorrectParam.errorOffset = offset;
    }

    // マスク補正パラメータ
    TbFloat32 GetMaskCorrectErrorOffset() const {
        return m_PlayerMaskCorrectParam.errorOffset;
    }

public: // デバッグ

    // カラーと深度をマッピングできない領域デバッグ有効
    void SetEnableDebugUnmapRegion( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_DEBUG_UNMAP_REGION, isEnable ); 
    }

    // カラーと深度をマッピングできない領域デバッグ有効
    TbBool IsEnableDebugUnmapRegion() const { 
        return m_BitArray.Test( FLAG_ENABLE_DEBUG_UNMAP_REGION ); 
    }

    // カラーテクスチャへのマスクの補正を有効
    void SetEnableMaskCorrect( TbBool isEnable ) {
        m_BitArray.SetBit( FLAG_ENABLE_CORRECT_MASK, isEnable ); 
    }

    // カラーテクスチャへのマスクの補正を有効
    TbBool IsEnableMaskCorrect() const { 
        return m_BitArray.Test( FLAG_ENABLE_CORRECT_MASK ); 
    }

public:

    // カラー用テクスチャファイル出力
    TbResult ExportColorImage( ExportFileFormat format );

private:

    enum {
        FLAG_ENABLE_COLOR_TEXTURE            , // カラーテクスチャ利用
        FLAG_ENABLE_DEPTH_TEXTURE            , // 深度ーテクスチャ利用

        FLAG_COLOR_MASK                      , // カラーマスク
        FLAG_DEPTH_MASK                      , // 深度マスク

        FLAG_ENABLE_CORRECT_MASK             , // マスク補正
        FLAG_ENABLE_DEBUG_CORRECT_MASK       , // マスク補正デバッグ描画
        FLAG_ENABLE_DEBUG_UNMAP_REGION       , // 深度とカラーでマッピングできない領域

        FLAG_ENABLE_DEPTH_MASK_COLOR         , // 深度へのマスクカラー有効

        FLAG_VERTEX_TO_DEPTH                 , // 頂点は深度バッファにマッピングする (デフォルトはカラー)
        FLAG_ENABLE_SKELETON_VERTEX          , // 骨格の頂点バッファを更新するか
        FLAG_READY                           , // 準備完了
    };

    static const TbSint32 VERTEX_COUNT = 38;
    static const TbUint32 COLOR_TEXTURE_HISTORY_COUNT = 1; //!< カラーテクスチャの履歴数

    // プレイヤー情報
    struct PlayerInfo
    {
        TbSint32            skeletonIndex;
        TbBool              isEnableMask;
        TbVector4           colorSkeleton[TB_NUI_SKELETON_POS_COUNT];
        TbVertexBuffer*     skeletonVertexBuffer;              // 骨格用頂点バッファ
        TbVertexXYZColor    skeletonVertex[VERTEX_COUNT];      // 骨格用頂点
    };

    struct PlayerMaskCorrectParam
    {
        PlayerMaskCorrectParam() 
            : beginRatio(0.8f)
            , checkYBeginRatio(0.0f)
            , errorOffset(0.8f)
        {}
        TbPointS32 leftUnit;
        TbPointS32 leftLoop;
        TbPointS32 rightUnit;
        TbPointS32 rightLoop;
        TbFloat32  beginRatio;
        TbFloat32  checkYBeginRatio;
        TbFloat32  errorOffset;
        TbPointS32 depthUnit;
        TbPointS32 depthLoop;
    };

private:

    // プレイヤーのスケルトンインデックスを更新
    void updatePlayerSkeletonIndex();

    // カラーテクスチャ更新
    void updateColorTexture();

    // 深度テクスチャ更新
    void updateDepthTexture();
    
    // カラーバッファにおける骨格座標更新
    void updateColorSkeleton( TbSint32 playerIndex );

    // 骨格位置座標を頂点バッファに書き込み
    void writePlayerSkeletonPosToVertexBuffer( TbSint32 index , TbSint32 jointIndex , TbNuiSkeletonPos pos1 , TbNuiSkeletonPos pos2 );

    // 頭の位置描画可能か
    TbBool isEnableDrawHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY );
    TbBool isEnableDrawDepthHeadPointByError( const TbNuiDepthPos& depthPos , TbBool isLeft , TbBool isEnableIgnoreY );

    // トラッキングスケルトン
    TbBool isEnableTrackingPlayerSkeleotn( TbSint32 skeletonIndex ){
        return 0 <= skeletonIndex && 0 < (m_PlayerSkeletonIndexBit & TB_BIT(skeletonIndex));
    }

private:

    TbBitArray32            m_BitArray;                                  // ビット配列
    TbNuiManager*           m_NuiManager;                                // NUI管理クラス
    TbTexture*              m_ColorTexture[COLOR_TEXTURE_HISTORY_COUNT]; // カラーテクスチャ
    TbUint32                m_ColorTextureFrontIndex;                    // 先頭インデックス
    TbRectF32               m_ColorUV;
    TbTexture*              m_DepthTexture;                              // 深度テクスチャ
    TbRectF32               m_DepthUV;
    TbSint32                m_Bright;                                    // 明るさ
    TbColorU32              m_DepthMaskColor;                            // 深度マスクカラー
    PlayerMaskCorrectParam  m_PlayerMaskCorrectParam;                      // エラー許容パラメータ

    TbUint32                m_PlayerSkeletonCount;                       // プレイヤー数
    TbUint32                m_PlayerSkeletonMax;                         // プレイヤー最大数
    TbUint32                m_PlayerSkeletonIndexBit;                    // プレイヤービット値
    PlayerInfo              m_PlayerInfo[TB_NUI_TRACKED_SKELETON_MAX];   // プレイヤー別情報
};

}

#endif
