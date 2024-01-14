/*!
 * エディット管理クラス
 * @author teco
 */

#ifndef _INCLUDED_EDIT_MODE_MANAGER_H_
#define _INCLUDED_EDIT_MODE_MANAGER_H_

#include <base/container/tb_static_array.h>
#include "edit/edit_mode_base.h"

class EditModeManager
{
public:

    // コンストラクタ
    EditModeManager();

    // デストラクタ
    virtual ~EditModeManager() {
        Finalize();
    }

public:

    // 初期化
    void Initialize( SideActionStage* stage );
    
    // 後始末
    void Finalize();

public:

    // 定期処理
    void Update(TbFloat32 deltasec);

    // 描画処理
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

    // 出力
    void printf( TbPointS32* pos, const char* str);
    void printf( TbPointS32* pos , const char* param , const char* value );

private:

    TbUint32                                m_CurrentModeIndex;
    TbStaticArray<EditModeBase*,MODE_MAX>   m_Mode;
    SideActionStage*                        m_Stage;
};

#endif