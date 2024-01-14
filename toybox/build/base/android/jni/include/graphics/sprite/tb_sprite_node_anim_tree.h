/*!
 * スプライトノード用アニメーションツリー
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_NODE_ANIM_H_
#define _INCLUDED_TB_SPRITE_NODE_ANIM_H_

#include <base/container/tb_node_tree.h>
#include <base/math/tb_math_animation_types.h>
#include <graphics/animation/tb_graphics_animation_types.h>
#include <graphics/sprite/tb_sprite_types.h>

namespace toybox
{

// ノード毎のアニメーション情報
class TbSpriteNodeAnim
{
public:

    typedef TbSpriteNodeAnim Self;

public:

    // コンストラクタ
    TbSpriteNodeAnim();

    // デストラクタ
    virtual ~TbSpriteNodeAnim();

public: // セットアップ

    // 初期化
    void Initialize( const TbSpriteNodeAnimInitParam& param );

public:
    
    // 開始フレーム設定(デフォルトは最初のキーフレーム)
    TbResult SetFrameBegin( TbFloat32 begin ) {
        m_FrameBegin = begin;
        return TB_S_OK;
    }
    
    // 開始フレーム取得
    TbFloat32 GetFrameBegin() const {
        return m_FrameBegin;
    }

    // 終了フレームを強制設定(デフォルトは最後のキーフレーム)
    TbResult SetFrameEnd( TbFloat32 end ) {
        m_FrameEnd = end;
        return TB_S_OK;
    }

    // 終了フレーム取得
    TbFloat32 GetFrameEnd() const {
        return m_FrameEnd;
    }

public:

    // 矩形データ取得
    const TbAnimationKeyFrameDataRectF32& GetRectData() const {
        return m_Animation.rect;
    }

    // UV矩形データ取得
    const TbAnimationKeyFrameDataRectF32& GetUVOffsetData() const {
        return m_Animation.uvOffset;
    }

    // 移動データ取得
    const TbAnimationKeyFrameDataVec2f& GetTransData() const {
        return m_Animation.translate;
    }

    // 回転データ取得
    const TbAnimationKeyFrameDataF32& GetRotData() const {
        return m_Animation.rot;
    }

    // スケールデータ取得
    const TbAnimationKeyFrameDataVec2f& GetScaleData() const {
        return m_Animation.scale;
    }

    // カラーデータ取得
    const TbAnimationKeyFrameDataColorR32G32B32A32& GetColorData() const {
        return m_Animation.color;
    }

public:

    struct Animation
    {
        TbAnimationKeyFrameDataRectF32          rect;       // 矩形
        TbAnimationKeyFrameDataRectF32          uvOffset;   // UVオフセット
        TbAnimationKeyFrameDataVec2f            translate;  // 移動
        TbAnimationKeyFrameDataF32              rot;        // 回転
        TbAnimationKeyFrameDataVec2f            scale;      // 拡大
        TbAnimationKeyFrameDataColorR32G32B32A32    color;      // カラー
    };

private:

    TbBitArray32   m_BitArray;          // ビット配列
    Animation      m_Animation;         // アニメーション
    TbFloat32      m_FrameBegin;        // 開始フレーム
    TbFloat32      m_FrameEnd;          // 終了フレーム

};

// スプライトノード用アニメーションツリー
typedef TbNodeTree<TbSpriteNodeAnim,TbSpriteNodeAnimInitParam> TbSpriteNodeAnimTree;

}

#endif
