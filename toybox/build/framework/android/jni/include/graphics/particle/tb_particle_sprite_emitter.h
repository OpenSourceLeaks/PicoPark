#ifndef _INCLUDED_TB_PARTICLE_SPRITE_EMITTER_H_
#define _INCLUDED_TB_PARTICLE_SPRITE_EMITTER_H_

#include <graphics/particle/tb_particle_sprite.h>

namespace toybox
{

class TbParticleSpriteScene;

class TbParticleSpriteEmitter
{
public:

    struct Param
    {
        TbVector2   initPos;
        TbVector2   spriteVelo;
        TbVector2   spriteAccel;
        TbVector2   spriteRandomVeloMin;
        TbVector2   spriteRandomVeloMax;
        TbVector2   spriteRandomAccelMin;
        TbVector2   spriteRandomAccelMax;
    };

public:

    // �R���X�g���N�^
    TbParticleSpriteEmitter();

    // �f�X�g���N�^
    ~TbParticleSpriteEmitter();

public:

    // ������
    void Initialize( const Param& param );

public:

    // ����
    void Enter( TbParticleSpriteScene* scene );

    // �o��
    void Exit();

public:

    // �������
    void Update( TbFloat32 deltaSec );

private:

    Param                   m_Param;
    TbParticleSpriteScene*  m_Scene;

};

}

#endif