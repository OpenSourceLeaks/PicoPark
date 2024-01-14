/*!
 * パズル基底ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_DR_STAGE_H_
#define _INCLUDED_PUZZLE_DR_STAGE_H_

#include "common/stage/stage.h"
#include "puzzle/map/puzzle_dr_map.h"
#include "puzzle/actor/puzzle_block_dr.h"
#include <base/container/tb_static_array.h>
#include <script/lua/tb_script_lua.h>

class PuzzleDrStage : public Stage
{
    typedef PuzzleDrStage Self;
    typedef Stage Super;

public:

    // コンストラクタ
    PuzzleDrStage( TbScriptLuaNode* node );

    // デストラクタ
    virtual ~PuzzleDrStage();


protected:
    
    // 更新
    void OnUpdate( TbFloat32 deltatime );

    // アクター生成
    virtual void OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData &userData );

protected:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent );

private:

    TbBitArray32    m_BitArray;         // ビット配列
    PuzzleDrMap     m_Map;              // マップ

};

#endif
