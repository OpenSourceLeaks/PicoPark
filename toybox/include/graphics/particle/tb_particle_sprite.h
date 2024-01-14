#ifndef _INCLUDED_TB_PARTICLE_SPRITE_H_
#define _INCLUDED_TB_PARTICLE_SPRITE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>

namespace toybox
{

class TbParticleSpriteScene;

// スプライトパーティクル
class TbParticleSprite
{
public:

    enum
    {
        FLAG_BIT_FADE_OUT_END = TB_BIT(0) , // 生存終了持にフェードアウト
    };

    struct Param
    {
        Param()
            : flags(0U)
            , initAlpha(1.0f)
            , lifeSec(0.0f)
        {
        }
        TbUint32  flags;
        TbVector2 initPos;      // 初期座標
        TbVector2 velo;         // 速度
        TbVector2 accel;        // 加速度
        TbFloat32 initAlpha;    // 初期半透明度
        TbFloat32 lifeSec;      // 生存時間
    };

public:

    // コンストラクタ
    TbParticleSprite();

    // デストラクタ
    ~TbParticleSprite();

public:

    // 初期化
    void Initialize( const Param& param , TbParticleSpriteScene* scene );

    // 後始末
    void Finalize();

public:

    void Update( TbFloat32 deltaSec );

private:

    TbVector2   m_Pos;    // 位置
    Param       m_Param;  // パラメータ
    TbSprite    m_Sprite; // スプライト

};

}

#endif

