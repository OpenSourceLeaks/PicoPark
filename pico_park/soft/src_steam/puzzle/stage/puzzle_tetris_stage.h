/*!
 * パズル基底ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_TETRIS_STAGE_H_
#define _INCLUDED_PUZZLE_TETRIS_STAGE_H_

#include "common/stage/stage.h"
#include "puzzle/map/puzzle_tetris_map.h"
#include "puzzle/actor/puzzle_block_tetris.h"
#include <base/container/tb_static_array.h>
#include <script/lua/tb_script_lua.h>

class PuzzleTetrisStage : public Stage
{
    typedef PuzzleTetrisStage Self;
    typedef Stage Super;

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

protected:
    
    // 更新
    void OnUpdate( TbFloat32 deltatime );

    // アクター生成
    virtual void OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData &userData);

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
