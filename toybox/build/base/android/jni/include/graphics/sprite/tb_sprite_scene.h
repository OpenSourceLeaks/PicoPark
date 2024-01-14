/*!
 * スプライト再生
 * 複数のスプライトをソートして奥から順に再生するようにする
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_SCENE_H_
#define _INCLUDED_TB_SPRITE_SCENE_H_

#include <graphics/sprite/tb_sprite_types.h>
#include <base/math/tb_matrix.h>
#include <base/container/tb_array.h>

namespace toybox
{

class TbSpriteMaterial;
class TbSprite;
class TbSpriteSceneHooker;

class TbSpriteScene
{
    friend class TbSprite;
    friend class TbSpriteSceneHooker;
public:

    // コンストラクタ
    TbSpriteScene( TbSizeT spriteMax = 0 );

    // デストラクタ
    ~TbSpriteScene();

public:

    // 初期化
    void SetCapacity( TbSizeT spriteMax );

    // 全スプライトのフレーム更新
    void Step( TbFloat32 frame );

    // 全スプライトの座標計算
    void Calculate();

    // 全スプライト描画
    void Draw();

public:

    // 全体デフォルトマテリアル設定
    void SetMaterial( TbSpriteMaterial* material ){
        m_Material = material;
    }

    // 全体共通マテリアル取得s
    const TbSpriteMaterial* GetMaterial() const {
        return m_Material;
    }

    // 全体に影響を与える姿勢行列
    void SetPoseMatrix( const TbMatrix& mtx ){
        m_PoseMatrix = mtx;
        m_PoseParam = PoseParam(); // ダイレクトの場合はリセット
    }

    // 姿勢行列取得
    const TbMatrix& GetPoseMatrix() const {
        return m_PoseMatrix;
    }

    // 姿勢設定
    void SetPose( const TbVector2& pivot , const TbVector2& t , TbAngle32 r , TbFloat32 scale )
    {
        m_PoseParam.pivot = pivot;
        m_PoseParam.t = t;
        m_PoseParam.r = r;
        m_PoseParam.s = scale;
        m_PoseMatrix = TbMatrix::IDENTITY;
        m_PoseMatrix.Translate(t.GetX(),t.GetY(),0.0f);
        m_PoseMatrix.Translate(pivot.GetX(),pivot.GetY(),0.0f);
        m_PoseMatrix.RotateZ(r);
        m_PoseMatrix.Scale(scale,scale,scale);
        m_PoseMatrix.Translate(-pivot.GetX(),-pivot.GetY(),0.0f);
    }

    // 姿勢設定
    void SetPoseTrasnlate( const TbVector2& t )
    {
        SetPose(m_PoseParam.pivot,t,m_PoseParam.r,m_PoseParam.s);
    }

    // 姿勢設定
    void SetPoseScale( TbFloat32 scale )
    {
        SetPose(m_PoseParam.pivot,m_PoseParam.t,m_PoseParam.r,scale);
    }

private:

    struct PoseParam
    {
        PoseParam() : s(1.0f ) , r(0) {}
        TbVector2 pivot;
        TbVector2 t;
        TbAngle32 r;
        TbFloat32 s;
    };

private:

    struct Node 
    {
        enum Type
        {
            TYPE_SPRITE , 
            TYPE_HOOKER 
        };

        // 奥行き取得
        TbFloat32 GetZ() const;

        // フレーム進行
        void StepFrame( TbFloat32 frame );

        // 計算
        void Calculate();

        // 描画
        void Draw( const TbMatrix& mtx , TbSpriteMaterial* material );

        // 比較演算子
        TbBool operator==( TbSprite* spr ) {
            return sprite == spr;
        }

        // 比較演算子
        TbBool operator==( TbSpriteSceneHooker* src ) {
            return hooker == src;
        }

        Type type;
        union {
            TbSprite*            sprite;
            TbSpriteSceneHooker* hooker;
        };
    };

    struct CompareFunc;
    friend struct CompareFunc;

private:

    // スプライト追加
    TbResult addSprite( TbSprite* sprite );

    // スプライト除去
    TbResult removeSprite( TbSprite* sprite );

    // フックオブジェクト追加
    TbResult addHooker( TbSpriteSceneHooker* hooker );

    // フックオブジェクト除去
    TbResult removeHooker( TbSpriteSceneHooker* hooker );

private:

    TbSpriteMaterial*   m_Material;
    TbArray<Node>       m_SpriteList;
    PoseParam           m_PoseParam;
    TbMatrix            m_PoseMatrix;

};

}

#endif
