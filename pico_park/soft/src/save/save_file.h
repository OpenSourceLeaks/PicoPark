/*!
 * �Z�[�u�f�[�^ 
 */

#ifndef _INCLUDED_SAVE_FILE_H_
#define _INCLUDED_SAVE_FILE_H_

#include "save_data_struct.h"
#include <base/save/tb_save_file.h>
#include <graphics/sprite/tb_sprite_all.h>

class SaveFile : public TbSingletonRef<SaveFile>
{
public:

    // �R���X�g���N�^
    SaveFile();

    // �f�X�g���N�^
    ~SaveFile();

public:

    // ������
    void Initialize();

    // �Z�[�u
    void Save();

    // ���[�h
    void Load();

    // �ǂݍ��ݍς�
    TbBool IsLoaded() const {
        return m_IsLoaded;
    }

    // �r�W�[��Ԃ�
    TbBool IsBusy() const;

public:

public:

    // �v���C�񐔐ݒ�
    void SetPlayCount(TbUint32 stage, TbUint32 playerCount, TbUint32 playCount);

    // �v���C�񐔃J�E���g�A�b�v
    void IncrementPlayCount(TbUint32 stage, TbUint32 playerCount);

    // �v���C�񐔎擾
    TbUint32 GetPlayCount(TbUint32 stage, TbUint32 playerCount) const;

    // �N���A�񐔐ݒ�
    void SetClearCount(TbUint32 stage, TbUint32 playerCount, TbUint32 clearCount);

    // �N���A�񐔃J�E���g�A�b�v
    void IncrementClearCount(TbUint32 stage, TbUint32 playerCount);

    // �N���A�񐔎擾
    TbUint32 GetClearCount(TbUint32 stage, TbUint32 playerCount) const;

    // �N���A�񐔎擾(��ԗǂ���)
    TbUint32 GetClearCount(TbUint32 stage) const;

    // �N���A�񐔎擾
    TbUint32 GetTotalClearCount(TbUint32 stage) const;

    // �x�X�g�^�C���ݒ�
    TbBool SetBestMillSec(TbUint32 stage, TbUint32 playerCount, TbUint32 ms);

    // �x�X�g�^�C���擾
    TbUint32 GetBestMillSec(TbUint32 stage, TbUint32 playerCount) const;

    // �x�X�g�^�C���ݒ�
    TbBool SetHiscore(TbUint32 stage, TbUint32 playerCount, TbUint32 score);

    // �x�X�g�^�C���擾
    TbUint32 GetHiscore(TbUint32 stage, TbUint32 playerCount) const;

    // �W�����v�{�^���؂�ւ���
    TbBool IsChangeJumpButton() const;

    // �W�����v�{�^���؂�ւ���
    void SetChangeJumpButton( TbBool isChangeJumpButton );

    // �w�i�J���[�擾
    TbUint32 GetBgColorType() const;

    // �w�i�J���[�ݒ�
    void SetBgColorType( TbUint32 bgColor );

    // �T�E���h�{�����[�����x���ݒ�
    void SetSoundBgmVolumeLevel(TbSint32 level);

    // �T�E���h�{�����[�����x���擾
    TbSint32 GetSoundBgmVolumeLevel() const;

    // �v���C���[�ԍ��\��
    void SetVisiblePlayerNumber( TbBool isVisibile );

    // �v���C���[�ԍ��\��
    TbBool IsVisiblePlayerNumber() const;

public:

    // �������
    void Update();

    // �A�C�R���`��
    void DrawIcon();

public:

    // �f�[�^�擾
    const SaveData& GetData() const {
        return m_Data;
    }

private:

    enum
    {
        PHASE_IDLE , 
        PHASE_LOAD = 10 , 
        PHASE_SAVE = 20
    };

private:

    // �ǂݍ���
    void load();

private:

    TbSint32        m_Phase;
    TbBool          m_IsDirty;
    TbBool          m_IsLoaded;
    SaveData        m_Data;
    TbSaveFile*     m_SaveFile;
    TbSprite                m_IconSprite;
    TbSpriteFixedMaterial   m_IconSpriteMaterial;   // �X�v���C�g�p�}�e���A��
    TbFloat32               m_IconSpriteTimer;

};

#endif
