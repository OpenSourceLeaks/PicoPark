#ifndef _INCLUDED_CMN_SIMPLE_SPRITE_H_
#define _INCLUDED_CMN_SIMPLE_SPRITE_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <framework/sub/tb_framework_simple.h>

// シンプルスプライト
class CmnSimpleSprite : public toybox::TbSprite
{
    typedef toybox::TbSprite Super;
public:
    // コンストラクタ
    CmnSimpleSprite();

    // デストラクタ
    virtual ~CmnSimpleSprite()
    {
    }
private:
    toybox::TbSpriteFixedMaterial   m_Material;   // スプライト用マテリアル
};

// テクスチャがない用のスプライト
class CmnWhiteSprite : public CmnSimpleSprite
{
public:
    typedef CmnSimpleSprite Super;
public:

    CmnWhiteSprite()
        : Super()
    {
    }

public:

    // 初期化
    void Initialize(const TbRectF32& rect)
    {
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512.0f-8.0f,0.0f,8.0f,8.0f));
        Super::Initialize(rect,uvRect);
    }

    // αアニメーション着き
    void InitializeWithAlphaAnim(const TbRectF32& rect , const TbAnimationKeyFrameDataColorR32G32B32A32* animColor)
    {
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512.0f - 8.0f, 0.0f, 8.0f, 8.0f));
        TbSpriteNodeTree::InitNodeParam initParam;
        initParam.name = "root";
        initParam.param.color.SetColorU8(0xFF, 0xFF, 0xFF, 0xFF);
        initParam.param.rect = rect; //
        initParam.param.uvRect = uvRect;
        m_SpriteNode.Initialize(initParam);
        {
            TbSpriteNodeAnimTree::InitNodeParam animInitParam;
            animInitParam.name = "root";
            animInitParam.param.name = "color";
            animInitParam.param.colorAnim = animColor;
            m_SpriteAnim.Initialize(1, &animInitParam);
        }
        Super::Initialize(&m_SpriteNode,&m_SpriteAnim);
    }

private:

    TbSpriteNodeTree        m_SpriteNode;
    TbSpriteNodeAnimTree    m_SpriteAnim;

};

// カーソルスプライト
class CmnCursorSprite : public CmnSimpleSprite
{
    typedef CmnSimpleSprite Super;
public:

    // コンストラクタ
    CmnCursorSprite()
        : Super()
        , m_ScaleCos(0.0f)
    {
    }
    
    // デストラクタ
    virtual ~CmnCursorSprite()
    {
    }
public:

    // 定期処理
    void Update(TbFloat32 deltaSec)
    {
        static const TbFloat32 CURSOR_SPEED = 3.0f;
        static TB_NDEBUG_CONST TbFloat32 CURSOR_SCALE_MAX = 0.03f;
        m_ScaleCos += deltaSec*CURSOR_SPEED;

        TbFloat32 scale = 1.0f + CURSOR_SCALE_MAX + TbCos(m_ScaleCos) * CURSOR_SCALE_MAX;
        SetScale(scale);
        Calculate();
    }

private:
    TbFloat32		m_ScaleCos;
};

#endif
