/*!
 * �Z�[�u�f�[�^ 
 */

#ifndef _INCLUDED_SAVE_DATA_STRUCT_H_
#define _INCLUDED_SAVE_DATA_STRUCT_H_

#include "common/cmn_def.h"

static const TbUint16 SAVE_DATA_VERSION = 0x0100;

// �w�b�_�[
struct SaveDataHeader
{
    TbUint16 version;   // �o�[�W����
    TbUint16 reserved;
    TbUint32 size;      // �T�C�Y
    TbUint32 reserved2[2];
};
void SaveInitialize(SaveDataHeader* header);
TB_STATIC_ASSERT(sizeof(SaveDataHeader) == 16);

// �ݒ�
struct SaveDataSetting
{
    enum
    {
        BIT_CHANGE_JUMP_BUTTON    = TB_BIT(0),
        BIT_CHANGE_CREAM_BG       = TB_BIT(1),
        BIT_VISIBLE_PLAYER_NUMBER = TB_BIT(2)
    };
    TbUint8  volume;
    TbUint8  buttonType;
    TbUint16 bitArray;
    TbUint32 reserved2[31]; // PC�p�l��(�L�[�R���t�B�O�A�t���X�N���[�����)
};
void SaveInitialize(SaveDataSetting* data);
TB_STATIC_ASSERT(sizeof(SaveDataSetting) == 128);

// ���̓X�e�[�W
struct SaveDataStage
{
    static const TbUint32 BEST_TIME_INVALID = 0xFFFFFFFF;
    struct PlayersData
    {
        TbUint32    playCount;
        TbUint32    clearCount;
        TbUint32    bestTime;
        TbUint32    hiscore;
        TbUint32    reserve;
    };
    PlayersData data[CMN_PLAYER_MAX];
};
void SaveInitialize(SaveDataStage* data);
TB_STATIC_ASSERT(sizeof(SaveDataStage)==200);

// ���̓��[���h
struct SaveDataCoopWorld
{
    static const        TbUint32 LEVEL_MAX = 8;
    TbBool              isOpened;
    TbUint32            reserved[15];
};
void SaveInitialize(SaveDataCoopWorld* data);
TB_STATIC_ASSERT(sizeof(SaveDataCoopWorld)==64);

// ���̓��[�h
struct SaveDataCoop
{
    static const TbUint32 WORLD_MAX = 128;
    SaveDataCoopWorld    world[WORLD_MAX];
};
void SaveInitialize(SaveDataCoop* data);

struct SaveData
{
    static const TbUint32 STAGE_MAX = 128;
    SaveDataHeader  header;
    SaveDataSetting setting;
    SaveDataCoop    coop;
    SaveDataStage   stage[STAGE_MAX];
};
void SaveInitialize(SaveData* data);

#endif
