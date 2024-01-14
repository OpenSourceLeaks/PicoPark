/*!
 * MMD用描画オブジェクト
 * @author Miyake_Shunsuke
 * @since 2014.04.05
 */

#ifndef _INCLUDED_TB_RENDER_OBJECT_MMD_H_
#define _INCLUDED_TB_RENDER_OBJECT_MMD_H_

#include <graphics/material/tb_material_pmx.h>
#include <graphics/motion/tb_motion_vmd.h>
#include <graphics/resource/tb_resource_model_pmx.h>
#include <graphics/scene/tb_render_scene_node.h>

namespace toybox
{

class TbMesh;
class TbSkeletonPmx;
class TbMotionVmd;
class TbShaderProgram;

class TbRenderObjectMmd : public TbRenderSceneNode
{
public:

    // コンストラクタ
    TbRenderObjectMmd();

    // デストラクタ
    ~TbRenderObjectMmd();

public: // セットアップ

    // モデルリソースから設定
    void Setup( TbResourceHandleModelPmx& model , TbShaderProgram* materialShader );

    // セットアップ(手動設定用)
    void Setup( TbUint32 materialCount );

public: // 表示個別設定

    // メッシュ設定
    void SetMesh( TbMesh* mesh );

    // マテリアル用パラメータ設定
    void SetMaterialShader( TbShaderProgram* materialShader );

    // マテリアル用シェーダ設定
    void SetMaterialShader( TbUint32 index , TbShaderProgram* materialShader );

public: // 状態設定

    // マテリアル毎に可否設定する
    void SetVisibleMaterial( TbUint32 materialIndex , TbBool isVisible );

    // マテリアル毎に可否設定する
    void SetVisibleMaterial( TbBool isVisible );

public: // アニメーション

    // スケルトン設定
    void SetSkeleton( const TbSkeletonPmx* skeleton ) {
        m_Motion.SetSkeleton( skeleton );
    }

    // モーション設定
    void SetMotion( const TbMotionDataVmd* motion , TbFloat32 startFrame = 0.0f ) {
        m_Motion.SetMotion( motion );
        m_Motion.SetFrame(startFrame);
    }

    // フレーム設定
    void SetFrame( TbFloat32 frame ) {
        m_Motion.SetFrame( frame );
    }

    // フレーム取得
    TbFloat32 GetFrame() const {
        return m_Motion.GetFrame();
    }

    // ループ設定
    void SetLoop( TbBool isLoop ) {
        m_Motion.SetLoop( isLoop );
    }

    // IK有効設定
    void SetEnableIK( TbBool isEnable ) {
        m_Motion.SetEnableIK(isEnable);
    }

    // IK有効かどうか
    TbBool IsEnableIK() const {
        return m_Motion.IsEnableIK();
    }

    // 行列取得
    const TbMatrix* GetMatrix( TbUint32 boneIndex ) {
        return m_Motion.GetMatrix( boneIndex );
    }

protected:

    // 更新
    virtual void OnStepImpl( TbFloat32 deltatime );

    // 描画
    virtual void OnDrawImpl( TbUint32 passIndex );

private:

    struct MaterialState {
        MaterialState() { bitArray.SetBit(FLAG_VISIBLE); }
        enum Flag
        {
            FLAG_VISIBLE ,
        };
        TbMaterialPmx   material;
        TbBitArray32    bitArray;
    };

private:

    TbUint32        m_MaterialCount;
    MaterialState*  m_MaterialState;
    TbMesh*         m_Mesh;
    TbMotionVmd     m_Motion;

};

}

#endif