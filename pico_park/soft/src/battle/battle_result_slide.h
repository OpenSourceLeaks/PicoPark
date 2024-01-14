/*!
* �o�g�����ʃy�[�W
* @author teco
*/

#ifndef _INCLUDED_BATTLE_RESULT_SLIDE_H_
#define _INCLUDED_BATTLE_RESULT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "battle/battle_manager.h"
#include "common/material/cmn_sprite_player_material.h"

class BattleResultPage : public CmnSlidePage
{
public:

    // �R���X�g���N�^
    BattleResultPage();

    // �f�X�g���N�^
    virtual ~BattleResultPage() {}

public:

    // �������
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // �A�N�e�B�u��Ԃ��ω�����
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // �`�揈��
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    struct PlayerInfo
    {
        CmnSimpleSprite         playerSprite;
        CmnSimpleSprite         helmetSprite;
        CmnSimpleSprite         winSpirte[BattleManager::WIN_MAX];
    };

private:

    // �v���C���[�ʒu�擾
    TbVector2 getPlayerPos( TbUint32 playerIndex );

private:

    PlayerInfo              m_PlayerInfo[CMN_PLAYER_MAX];
    CmnSpritePlayerMaterial m_PlayerMaterial;
    TbFloat32               m_Sec;
    TbBool                  m_IsNewWin;

};

class BattleResultSlide : public CmnSlide
{
    typedef CmnSlide Super;
public:

    // �R���X�g���N�^s
    BattleResultSlide();

    // �f�X�g���N�^
    virtual ~BattleResultSlide();

public:

    // �X�e�[�WID�擾
    TbUint32 GetStageId() const;

protected:

    // �������
    virtual void OnUpdate();

    // �`��
    virtual void OnDraw();

    // �y�[�W�ύX
    virtual void OnChangingPage(TbBool isNext) TB_OVERRIDE;

    // ���̃y�[�W�擾
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:
    BattleResultPage    m_BattleResultPage;
};

#endif