/*!
 * タイトルシーケンス
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_chara_select.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>
#include "side_action/stage/side_action_stage.h"
#include "system/sys_input.h"
#include <input/tb_pad.h>

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.07
 */
SeqCharaSelect::SeqCharaSelect()
    : Super(SEQ_TITLE,"SeqCharaSelect")
    , m_Stage(NULL)
    , m_PlayerCount(0)
{
    // ステージ構築
    {
        TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
        TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);
        TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");
        res = node->DoFileSync("stage/" CMN_STAGE_DIR "stage_051.lua");
        node->GoToAbsolutely("stage_051");
        m_Stage = new SideActionStage(node);
        node->GoToAbsolutely("");
        m_Stage->SetActive(TB_TRUE);
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.09.07
 */
SeqCharaSelect::~SeqCharaSelect()
{
    TB_SAFE_DELETE(m_Stage);
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2013.09.07
 */
void SeqCharaSelect::OnUpdate()
{
    if( m_Stage ){
        m_Stage->Update();
    }
}

/*!
 * 描画関数
 * @author Miyake_Shunsuke
 * @since 2013.10.27
 */
void SeqCharaSelect::OnDraw()
{
    if( m_Stage ){
        m_Stage->Draw();
    }
}
