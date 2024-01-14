/*!
 * パズル基底ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_TETRIS_STAGE_H_
#define _INCLUDED_PUZZLE_TETRIS_STAGE_H_

#include "puzzle/map/puzzle_tetris_map.h"
#include "puzzle/actor/puzzle_block_tetris.h"
#include <base/container/tb_static_array.h>
#include <script/lua/tb_script_lua.h>
#include <crayon/stage/cr_stage.h>

class PuzzleTetrisStage : public CrStage
{
    typedef PuzzleTetrisStage Self;
    typedef CrStage Super;

public:

    enum BlockType
    {
        BLOCK_TYPE_4 ,
        BLOCK_TYPE_3
    };

public:

    // コンストラクタ
    PuzzleTetrisStage( TbScriptLuaNode* node , BlockType type );

    // デストラクタ
    virtual ~PuzzleTetrisStage();

public:

    // ノルマ設定
    void SetNorma( TbUint32 norma ) {
        m_Map.SetNormaLineCount( norma );
    }

    // ライン数取得
    TbUint32 GetLineCount() const {
        return m_Map.GetLineCount();
    }

protected:
    
    // 更新
    void OnUpdate( TbFloat32 deltatime ) TB_OVERRIDE;

    // アクター生成
    virtual CrActor* OnCreatedActor(const CrActorFactory::CreateParam& param) TB_OVERRIDE;

protected:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent );

private:

    // フラグ
    enum Flag
    {
        FLAG_GAME_OVER , 
    };

private:

    BlockType           m_Type;
    TbBitArray32        m_BitArray;         // ビット配列
    PuzzleTetrisMap     m_Map;              // マップ

};

#endif
