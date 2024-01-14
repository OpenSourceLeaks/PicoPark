/*!
 * スプライトアニメーション用ノードツリー
 * @author Miyake Shunsuke
 * @since 2012.01.15
 */

#include "tb_fwd.h"
#include "graphics/sprite/tb_sprite_node_anim_tree.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.05
 */
TbSpriteNodeAnim::TbSpriteNodeAnim()
    : m_BitArray()
    , m_Animation()
    , m_FrameBegin(0.0f)
    , m_FrameEnd(0.0f)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.05
 */
TbSpriteNodeAnim::~TbSpriteNodeAnim()
{
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 * @since 2012.02.05
 */
void TbSpriteNodeAnim::Initialize( const TbSpriteNodeAnimInitParam& param )
{
    if( param.rectAnim ){
        m_Animation.rect = *param.rectAnim;
    }
    if( param.uvOffsetAnim ){
        m_Animation.uvOffset = *param.uvOffsetAnim;
    }
    if( param.translateAnim ){
        m_Animation.translate = *param.translateAnim;
    }
    if( param.rotAnim ){
        m_Animation.rot = *param.rotAnim;
    }
    if( param.scaleAnim ){
        m_Animation.scale = *param.scaleAnim;
    }
    if (param.colorAnim) {
        m_Animation.color = *param.colorAnim;
    }

    // 最終フレームを揃える
    TbFloat32 frameEnd = m_Animation.rect.GetFrameEnd();
    frameEnd = TbMax( frameEnd , m_Animation.uvOffset.GetFrameEnd() );
    frameEnd = TbMax( frameEnd , m_Animation.translate.GetFrameEnd() );
    frameEnd = TbMax( frameEnd , m_Animation.rot.GetFrameEnd() );
    frameEnd = TbMax( frameEnd , m_Animation.scale.GetFrameEnd() );
    frameEnd = TbMax(frameEnd, m_Animation.color.GetFrameEnd());
    SetFrameBegin( 0.0f );
    SetFrameEnd( frameEnd );
}

}
