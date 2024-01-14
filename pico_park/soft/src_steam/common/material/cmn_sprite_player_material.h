/*!
 * プレイヤー用マテリアル
 * @author teco
 */

#ifndef _INCLUDED_CMN_MATERIAL_PLAYER_H_
#define _INCLUDED_CMN_MATERIAL_PLAYER_H_

#include <graphics/material/tb_material.h>
#include <graphics/render/3dapi/tb_shader_manager.h>
#include <graphics/sprite/tb_sprite_material.h>

// プレイヤー用マテリアル
class CmnSpritePlayerMaterial : public TbSpriteMaterial
{
public:

    // コンストラクタ
    CmnSpritePlayerMaterial();

    // デストラクタ
    virtual ~CmnSpritePlayerMaterial();

public:

    // ステップフレーム
    virtual void StepFrame(TbFloat32 stepFrame) {}

    // 描画開始
    virtual void BeginDraw();

    // 描画
    virtual void Draw(const TbRectF32& rect,
        const TbRectF32& uvRect,
        const TbColor&   color,
        const TbMatrix&  drawMatrix);

    // 描画終了
    virtual void EndDraw();

public:

    // マテリアルカラー設定
    void SetColor(TbColor color) {
        m_Color = color;
    }

    // マテリアルカラー取得
    const TbColor& GetColor() const {
        return m_Color;
    }

    // テクスチャ設定
    void SetTexture(TbTexture* texture) {
        m_Texture = texture;
    }

    // テクスチャ取得
    TbTexture* GetTexture() {
        return m_Texture;
    }

private:
    TbUint32                        m_PassIndex;    // 描画するレンダリングパス
    TbColor                         m_Color;        // 色
    TbTexture*                      m_Texture;      // テクスチャ
    TbShaderConstHandle             m_MatrixHandle;  // 

};

#endif
