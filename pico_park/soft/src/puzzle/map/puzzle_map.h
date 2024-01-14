/*!
 * パズル用マップ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_PUZZLE_MAP_H_
#define _INCLUDED_PUZZLE_MAP_H_

#include <base/math/tb_vector2.h>
#include <base/math/tb_geometry.h>
#include <base/util/tb_function.h>
#include "puzzle/actor/puzzle_block_base.h"
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <crayon/map/cr_tile_map.h>

enum
{
    PUZZLE_MAP_CHIP_PRE_BLOCK = CR_TILE_MAP_CHIP_USER_BEGIN , 
    PUZZLE_MAP_CHIP_PRE_BLOCK_END = PUZZLE_MAP_CHIP_PRE_BLOCK + CMN_PLAYER_MAX ,
    
    PUZZLE_MAP_CHIP_POST_BLOCK = PUZZLE_MAP_CHIP_PRE_BLOCK_END ,
    PUZZLE_MAP_CHIP_POST_BLOCK_END = PUZZLE_MAP_CHIP_POST_BLOCK + CMN_PLAYER_MAX ,

    PUZZLE_MAP_CHIP_TARGET_BLOCK = PUZZLE_MAP_CHIP_POST_BLOCK_END ,
    PUZZLE_MAP_CHIP_TARGET_BLOCK_END = PUZZLE_MAP_CHIP_TARGET_BLOCK + CMN_PLAYER_MAX ,

    PUZZLE_MAP_CHIP_MAX = PUZZLE_MAP_CHIP_TARGET_BLOCK_END
};

class PuzzleMap : public CrTileMap
{
public:

    typedef PuzzleMap Self;
    typedef CrTileMap Super;

public:

    // マップチップのUV情報取得
    static TbRectF32 GetMapChipUV( TbUint32 mapChip );

public:

    // コンストラクタ
    PuzzleMap();

    // デストラクタ
    virtual ~PuzzleMap();

public:

    // 落下時間取得
    TbFloat32 GetFallTime() const {
        return m_FallTime;
    }

    // 落下時間設定
    void SetFallTime( TbFloat32 fallTime ) {
        m_FallTime = fallTime;
    }

    // 床落下時間取得
    TbFloat32 GetFallFloorTime() const {
        return TbMax(m_FallTime,m_FallFloorTime);
    }

    // 床落下時間設定
    void SetFallFloorTime(TbFloat32 fallTime) {
        m_FallFloorTime = fallTime;
    }

    // スタート点取得
    TbPointS32 GetStartPoint( TbUint32 playerIndex ) const;

    // 判定領域取得
    const TbRectS32& GetJudgeRect() const {
        return m_JudgeRect;
    }

    // 情報表示座標取得
    const TbVector2& GetInfoDispPos() const {
        return m_InfoDispPos;
    }

    // プレイヤーブロック数取得
    TbUint32 GetPlayerBlockCount() const {
        return m_PlayerBlock.GetCount();
    }

    // プレイヤーブロック取得
    PuzzleBlockBase* GetPlayerBlock( TbUint32 index ) {
        if( index < m_PlayerBlock.GetCount() ){
            return m_PlayerBlock[index];
        }
        return nullptr;
    }

    // プレイヤーブロック追加
    void AddPlayerBlock( PuzzleBlockBase* block ) {
        block->Retain();
        m_PlayerBlock.PushBack(block);
    }

    // ゲームオーバーしたか
    TbBool IsGameOver() const {
        TbBool result = TB_TRUE; 
        for( BlockList::ConstIterator ite = m_PlayerBlock.Begin() , endIte = m_PlayerBlock.End();
                ite != endIte; ++ite ) 
        {
            if( (*ite)->IsActive() && !(*ite)->IsEnd() ){
                result = TB_FALSE;
            }
        }
        return result;
    }

public:

    // 初期化
    virtual void OnInitialize( TbScriptLuaNode* node );

    // デバッグメニュー構築
    virtual void OnCreateDebugMenu( TbDebugMenu* menu );

protected:

    // 背景描画
    virtual void OnDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material ) TB_OVERRIDE;

private:

    typedef TbStaticArray<PuzzleBlockBase*,CMN_PLAYER_MAX> BlockList;

private:

    TbVector2       m_InfoDispPos;  // 情報表示位置
    TbRectS32       m_JudgeRect;    // 判定領域]
    BlockList       m_PlayerBlock;  // ユーザーが操作可能なブロックリスト
    TbFloat32       m_FallTime;         // 落下時間
    TbFloat32       m_FallFloorTime;

};

#endif
