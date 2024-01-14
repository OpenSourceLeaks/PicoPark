/*!
 * パズル基底ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_VS_DR_STAGE_H_
#define _INCLUDED_PUZZLE_VS_DR_STAGE_H_

#include "puzzle/map/puzzle_dr_map.h"
#include "puzzle/actor/puzzle_block_dr.h"
#include "puzzle/actor/puzzle_timer.h"
#include <base/container/tb_static_array.h>
#include <script/lua/tb_script_lua.h>
#include "ui/ui_message_window.h"
#include <crayon/stage/cr_stage.h>

class PuzzleVsDrStage : public CrStage
{
    typedef PuzzleVsDrStage Self;
    typedef CrStage Super;

public:

    // コンストラクタ
    PuzzleVsDrStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~PuzzleVsDrStage();


protected:
    
    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // アクター生成
    virtual CrActor* OnCreatedActor(const CrActorFactory::CreateParam& param) TB_OVERRIDE;

protected:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent );

private:

    enum Team
    {
        TEAM_INVALID = -1 ,
        TEAM_LEFT ,
        TEAM_RIGHT ,
        TEAM_MAX
    };

    enum
    {
        PHASE_PLAY , 
        PHASE_RESULT = 10 ,
    };

private:

    TbSint32        m_Phase;
    TbBitArray32    m_BitArray;         // ビット配列
    PuzzleDrMap     m_Map[TEAM_MAX];              // マップ
    PuzzleTimer*    m_Timer;
    Team            m_Winner;

private:

    UiMessageWindow m_MessageWindow;

};

#endif
