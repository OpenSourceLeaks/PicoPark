/*!
 * �G�f�B�b�g�Ǘ��N���X
 * @author teco
 */

#include "fwd.h"
#include "edit/edit_mode_manager.h"
#include "edit/edit_mode_base.h"
#include "edit/edit_mode_map.h"
#include "edit/edit_mode_actor.h"
#include "edit/edit_mode_setting.h"
#include "root.h"
#include <input/tb_mouse.h>

/*!
 * �R���X�g���N�^
 * @author teco
 */
EditModeManager::EditModeManager()
    : m_CurrentModeIndex(0)
    , m_Stage(nullptr)
{
}

/*!
 * ������
 * @author teco
 */
void EditModeManager::Initialize(SideActionStage* stage)
{
    m_Stage = stage;
    m_Mode.PushBack(TB_NEW EditModeMap());
    m_Mode.PushBack(TB_NEW EditModeActor());
    m_Mode.PushBack(TB_NEW EditModeSetting());
    TbForIterator(it, m_Mode) {
        (*it)->SetStage(stage);
        (*it)->Initialize();
    }
    if (m_CurrentModeIndex < m_Mode.GetCount()) {
        EditModeBase* mode = m_Mode[m_CurrentModeIndex];
        mode->OnChangeActive(TB_TRUE);
    }
}
    
/*!
 * ��n��
 * @author teco
 */
void EditModeManager::Finalize()
{
    TbForIterator(it, m_Mode) {
        TB_SAFE_DELETE(*it);
    }
}

/*!
 * �������
 * @author teco
 */
void EditModeManager::Update(TbFloat32 deltasec)
{
    if (m_CurrentModeIndex < m_Mode.GetCount()) {
        EditModeBase* mode = m_Mode[m_CurrentModeIndex];
        mode->OnUpdate(deltasec);
    }
}

/*!
 * �`�揈��
 * @author teco
 */
void EditModeManager::Draw()
{
    TbPointS32 point(25,1);
    TbFontBuffer2d* font = Root::GetInstance().GetDebugFont();
    font->PushSetting();
    font->SetFontSize(32);
    font->SetEnableBorder(TB_TRUE);
    EditUtil::PrintfLine(&point,"======================");
    EditUtil::PrintfLine(&point,"MODE=%s",getCurrentModeName());
    if (m_CurrentModeIndex < m_Mode.GetCount()) {
        EditModeBase* mode = m_Mode[m_CurrentModeIndex];
        mode->OnDraw(&point);
    }
    EditUtil::PrintfLine(&point,"======================");
    font->PopSetting();
}
    