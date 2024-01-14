/*!
 * �p�[�e�B�N���Ǘ��N���X
 */

#ifndef _INCLUDED_TB_PARTICLE_MANAGER_
#define _INCLUDED_TB_PARTICLE_MANAGER_

#include <base/math/tb_matrix.h>
#include <base/container/tb_static_array.h>
#include <graphics/particle/tb_particle_sprite_emitter.h>
#include <graphics/particle/tb_particle_sprite.h>
#include <graphics/sprite/tb_sprite_all.h>

namespace toybox
{

class TbParticleSpriteScene
{
public:

    static const TbUint32 EMITTER_MAX = 32;
    static const TbUint32 PARTICLE_MAX  = 1024;

public:

    // �R���X�g���N�^
    TbParticleSpriteScene();

    // �f�X�g���N�^
    ~TbParticleSpriteScene();

public:

    // ������
    void Initlaize();
    
    // �X�v���C�g�V�[��
    TbSpriteScene* GetSpriteScene() {
        return m_SpriteScene;
    }

public:

    // �������
    void Update();

    // �`��
    void Draw();

private:

    typedef TbStaticArray<TbParticleSpriteEmitter*, EMITTER_MAX> EmitterArray;
    typedef TbStaticArray<TbParticleSprite*, PARTICLE_MAX>       ParticleArray;

private:

    TbMatrix        m_PoseMatrix;
    EmitterArray    m_EmitterArray;
    ParticleArray   m_ParticleArray;
    TbSpriteScene*  m_SpriteScene;

};

}

#endif
