/*!
 * スプライトマテリアル
 * @author Miyake Shunsuke
 * @since 2011.12.07
 */

#ifndef _INCLUDED_TB_SPRITE_MATERIAL_H_
#define _INCLUDED_TB_SPRITE_MATERIAL_H_

#include <base/math/tb_geometry.h>
#include <graphics/render/3dapi/tb_color.h>
#include <graphics/sprite/tb_sprite_types.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_matrix.h>

namespace toybox
{

class TbTexture;

class TbSpriteMaterial
{
public:

    // ステップフレーム
    virtual void StepFrame( TbFloat32 stepFrame ) = 0;

    // 描画開始
    virtual void BeginDraw() = 0;

    // 描画
    virtual void Draw( const TbRectF32& rect , 
                       const TbRectF32& uvRect , 
                       const TbColor&        color , 
                       const TbMatrix& drawMatrix ) = 0;

    // 描画終了
    virtual void EndDraw() = 0;

};

// 固定スプライトマテリアルアニメーション元データ
struct TbSpriteFixedMaterialAnim
{
    TbAnimationKeyFrameColorR8G8B8A8 color;
    TbAnimationKeyFrameVec2f         offsetUV;
};

// 固定スプライトマテリアル
class TbSpriteFixedMaterial : public TbSpriteMaterial
{
public:

    // コンストラクタ
    TbSpriteFixedMaterial( TbUint32 passIndex );
    
    // デストラクタ
    virtual ~TbSpriteFixedMaterial();

public:

    // ステップフレーム
    virtual void StepFrame( TbFloat32 stepFrame );

    // 描画開始
    virtual void BeginDraw();

    // 描画
    virtual void Draw( const TbRectF32& rect , 
                       const TbRectF32& uvRect , 
                       const TbColor&   color , 
                       const TbMatrix&  drawMatrix );

    // 描画終了
    virtual void EndDraw();

public:

    // マテリアルカラー設定
    void SetColor( TbColor color ){
        m_Color = color;
    }

    // マテリアルカラー取得
    const TbColor& GetColor() const {
        return m_Color;
    }

    // UVオフセット設定
    void SetOffsetUV( const TbVector2& offset ) {
        m_OffsetUV = offset;
    }

    // UVオフセット取得
    const TbVector2& GetOffsetUV() const {
        return m_OffsetUV;
    }

    // テクスチャ設定
    void SetTexture( TbTexture* texture ) {
        m_Texture = texture;
    }

    // テクスチャ取得
    TbTexture* GetTexture() {
        return m_Texture;
    }

private:
    TbUint32                        m_PassIndex; // 描画するレンダリングパス
    TbColor                         m_Color;     // 色
    TbVector2                       m_OffsetUV;  // UVオフセット座標
    TbTexture*                      m_Texture;   // テクスチャ
    TbSpriteFixedMaterialAnim       m_Anim;      // アニメーションデータ
};

}

#endif
