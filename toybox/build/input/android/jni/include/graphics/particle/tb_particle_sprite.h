#ifndef _INCLUDED_TB_PARTICLE_SPRITE_H_
#define _INCLUDED_TB_PARTICLE_SPRITE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>

namespace toybox
{

class TbParticleSpriteScene;

// �X�v���C�g�p�[�e�B�N��
class TbParticleSprite
{
public:

    enum
    {
        FLAG_BIT_FADE_OUT_END = TB_BIT(0) , // �����I�����Ƀt�F�[�h�A�E�g
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
        TbVector2 initPos;      // �������W
        TbVector2 velo;         // ���x
        TbVector2 accel;        // �����x
        TbFloat32 initAlpha;    // �����������x
        TbFloat32 lifeSec;      // ��������
    };

public:

    // �R���X�g���N�^
    TbParticleSprite();

    // �f�X�g���N�^
    ~TbParticleSprite();

public:

    // ������
    void Initialize( const Param& param , TbParticleSpriteScene* scene );

    // ��n��
    void Finalize();

public:

    void Update( TbFloat32 deltaSec );

private:

    TbVector2   m_Pos;    // �ʒu
    Param       m_Param;  // �p�����[�^
    TbSprite    m_Sprite; // �X�v���C�g

};

}

#endif

