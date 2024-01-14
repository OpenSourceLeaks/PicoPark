/*!
 * �ҏW���[�h
 * @author teco
 */

#ifndef _INCLUDED_EDIT_MODE_MAP_H_
#define _INCLUDED_EDIT_MODE_MAP_H_

#include "edit_mode_base.h"

class EditModeMap : public EditModeBase
{
public:

    typedef EditModeMap Self;
    typedef EditModeBase Super;

public:

    // �R���X�g���N�^
    EditModeMap();

    // �f�X�g���N�^
    virtual ~EditModeMap();

public:

    // ������
    void Initialize() TB_OVERRIDE;

public:

    // �������
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive( TbBool isActive ) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(TbPointS32* pos) TB_OVERRIDE;

private:

    enum MapChipType
    {
        MC_TYPE_NONE  ,
        MC_TYPE_BLOCK ,
        MC_TYPE_DOKAN , 
        MC_TYPE_BREAKOUT1,
        MC_TYPE_BREAKOUT2,
        MC_TYPE_BREAKOUT3,
        MC_TYPE_BREAKOUT4,
        MC_TYPE_MAX   ,
    };

private:

    // �}�b�v�`�b�v�ݒ�
    void setMapChip( CrActionMap* map , const TbPointS32& point );

    // �}�b�v�`�b�v�X�V
    void updateMapChip(CrActionMap* map, TbSint32 x , TbSint32 y);

    // �}�b�v�`�b�v�^�C�v�擾
    MapChipType getMapChipType( TbUint32 mapChip );

private:

    TbPointS32  m_Point;
    MapChipType m_MapChipType;
    TbUint32    m_FloorBlockY;

private:

    TbPointS32  m_MarkerBeginPoint;
    TbPointS32  m_MarkerEndPoint;

};

#endif