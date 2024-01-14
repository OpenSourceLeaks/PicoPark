/*!
* ステージエディットシーケンス
* @author teco
*/

#include "fwd.h"
#include "seq_stage_edit.h"
#include "seq_manager.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/input/cr_input.h>
#include "common/cmn_util.h"
#include "root.h"
#include <base/io/tb_file_all.h>
#include <base/util/tb_bind.h>
#include <base/save/tb_save_file.h>
#include <base/io/stream/tb_stream_text_writer.h>
#include <base/io/stream/tb_file_stream.h>
#include <base/io/stream/tb_memory_stream.h>

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStageEdit::SeqStageEdit( TbSint32 id )
    : Super(SEQ_STAGE,"SeqStageEdit")
    , m_Stage(nullptr)
    , m_Id(id < 0 ? 100+id : id)
{
    CrInput::SetModeBit(EDIT_INPUT_MODE);
    SeqManager& seqManager = SeqManager::GetInstance();
    TbStaticString32 scriptName = seqManager.GetStageName(m_Id);
    const char* dir = seqManager.GetStageDir(m_Id);
    setupStage(scriptName.GetBuffer(), dir);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.07
 */
SeqStageEdit::~SeqStageEdit()
{
    CrInput::SetModeBit(CrInput::MODE_BIT_DEFAULT);
    TB_SAFE_DELETE(m_Stage);
}

/*!
 * 更新
 * @author teco
 * @since 2013.09.07
 */
void SeqStageEdit::OnUpdate()
{
    updateStage();
    m_EditModeManager.Update(1/60.0f);
    if (TbKeyboard::IsOn(TB_KEY_CONTROL_L))
    {
        if(TbKeyboard::IsPress(TB_KEY_S)){
            saveStage();
        }else if (TbKeyboard::IsPress(TB_KEY_R)) {
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE_EDIT, m_Id);
        }else if (TbKeyboard::IsPress(TB_KEY_E)) {
            SeqManager::GetInstance().RequestSequence(SEQ_STAGE, m_Id);
        }
    }
}

/*!
 * 描画関数
 * @author teco
 * @since 2013.10.27
 */
void SeqStageEdit::OnDraw() 
{
    if( m_Stage ){
        m_Stage->Draw();
    }
    m_EditModeManager.Draw();
}

/*!
 * ステージセットアップ
 * @author teco
 * @since 2013.10.27
 */
void SeqStageEdit::setupStage( const char* scriptName  , const char* dirName )
{
    TbScriptLuaManager& script = TbScriptLuaManager::GetInstance();
    TbScriptLuaNode* node = script.GetVM(0)->GetNode(0);

    // 共通luaを読み込み
    TbScriptLuaNode::Result res = node->DoFileSync("stage/stage_common.lua");

    // 個別luaを読み込み
    TbStaticString128 name = scriptName;
    TbStaticString32 dir;
    if (dirName && dirName[0] ) {
        dir += dirName;
        dir += "/";
    }else{
        dir += CMN_STAGE_DIR;
    }
    TbStaticString128 path = TbStaticString128::Format("stage/%s%s.lua", dir.GetBuffer(),name.GetBuffer());
    res = node->DoFileSync(path.GetBuffer());
    if( TB_VERIFY( res == TbScriptLuaNode::RESULT_OK ) )
    {
        node->GoToAbsolutely(name);
        SideActionStage* stage = new SideActionStage(node);
        if(stage) {
            stage->SetId(m_Id);
            stage->SetPauseScroll(TB_TRUE);
            stage->SetEnableCreateActor(TB_FALSE);
            stage->SetActive(TB_TRUE);
        }
        m_EditModeManager.Initialize(stage);
        m_Stage = stage;
    }
}

/*!
 * ステージ更新
 * @author teco
 * @since 2013.10.27
 */
void SeqStageEdit::updateStage()
{
    if (m_Stage) {
        m_Stage->Update();
    }
}

/*!
 * ステージセーブ
 * @author teco
 */
void SeqStageEdit::saveStage()
{
    if( !m_Stage ){
        return;
    }
    SeqManager& seqManager = SeqManager::GetInstance();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(m_Stage);
    TbStaticString32 scriptName = seqManager.GetStageName(m_Id);
    TbStaticString128 fileName;
    fileName.SetFormatedString("%s.lua", scriptName.GetBuffer());
    CrActionMap& map = stage->GetMap();

    TbFloat32 scale = stage->GetScale();
    TbFloat32 chipSize = map.GetChipSize() * scale;
    TbUint32  xPerPage = TbRoundUp(static_cast<TbFloat32>(CMN_WINDOW_WIDTH)/chipSize);

    TbSaveFile file;
    file.Initialize(fileName.GetBuffer(), TB_FILE_SYNC);
    {
        static const TbUint32 BUFSIZE = 1024 * 64;
        TbChar8* buf = new TbChar8[BUFSIZE]; // 最大64K
        TbMemoryStream stream(buf, BUFSIZE);
        TbStreamTextWriter writer(&stream);
        writer.Printf("%s.map.height=%d\n",scriptName.GetBuffer(), map.GetTileHeight());
        writer.Printf("%s.map.width=%d\n", scriptName.GetBuffer(), map.GetTileWidth());
        writer.Printf("%s.map.chipSize=%d\n", scriptName.GetBuffer(), map.GetChipSize());
        writer.Printf("%s.map.variable=%d\n", scriptName.GetBuffer(), map.IsVariable());
        writer.Printf("%s.map.table = \n", scriptName.GetBuffer());
        writer.Printf("{\n");
        writer.Printf("    -- page 1 \n");
        for (TbUint32 x = 0; x < map.GetTileWidth(); ++x)
        {
            writer.Printf("    ");
            for (TbUint32 y = 0; y < map.GetTileHeight(); ++y)
            {
                TbUint32 mapChip =  map.GetMapChip(x,y);
                writer.WriteString(EditUtil::GetMapChipName(mapChip));
                if (x != map.GetTileWidth() - 1 || y != map.GetTileHeight() - 1)
                {
                    writer.Printf(",");
                }
            }
            writer.Printf("  -- %d \n",x);

            // 1ページ
            if ( 0 < x && x%xPerPage == 0)
            {
                TbUint32 page = (x/xPerPage)+1;
                writer.Printf("    -- page %d \n", page);
            }
        }
        writer.Printf("}\n\0");
        writer.WriteEOFChar8();

        TbSizeT fileSize = TbStrCharCount(buf);
        file.Write(buf, fileSize);
        TB_SAFE_DELETE_ARRAY(buf);
    }
    CmnUtil::OpenTextFileByEditor(fileName.GetBuffer(), TB_FILE_PATH_ID_PRESET_SAVE);

}
