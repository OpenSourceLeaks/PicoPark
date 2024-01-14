/*! 
 * KinectSDK依存部分
 * @author teco
 */
 
#ifndef _INCLUDED_TB_NUI_MANAGER_KINECT2_H_
#define _INCLUDED_TB_NUI_MANAGER_KINECT2_H_

#include <base/util/tb_async_func.h>

namespace toybox
{

class TbNuiManager;

class TbNuiManagerDepend
{
    typedef TbNuiManagerDepend Self;
    friend class TbNuiManager;
public:
    // コンストラクタ
    TbNuiManagerDepend();
    // デストラクタ
    ~TbNuiManagerDepend();
public:

    // 更新
    void Update();

public: // クエリー

    // センサー取得
    IKinectSensor* GetSensor() {
        return m_KinectSensor;
    }

    // マッパー
    ICoordinateMapper* GetCoordinateMapper() {
        return m_CoordinateMapper;
    }

    // カラーフレーム取得
    IColorFrame* GetColorFrame() {
        return m_ColorFrame;
    }

    // 深度フレーム取得
    IDepthFrame* GetDepthFrame() {
        return m_DepthFrame;
    }

    // ボディーインデックスフレーム取得
    IBodyIndexFrame* GetBodyIndexFrame() {
        return m_BodyIndexFrame;
    }

    // フレームリセット
    void ResetFrame() {
        TB_SAFE_RELEASE(m_ColorFrame);
        TB_SAFE_RELEASE(m_DepthFrame);
        TB_SAFE_RELEASE(m_BodyIndexFrame);
    }

private:

    // 初期化
    TbSint32 onInitialize( TbAsyncFunc& asyncFunc );

    // 初期化後処理
    TbSint32 onAfterInitialize( TbAsyncFunc& asyncFunc , const TbUint32 funcIndex );

    // 更新
    TbSint32 onUpdate( TbAsyncFunc& asyncFunc );

    // 骨格フレーム更新
    void onUpdateBodyFrame();

    // マルチソース更新
    void onUpdateMultiSource();

private:

    // 共通部分設定
    void setCommon( TbNuiManager* common ) { m_Common = common; }

private:
    TbAsyncFunc              m_AsyncFunc;     //!< 非同期関数
    TbNuiManager*            m_Common;        //!< 共通部分
    IKinectSensor*           m_KinectSensor;  //!< 
    ICoordinateMapper*       m_CoordinateMapper;
    IBodyFrameReader*        m_BodyFrameReader;
    DepthSpacePoint*         m_ColorDepthPoint;   // カラー用深度座標
    IMultiSourceFrameReader* m_MultiSourceFrameReader;
    IColorFrame*             m_ColorFrame;
    IDepthFrame*             m_DepthFrame;
    IBodyIndexFrame*         m_BodyIndexFrame;
};

#define TB_NUI_MANAGER_FRIEND_DECL() friend class TbNuiManagerDepend

}

#endif
