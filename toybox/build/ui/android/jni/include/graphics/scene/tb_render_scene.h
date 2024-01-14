/*!
 * 描画シーン
 * @author Miyake Shunsuke
 * @since 2013.05.07
 */

#ifndef _INCLUDED_TB_RENDER_SCENE_H_
#define _INCLUDED_TB_RENDER_SCENE_H_

#include <base/math/tb_geometry.h>

namespace toybox
{

class TbCamera;
class TbRenderSceneNode;

class TbRenderScene
{
    friend class TbRenderSceneNode;
public:

    // コンストラクタ
    TbRenderScene();

    // デストラクタ
    ~TbRenderScene();

public:
    
    // 更新(アニメーションの更新など)
    void Step( TbFloat32 deltatime );
    
    // 描画
    void Draw();

public:

    // レンダーパス設定
    void SetRenderPass( TbUint32 pass ) {
        m_RenderPass = pass;
    }

    // 更新可能か
    void SetEnableStep( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_STEP,isEnable);
    }

    // 更新可能か
    TbBool IsEnableStep() const {
        return m_BitArray.Test(FLAG_ENABLE_STEP);
    }

    // 描画可能か
    void SetEnableDraw( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_DRAW,isEnable);
    }

    // 描画可能か
    TbBool IsEnableDraw() const {
        return m_BitArray.Test(FLAG_ENABLE_DRAW);
    }

private:

    enum
    {
        FLAG_ENABLE_STEP , 
        FLAG_ENABLE_DRAW , 
        FLAG_MAX ,
    };

    struct ViewInfo
    {
        TbCamera* camera;
        TbUint32  cameraCount;
        TbUint32  prevCameraIndex;
        TbUint32  currentCameraIndex;
        TbFloat32 ratio;
        TbRectF32 viewport;
        TbUint32  passIndex;
    };
    static const TbUint32 VIEW_INFO_MAX = 4;

private:

    // ノード登録
    void addNode( TbRenderSceneNode* node );

    // ノードを除去
    void removeNode( TbRenderSceneNode* node );
    
private:
    
    TbBitArray32        m_BitArray;             // ビット配列
    TbRenderSceneNode*  m_Top;                  // 
    TbUint32            m_RenderPass;           // 対象のレンダーパス
    ViewInfo            m_View[VIEW_INFO_MAX];  // 
    
};

}

#endif
