/*! 
 * KinectSDK依存部分
 * @author Miyake Shunsuke
 * @since 2011.07.22
 */
 
#ifndef _INCLUDED_TB_NUI_KINECT_H_
#define _INCLUDED_TB_NUI_KINECT_H_

#include <base/util/tb_async_func.h>
#include <base/math/tb_geometry.h>

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

private:

    // 初期化
    TbSint32 onInitialize( TbAsyncFunc& asyncFunc );

    // 初期化後処理
    TbSint32 onAfterInitialize( TbAsyncFunc& asyncFunc , const TbUint32 funcIndex );

    // 更新
    TbSint32 onUpdate( TbAsyncFunc& asyncFunc );

private:

    // 共通部分設定
    void setCommon( TbNuiManager* common ) { m_Common = common; }

public:
    TbAsyncFunc        m_AsyncFunc; //!< 非同期関数
    TbNuiManager*      m_Common;    //!< 共通部分
    NUI_SKELETON_FRAME m_SkeletonFrame; //!< 骨格フレーム
    TbPointS16*        m_ColorDepthPoint;   // カラー用深度座標
};

#define TB_NUI_MANAGER_FRIEND_DECL() friend class TbNuiManagerDepend

}

#endif
