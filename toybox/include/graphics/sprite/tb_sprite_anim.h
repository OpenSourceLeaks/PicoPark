/*!
 * スプライトノード用アニメーションツリー
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_NODE_ANIM_TREE_H_
#define _INCLUDED_TB_SPRITE_NODE_ANIM_TREE_H_

#include <base/math/tb_math_animation_types.h>
#include <base/container/tb_node_tree.h>
#include <graphics/animation/tb_graphics_animation_types.h>
#include <graphics/sprite/tb_sprite_node_anim.h>

namespace toybox
{

class TbSpriteAnim
{
public:
    typedef TbSpriteNodeAnimTreeContainer TreeContainer;
public:
    
    // コンストラクタ
    TbSpriteAnim();

    // デストラクタ
    ~TbSpriteAnim();

public:
    // 初期化
    void Initialize( TbUint32 nodeCount , TbSpriteNodeAnimTreeContainer::InitNodeParam* paramList );
public:

    // コンテナ取得
    TbSpriteNodeAnimTreeContainer& GetContainer();

    // 全ノードに対してフレームステップ
    TbResult StepFrame( TbFloat32 frame );

    // ノード指定でフレームステップ
    TbResult StepFrame( const char* nodeName , TbFloat32 frame , TbBool canApplyChild = TB_TRUE );

    // ノード指定でフレームステップ
    TbResult StepFrame( TbUint32 nodeIndex , TbFloat32 frame , TbBool canApplyChild = TB_TRUE );

    // 全ノードに対してフレーム強制設定
    TbResult SetFrame( TbFloat32 frame );

    // ノード指定でフレームステップ
    TbResult SetFrame( const char* nodeName , TbFloat32 frame , TbBool canApplyChild = TB_TRUE );

    // ノード指定でフレームステップ
    TbResult SetFrame( TbUint32 nodeIndex , TbFloat32 frame , TbBool canApplyChild = TB_TRUE );

private:
    
    // ノード指定でフレームステップ
    TbResult stepFrame( TreeContainer::Node& node , TbFloat32 frame , TbBool canApplyChild );
    
    // ノード指定でフレームステップ
    TbResult setFrame( TreeContainer::Node& node , TbFloat32 frame , TbBool canApplyChild );

private:
    TreeContainer m_Container;                // コンテナ
};

}

#endif
