/*!
 * スプライト再生クラス
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_H_
#define _INCLUDED_TB_SPRITE_H_

#include <base/math/tb_matrix.h>
#include <base/time/tb_frame_rate_control.h>
#include <graphics/sprite/tb_sprite_node_tree.h>
#include <graphics/sprite/tb_sprite_node_anim_tree.h>

namespace toybox
{

class TbSpriteMaterial;
class TbSpriteScene;

class TbSprite
{
public:

    typedef TbSprite Self;

public:

    // コンストラクタ
    TbSprite();

    // デストラクタ
    ~TbSprite();

public: // セットアップ

    // ノードツリー設定
    TbResult Initialize( TbSpriteNodeTree* nodeTree , TbSpriteNodeAnimTree* anim = nullptr );

    // ノードツリー設定
    TbResult Initialize( const TbRectF32& rect , 
                         const TbRectF32& uvRect );

    // 後始末
    void Finalize();

    // 初期化済みか
    TbBool IsInitialized() const {
        return m_NodeInfoList != nullptr;
    }

    // マテリアル設定
    void SetMaterial( TbUint32 index , TbSpriteMaterial* material );

    // マテリアル設定
    void SetMaterial( TbSpriteMaterial* material ) {
        m_Material = material;
    }

    // ノードアニメーション設定
    void SetNodeAnimation( TbSpriteNodeAnimTree* sprite );

    // ノード数取得
    TbSizeT GetNodeCount() const {
        return m_NodeCount;
    }

    // ノード取得
    TbSpriteNodeTree* GetNodeTree() {
        return m_NodeTree;
    }

    // ノード取得
    const TbSpriteNodeTree* GetNodeTree() const {
        return m_NodeTree;
    }

public: // シーン

    // シーンに入る
    void Enter( TbSpriteScene* scene );

    // シーンから解除
    void Exit();

    // シーンに入っているか
    TbBool IsEntered() const {
        return m_Scene != nullptr;
    }

    // シーン取得
    TbSpriteScene* GetScene() {
        return m_Scene;
    }

public: // TRS
    
    // 位置設定(X座標)
    void SetTranslateX( TbFloat32 x ) {
        m_Trans.SetX(x);
    }
    
    // 位置設定(Y座標)
    void SetTranslateY( TbFloat32 y ) {
        m_Trans.SetY(y);
    }

    // 位置設定
    void SetTranslate( const TbVector2& pos ) {
        m_Trans.Set(pos.GetX(),pos.GetY());
    }

    // 位置設定
    void SetTranslate( TbFloat32 x , TbFloat32 y ) {
        m_Trans.Set(x,y);
    }

    // 位置加算
    void AddTranslate( TbFloat32 x , TbFloat32 y ) {
        m_Trans.Add(x,y);
    }

    // 位置加算
    void AddTranslate( const TbVector2& vec ) {
        m_Trans.Add(vec.GetX(),vec.GetY());
    }

    // 描画時の位置取得
    const TbVector2& GetTranslate() const {
        return m_Trans;
    }

    // 回転角度設定
    void SetRotation( TbAngle32 rot );

    // 回転角度設定
    void AddRotation( TbAngle32 rot ) {
        SetRotation( m_Rot + rot );
    }

    // 回転角度取得
    TbAngle32 GetRotation() const {
        return m_Rot;
    }

    // スケール設定
    void SetScale( TbFloat32 scale ) {
        m_Scale.Set(scale,scale);
    }

    // スケール設定
    void SetScale( TbFloat32 scaleX , TbFloat32 scaleY ) {
        m_Scale.Set(scaleX,scaleY);
    }

    // スケール設定
    void SetScale( const TbVector2& scale ) {
        m_Scale = scale;
    }

    // スケール取得
    const TbVector2& GetScale() const {
        return m_Scale;
    }

    // カラー設定(
    void SetColor( const TbColor& color ) {
        m_Color = color;
    }

    // カラー設定(アルファは反映されません)
    void SetColorExceptAlpha( const TbColor& color ) {
        m_Color.SetColorU8( color.GetRedU8() , color.GetBlueU8() , color.GetGreenU8() );
    }

    // アルファ設定
    void SetAlpha( TbFloat32 alpha ) {
        m_Color.SetAlphaF32(alpha);
    }

    // アルファ設定
    TbFloat32 GetAlpha() const {
        return m_Color.GetAlphaF32();
    }

    // カラー取得
    const TbColor& GetColor() const {
        return m_Color;
    }

    // 奥行き設定
    void SetZ( TbFloat32 z ) {
        m_Z = z;
    }

    // 奥行き取得
    TbFloat32 GetZ() const {
        return m_Z;
    }

    // 表示可能設定
    void SetVisible( TbBool isVisible ) {
        m_BitArray.SetBit(FLAG_VISIBLE,isVisible);
    }

    // 表示可能か
    TbBool IsVisible() const {
        return m_BitArray.Test(FLAG_VISIBLE);
    }

    // UV設置絵
    void SetUV( const TbRectF32& uvRect , TbSint32 nodeIndex = 0 ) {
        if( m_NodeTree ) {
            TbSpriteNodeTree::Node* node = m_NodeTree->GetNodeFromIndex(nodeIndex);
            if( node ){
                node->GetBody().uvRect = uvRect;
            }
        }else{
            m_SingleNodeInfo.uvRect = uvRect;
        }
    }

    // UV設定取得
    const TbRectF32& GetUV(TbSint32 nodeIndex = 0) const 
    {
        if (m_NodeTree) {
            TbSpriteNodeTree::Node* node = m_NodeTree->GetNodeFromIndex(nodeIndex);
            if (node) {
                return node->GetBody().uvRect;
            }
        }
        return m_SingleNodeInfo.uvRect;
    }

public: // アニメーション再生

    // 直接フレームを進める
    void StepFrame( TbFloat32 frame );

    // ステップ進行速度設定
    void SetStepFrameSpeed(TbFloat32 speed) {
        m_StepFrameSpeed = speed;
    }

    // 直接フレーム設定
    void SetFrame( TbFloat32 frame );

    // フレーム取得
    TbFloat32 GetFrame() const {
        return m_Frame;
    }

    // フレーム取得
    TbFloat32 GetFrameBegin() const {
        return m_FrameBegin;
    }

    // フレーム取得
    TbFloat32 GetFrameEnd() const {
        return m_FrameEnd;
    }

    // ループ設定
    void SetEnableLoop( TbBool isLoop ) {
        m_BitArray.SetBit( FLAG_LOOP , isLoop );
    }

    // ループ取得
    TbBool IsEnableLoop() const {
        return m_BitArray.Test(FLAG_LOOP);
    }

    // ステップ更新可能か
    void SetEnableStep( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_DISABLE_STEP,!isEnable);
    }

    // ステップ更新可能か
    TbBool IsEnableStep() const {
        return !m_BitArray.Test(FLAG_DISABLE_STEP);
    }

    // フレームレートコントロール設定
    void SetFrameRateControl( const TbFrameRateControl* control ){
        m_FrameRateControl = control;
    }

    // アニメーション再生終了
    TbBool IsPlayEnd() const {
        if( TB_FLOAT32_EQUAL(GetFrameEnd(),GetFrame()) ){
            return TB_TRUE;
        }
        return TB_FALSE;
    }

public: // 描画

    // 座標計算
    TbResult Calculate();

    // 描画
    TbResult Draw( const TbMatrix& mtx );

    // 描画
    TbResult Draw( const TbMatrix& mtx , TbSpriteMaterial* defaultMaterial );

public: // デバッグ

    // 名前設定
    void SetName( const char* name ) {
        m_Name = name;
    }

private:

    struct Animation
    {
        TbAnimationKeyFrameRectF32  rect;       // 矩形
        TbAnimationKeyFrameRectF32  uvOffset;   // UVオフセット矩形
        TbAnimationKeyFrameVec2f    translate;  // 移動
        TbAnimationKeyFrameF32      rot;        // 回転
        TbAnimationKeyFrameVec2f    scale;      // 拡大
        TbAnimationKeyFrameColorR32G32B32A32    color;      // 拡大
    };

    struct NodeInfo
    {
        NodeInfo() 
            : index(0) , rect() , uvRect() , color() , trsMatrix() , material(nullptr) 
        {
            trsMatrix.Unit();
        }
        ~NodeInfo() {}
        TbUint32          index;      // インデックス値
        TbRectF32         rect;       // 矩形
        TbRectF32         uvRect;     // 矩形UV
        TbColor           color;      // カラー
        TbMatrix          trsMatrix;  // 描画時の変換行列
        Animation         anim;       // アニメーション
        TbSpriteMaterial* material;   // マテリアル
    };

    enum
    {
        FLAG_LOOP ,         // ループ再生可能
        FLAG_DISABLE_STEP , // フレームのステープ更新を禁止
        FLAG_UPDATE_ANIM ,  // アニメーション更新
        FLAG_VISIBLE     ,  // 表示可能
    };

private:

    // 座標計算
    TbResult calcNode( const TbSpriteNodeTree::Node& node , 
                       const TbMatrix& parentMatrix );

    // ノード情報リスト削除
    void deleteNodeInfoList() {
        if( m_NodeInfoList != &m_SingleNodeInfo ){
            TB_SAFE_DELETE_ARRAY(m_NodeInfoList);
        }else{
            m_NodeInfoList = nullptr;
        }
        m_NodeCount = 0;
    }

private:

    TbBitArray32                m_BitArray;             // ビット配列
    TbStaticString32            m_Name;
    TbSpriteMaterial*           m_Material;             // マテリアル (一括設定用)
    TbSizeT                     m_NodeCount;
    NodeInfo*                   m_NodeInfoList;         // ノード情報配列

private: // 単一矩形なスプライト用

    NodeInfo                    m_SingleNodeInfo;

private: // 階層構造スプライト用

    TbSpriteNodeTree*           m_NodeTree;             // ノードツリー
    
private:    // アニメーション

    TbSpriteNodeAnimTree*       m_NodeAnimTree;         // アニメーションツリー
    TbFloat32                   m_Frame;                // フレーム
    TbFloat32                   m_FrameBegin;           // 開始フレーム
    TbFloat32                   m_FrameEnd;             // 終了フレーム
    TbFloat32                   m_StepFrameSpeed;       // 進行フレーム速度
    const TbFrameRateControl*   m_FrameRateControl;     // フレームレートコントロール

private: // 全ノードに適用されるパラメータ

    TbFloat32    m_Z;                    // 奥行き
    TbVector2    m_Trans;                // 位置
    TbAngle32    m_Rot;                  // 回転
    TbVector2    m_Scale;                // スケール
    TbColor      m_Color;                // カラー

private: // シーン情報

    TbSpriteScene*  m_Scene;    // 所属シーン

};

}

#endif
