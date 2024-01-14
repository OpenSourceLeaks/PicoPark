/*!
* 編集モード
* @author teco
*/

#ifndef _INCLUDED_EDIT_MODE_ACTOR_H_
#define _INCLUDED_EDIT_MODE_ACTOR_H_

#include "edit_mode_base.h"

class EditModeActor : public EditModeBase
{
public:

    typedef EditModeActor Self;
    typedef EditModeBase Super;

public:

    // コンストラクタ
    EditModeActor();

    // デストラクタ
    virtual ~EditModeActor();

public:

    // 初期化
    void Initialize() TB_OVERRIDE;

public:

    // 定期処理
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // アクティブ状態が変化した
    void OnChangeActive(TbBool isActive) TB_OVERRIDE;

    // 描画処理
    void OnDraw(TbPointS32* pos) TB_OVERRIDE;

};

#endif