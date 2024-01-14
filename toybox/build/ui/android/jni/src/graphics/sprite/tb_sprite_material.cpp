/*!
 * スプライトマテリアル
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#include "tb_fwd.h"
#include "graphics/sprite/tb_sprite_material.h"
#include "graphics/render/tb_draw_2d.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.21
 */
TbSpriteFixedMaterial::TbSpriteFixedMaterial( TbUint32 passIndex  )
    : m_PassIndex(passIndex)
    , m_Color(TbColor::White())
    , m_OffsetUV()
    , m_Texture(nullptr)
    , m_Anim()
{
}
    
/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2012.02.21
 */
TbSpriteFixedMaterial::~TbSpriteFixedMaterial()
{
}

/*!
 * ステップフレーム
 * @author Miyake Shunsuke
 * @since 2012.02.21
 */
void TbSpriteFixedMaterial::StepFrame( TbFloat32 stepFrame )
{
    if( !m_Anim.color.IsEmpty() ){
        m_Anim.color.StepFrame(stepFrame);
    }
    if( !m_Anim.offsetUV.IsEmpty() ){
        m_Anim.offsetUV.StepFrame(stepFrame);
    }
}

/*!
 * 描画開始
 * @author Miyake Shunsuke
 * @since 2012.02.26
 */
void TbSpriteFixedMaterial::BeginDraw()
{
    TbDraw2dSetRenderPass(m_PassIndex);
    m_Texture ? TbDraw2dTextureBegin() : TbDraw2dBegin();
}

/*!
 * 描画
 * @author Miyake Shunsuke
 * @since 2012.02.21
 */
void TbSpriteFixedMaterial::Draw( const TbRectF32& rect , 
                                  const TbRectF32& uvRect , 
                                  const TbColor&        color , 
                                  const TbMatrix&       drawMatrix )
{
    // TbDraw系に依存した描画
    TbColor resultColor = m_Color * color;
    TbRectF32 resultRectUV = uvRect;
    TbBool isValidUV = !(TB_FLOAT32_EQUAL(uvRect.x,0.0f) && 
                         TB_FLOAT32_EQUAL(uvRect.y,0.0f) && 
                         TB_FLOAT32_EQUAL(uvRect.width,0.0f) && 
                         TB_FLOAT32_EQUAL(uvRect.height,0.0f));
    resultRectUV.x += m_OffsetUV.GetX();
    resultRectUV.y += m_OffsetUV.GetY();
    if( m_Texture && isValidUV ){
        TbDrawRectTextureParam param( m_Texture);
        param.pos.SetXY( rect.x , rect.y );
        param.size.width = rect.width;
        param.size.height = rect.height;
        param.matrix = drawMatrix;
        param.color = resultColor;
        param.rectUV = resultRectUV;
        TbDrawRectTexture(  param );
    }else{
        TbDrawRectParam param;
        param.pos.SetXY( rect.x , rect.y );
        param.size.width = rect.width;
        param.size.height = rect.height;
        param.matrix = drawMatrix;
        param.color = resultColor;
        TbDrawRect( param );
    }
}

/*!
 * 描画終了
 * @author Miyake Shunsuke
 * @since 2012.02.26
 */
void TbSpriteFixedMaterial::EndDraw()
{
    m_Texture ? TbDraw2dTextureEnd() : TbDraw2dEnd();
}

}
