/*! 
 * NUI(Natural User Interface)管理クラス
 * @author Miyake Shunsuke
 * @since 2011.07.21
 */

#ifndef _INCLUDED_TB_NUI_MANAGER_H_
#define _INCLUDED_TB_NUI_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <nui/tb_nui_types.h>
#include "./depend/tb_nui_manager_depend.h"

namespace toybox
{

class TbNuiDepthFrame; // 深度情報
class TbNuiColorFrame; // 画像情報

class TbNuiManager : public TbSingletonRef<TbNuiManager>
{
    TB_NUI_MANAGER_FRIEND_DECL();
public:

    // コンストラクタ
    explicit TbNuiManager( const TbNuiCreateParam& param );
    // デストラクタ
    ~TbNuiManager();

private: // コピー禁止

public:

    // 更新
    void Update();

public: // 情報取得

    // 準備完了
    TbBool IsReady() const { return m_BitArray.Test( FLAG_READY ); }

    // 生成パラメータ取得
    const TbNuiCreateParam& GetCreateParam() const { return m_CreateParam; }

    // スケルトンを取得
    const TbNuiSkeleton* GetSkeleton( TbUint32 index = 0 ) const;

    // 深度フレームを取得
    TbNuiDepthFrame* GetDepthFrame( TbNuiImageResolutionType resolution , TbUint32 index = 0 );
    TbNuiDepthFrame* GetDepthFrame( TbUint32 index = 0 );
    const TbNuiDepthFrame* GetDepthFrame( TbNuiImageResolutionType resolution , TbUint32 index = 0 ) const;
    const TbNuiDepthFrame* GetDepthFrame( TbUint32 index = 0 ) const;
    
    // RGBフレームを取得
    TbNuiColorFrame* GetColorFrame( TbNuiImageResolutionType resolution , TbUint32 index = 0 );
    TbNuiColorFrame* GetColorFrame( TbUint32 index = 0 );
    const TbNuiColorFrame* GetColorFrame( TbNuiImageResolutionType resolution , TbUint32 index = 0 ) const;
    const TbNuiColorFrame* GetColorFrame( TbUint32 index = 0 ) const;
    
    //! 接続済みデバイス数取得
    TbUint32 GetConnectedDeviceCount() const 
    { 
        return m_ConnectedDeviceCount;
    }

    //! 追跡中スケルトン数取得
    TbUint32 GetTrackedSkeletonCount() const
    {
        return m_TrackedSkeletonCount;
    }

    //! スケルトン情報更新可能設定
    TbResult SetEnableUpdateSkeleton( TbBool isEnable )
    {
        m_BitArray.SetBit( FLAG_ENABLE_UPDATE_SKELETON , isEnable );
    }
    
    //! スケルトン情報更新可能設定取得
    TbBool IsEnableUpdateSkeleton() const
    {
        m_BitArray.Test( FLAG_ENABLE_UPDATE_SKELETON );
    }

    // カラー情報更新可能設定
    TbResult SetEnableUpdateColor( TbBool isEnable )
    {
        m_BitArray.SetBit( FLAG_ENABLE_UPDATE_DEPTH , isEnable );
    }

    // カラー情報更新可能設定取得
    TbBool IsEnableUpdateColor() const
    {
        return m_BitArray.Test( FLAG_ENABLE_UPDATE_COLOR );
    }

    // 深度情報更新可能設定
    void SetEnableUpdateDepth( TbBool isEnable )
    {
        m_BitArray.SetBit( FLAG_ENABLE_UPDATE_DEPTH , isEnable );
    }

    // 深度情報更新可能設定取得
    TbBool IsEnableUpdateDepth() const
    {
        return m_BitArray.Test( FLAG_ENABLE_UPDATE_DEPTH );
    }

    // スケルトンにデバイス傾き補正をかけるかどうか
    void SetEnableTiltCorrection( TbBool isEnable )
    {
        m_BitArray.SetBit( FLAG_ENABLE_TILT_CORRECTION , isEnable );
    }

    // スケルトンにデバイス傾き補正をかけるかどうかの設定取得
    TbBool IsEnableTiltCorrection() const
    {
        return m_BitArray.Test( FLAG_ENABLE_TILT_CORRECTION );
    }

    // スケルトン座標を深度画像にマッピング
    TbResult TransformSkeletonToDepth( TbNuiDepthPos& depthPos , const TbVector4& skeletonPos );

    // 深度座標をスケルトン座標に変換
    TbResult TransformDepthToSkeleton( TbVector4& skeletonPos , const TbNuiDepthPos& depthPos );

    // スケルトン座標を深度画像にマッピング
    TbResult TransformSkeletonToColor( TB_INOUT TbNuiColorPos& colorPos , const TbVector4& skeletonPos );

    // 深度座標をカラー座標に変換
    TbResult TransformDepthToColor( TB_INOUT TbNuiColorPos& colorPos , const TbNuiDepthPos& depthPos );

    // カラー座標を深度座標に変換
    TbResult TransformColorToDepth( TB_INOUT TbNuiDepthPos& depthPos , TbSint16* skeletonIndex , const TbNuiColorPos& colorPos );

public: // デバイス制御

    // 傾き加える
    TbResult AddTiltMotorAngleDeg( const TbFloat32 deg );

    // 傾き取得
    TbResult SetTiltMotorAngleDeg( const TbFloat32 deg );
    
    // 傾き設定
    TbResult GetTiltMotorAngleDeg( TbFloat32& deg ) const;
    
    // 正規化重力ベクトル
    const TbVector4& GetGravityNormal() const { return m_GravityNormal; }

public: // 依存部分
    
    // 依存部分アクセス
    TbNuiManagerDepend& GetDepend() { 
        return m_Depend; 
    }

    const TbNuiManagerDepend& GetDepend() const { 
        return m_Depend; 
    } 

private:

    enum {
        FLAG_READY ,                  // 準備完了
        FLAG_ENABLE_UPDATE_SKELETON , // 骨格情報を更新可能
        FLAG_ENABLE_UPDATE_COLOR    , // カラー情報を更新可能
        FLAG_ENABLE_UPDATE_DEPTH    , // 深度情報を更新可能
        FLAG_ENABLE_TILT_CORRECTION , // デバイスの傾き補正を自動で行う
    };

private:

    TbBitArray32         m_BitArray;                                     //!< ビット配列
    TbNuiCreateParam     m_CreateParam;                                  //!< 生成パラメータ
    TbUint32             m_ConnectedDeviceCount;                         //!< 接続済みデバイス数
    TbVector4            m_GravityNormal;                                //!< 重力ベクトル
    TbNuiSkeleton        m_Skeleton[TB_NUI_SKELETON_MAX];                //!< スケルトン情報
    TbUint32             m_TrackedSkeletonCount;                         //!< 追跡スケルトン数
    TbNuiTrackedSkeleton m_TrackedSkeleton[TB_NUI_TRACKED_SKELETON_MAX]; //!< 追跡スケルトン情報
    TbNuiDepthFrame*     m_DepthFrame;                                   //!< 深度情報
    TbNuiColorFrame*     m_ColorFrame;                                   //!< 画像情報
    TbNuiManagerDepend   m_Depend;                                       //!< 依存部分

};

}

#endif
