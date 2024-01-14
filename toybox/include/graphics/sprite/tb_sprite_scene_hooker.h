/*!
 * スプライトシーンの処理をフックするクラス
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_SCENE_HOOKER_H_
#define _INCLUDED_TB_SPRITE_SCENE_HOOKER_H_

#include <base/math/tb_matrix.h>
#include <base/util/tb_function.h>

namespace toybox
{

class TbSpriteMaterial;
class TbSpriteScene;

class TbSpriteSceneHooker
{
public:
    typedef TbStaticFunction32<void (TbFloat32)>                                        StepFunc;
    typedef TbStaticFunction32<void (const TbMatrix& poseMatrix,TbSpriteMaterial*)>     DrawFunc;
    typedef TbStaticFunction32<void ()>                                                 CalcFunc;
public:

    // コンストラクタ
    TbSpriteSceneHooker();

    // デストラクタ
    virtual ~TbSpriteSceneHooker();

public: // シーン

    // シーンに入る
    void Enter( TbSpriteScene* scene );

    // シーンから解除
    void Exit();

public:
    
    // 奥行き設定
    void SetZ( TbFloat32 z ) {
        m_Z = z;
    }

    // 奥行き取得
    TbFloat32 GetZ() const {
        return m_Z;
    }

    // 表示設定
    void SetVisible(TbBool isVisible) {
        m_IsVisible = isVisible;
    }

    // 表示設定取得
    TbBool IsVisible() const {
        return  m_IsVisible;
    }

    // マテリアル設定
    void SetMaterial(TbSpriteMaterial* material) {
        m_Material = material;
    }

public:

    // 計算フック関数設定
    void SetCalcFunc( const CalcFunc& func ) {
        m_CalcFunc = func;
    }

    // ステップフック関数設定
    void SetStepFunc( const StepFunc& func ) {
        m_StepFunc = func;
    }

    // 描画フック関数設定
    void SetDrawFunc( const DrawFunc& func ) {
        m_DrawFunc = func;
    }

public: // フック
    
    // 直接フレームを進める
    void StepFrame( TbFloat32 frame ) {
        if( m_StepFunc.IsValid() ){
            m_StepFunc(frame);
        }
    }

    // 座標計算
    TbResult Calculate() {
        if( m_CalcFunc.IsValid() ){
            m_CalcFunc();
        }
        return TB_S_OK;
    }

    // 描画
    TbResult Draw( const TbMatrix& poseMatrix , TbSpriteMaterial* defaultMaterial );

private: // 全ノードに適用されるパラメータ

    TbFloat32           m_Z;        // 奥行き
    TbBool              m_IsVisible;// 描画するか
    TbSpriteMaterial*   m_Material; // マテリアル
    TbSpriteScene*      m_Scene;    // 所属シーン
    CalcFunc            m_CalcFunc; // Calculateフック関数
    StepFunc            m_StepFunc; // ステップフック関数
    DrawFunc            m_DrawFunc; // 描画フック関数

};

}

#endif
