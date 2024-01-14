/*!
 * 編集モード
 * @author teco
 */

#ifndef _INCLUDED_EIDIT_MODE_BASE_H_
#define _INCLUDED_EIDIT_MODE_BASE_H_

#include <base/math/tb_vector2.h>
#include "edit/edit_types.h"
#include "edit/edit_util.h"
#include "side_action/stage/side_action_stage.h"
#include <input/tb_mouse.h>

class EditModeBase
{
public:

    // コンストラクタ
    EditModeBase( const char* name )
        : m_Name(name)
        , m_Stage(nullptr)
    {}

    // デストラクタ
    virtual ~EditModeBase() {}

public:

    // 名前取得
    const char* GetName() const {
        return m_Name.GetBuffer();
    }

    // ステージ設定
    void SetStage(SideActionStage* stage) {
        m_Stage = stage;
    }

    // ステージ取得
    SideActionStage* GetStage() {
        return m_Stage;
    }

public:

    // 初期化
    virtual void Initialize() {}

public:

    // 定期処理
    virtual void OnUpdate(TbFloat32 deltasec) {}

    // アクティブ状態が変化した
    virtual void OnChangeActive( TbBool isActive ) {}

    // 描画処理
    virtual void OnDraw(TbPointS32* pos) {}

private:

    TbStaticString32    m_Name;
    SideActionStage*    m_Stage;

};

#endif