/*!
 * パズル基底ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_DR_STAGE_H_
#define _INCLUDED_PUZZLE_DR_STAGE_H_

#include "puzzle/map/puzzle_dr_map.h"
#include "puzzle/actor/puzzle_block_dr.h"
#include <base/container/tb_static_array.h>
#include <script/lua/tb_script_lua.h>
#include <crayon/stage/cr_stage.h>

class PuzzleDrStage : public CrStage
{
    typedef PuzzleDrStage Self;
    typedef CrStage Super;

public:

    // コンストラクタ
    PuzzleDrStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~PuzzleDrStage();


protected:
    
    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // アクター生成
    virtual CrActor* OnCreatedActor( const CrActorFactory::CreateParam& param ) TB_OVERRIDE;

protected:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent );

private:

    TbBitArray32    m_BitArray;         // ビット配列
    PuzzleDrMap     m_Map;              // マップ

};

#endif
