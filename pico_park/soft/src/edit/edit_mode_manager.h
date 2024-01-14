/*!
 * �G�f�B�b�g�Ǘ��N���X
 * @author teco
 */

#ifndef _INCLUDED_EDIT_MODE_MANAGER_H_
#define _INCLUDED_EDIT_MODE_MANAGER_H_

#include <base/container/tb_static_array.h>
#include "edit/edit_mode_base.h"

class EditModeManager
{
public:

    // �R���X�g���N�^
    EditModeManager();

    // �f�X�g���N�^
    virtual ~EditModeManager() {
        Finalize();
    }

public:

    // ������
    void Initialize( SideActionStage* stage );
    
    // ��n��
    void Finalize();

public:

    // �������
    void Update(TbFloat32 deltasec);

    // �`�揈��
    void Draw();
    
private:
    
    static const TbUint32 MODE_MAX = 16;

private:

    const char* getCurrentModeName() const {
        if (m_CurrentModeIndex < m_Mode.GetCount()) {
            return m_Mode[m_CurrentModeIndex]->GetName();
        }
        return "-";
    }

    // �o��
    void printf( TbPointS32* pos, const char* str);
    void printf( TbPointS32* pos , const char* param , const char* value );

private:

    TbUint32                                m_CurrentModeIndex;
    TbStaticArray<EditModeBase*,MODE_MAX>   m_Mode;
    SideActionStage*                        m_Stage;
};

#endif